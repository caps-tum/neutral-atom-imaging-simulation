#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "settings.h"
#include "distributionSampling.h"
#include "imageModulation.h"

#define EulerMascheroni 0.5772156649015328606065120900824024310422

void initImageAndSimulateOpticalEffects(double *image, int imageHeight, int imageWidth, const double atomLocations[][2], 
    double *truth, const double zernikeCoefficients[15], int atomCount, int approximationSteps)
{
    double fractionalSolidAngle = (1 - sqrt(1 - simulationSettings.numericalAperture * simulationSettings.numericalAperture)) / 2;
    double photonsPerAtom = fractionalSolidAngle * simulationSettings.scatteringRate * simulationSettings.exposureTime * simulationSettings.quantumEfficiency;

    memset(image, 0, imageWidth * imageHeight * sizeof(double) * 4);

    double gaussianNormalizationFactor = 1;
    double effectiveLightSourceStdev = simulationSettings.lightSourceStdev * approximationSteps;
    if(effectiveLightSourceStdev > 0)
    {
        gaussianNormalizationFactor = 1 / (2 * M_PI * effectiveLightSourceStdev * effectiveLightSourceStdev);
    }

    unsigned short anyAtomWithinSight = 0;
    for (int a = 0; a < atomCount; a++)
    {
        if(truth)
        {
            while((*truth) < 0.5)
            {
                truth++;
            }
        }
        double x = imageWidth * atomLocations[a][0];
        double y = imageHeight * atomLocations[a][1];
        if(x >= 0 && y >= 0 && x < imageWidth && y < imageHeight)
        {
            y += imageHeight / 2;
            x += imageWidth / 2;
            anyAtomWithinSight = 1;
            double brightness = 1;
            if(randomZeroToOne() > simulationSettings.survivalProbability)
            {
                brightness = sampleTimeOfAtomLossImaging(simulationSettings.survivalProbability);
                if(truth)
                {
                    *truth = brightness;
                }
            }
            if(effectiveLightSourceStdev > 0)
            {
                for(int yi = 0; yi < 2 * imageHeight; yi++)
                {
                    for(int xi = 0; xi < 2 * imageWidth; xi++)
                    {
                        image[yi * imageWidth * 2 + xi] += brightness * gaussianNormalizationFactor * 
                            pow(M_E, -((xi - x) * (xi - x) + (yi - y) * (yi - y)) / (2 * effectiveLightSourceStdev * effectiveLightSourceStdev));
                    }
                }
            }
            else
            {
                image[(int)y * imageWidth * 2 + (int)x] += brightness;
            }
        }
        if(truth)
        {
            truth++;
        }
    }

    if(anyAtomWithinSight)
    {
        simulateOptics(image, imageHeight * 2, imageWidth * 2, simulationSettings.pixelSize / approximationSteps, photonsPerAtom);
    }
}

double fillAtomLocations(const double potentialAtomLocations[][2], unsigned int potentialAtomCount, double (**filledAtomLocations)[2], double *truth)
{
    if (!potentialAtomLocations || !potentialAtomCount)
    {
        return 0;
    }
    else
    {
        int atomCount = 0;
        int bitsPerInt = 8 * sizeof(unsigned int);
        unsigned int *atomSiteMasks = calloc(1 + potentialAtomCount / bitsPerInt, sizeof(unsigned int));
        for (int i = 0; i < potentialAtomCount; i++)
        {
            if (randomZeroToOne() <= simulationSettings.fillingRatio)
            {
                atomSiteMasks[i / bitsPerInt] |= 1 << (i % bitsPerInt);
                atomCount++;
                if(truth)
                {
                    truth[i] = 1;
                }
            }
            else
            {
                if(truth)
                {
                    truth[i] = 0;
                }
            }
        }
        int index = 0;
        *filledAtomLocations = malloc(sizeof(double[atomCount][2]));
        for(int i = 0; i < potentialAtomCount; i++)
        {
            if (atomSiteMasks[i / bitsPerInt] & 1 << (i % bitsPerInt))
            {
                (*filledAtomLocations)[index][0] = potentialAtomLocations[i][0];
                (*filledAtomLocations)[index++][1] = potentialAtomLocations[i][1];
            }
        }
        free(atomSiteMasks);
        return atomCount;
    }
}

void normalizeCameraCoords(double normalizedAtomLocations[][2], double atomLocations[][2], int atomCount, unsigned short cameraCoords)
{
    if(!cameraCoords)
    {
        double fovWidth = simulationSettings.resolutionX * simulationSettings.physicalPixelSize / simulationSettings.magnification;
        double fovHeight = simulationSettings.resolutionY * simulationSettings.physicalPixelSize / simulationSettings.magnification;
        for (int i = 0; i < atomCount; i++)
        {
            normalizedAtomLocations[i][0] = atomLocations[i][0] / fovWidth;
            normalizedAtomLocations[i][1] = atomLocations[i][1] / fovHeight;
        }
    }
    else
    {
        memcpy(normalizedAtomLocations, atomLocations, atomCount * 2 * sizeof(double));
    }
}

void createImageEMCCD(double *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, unsigned int potentialAtomCount, unsigned int approximationSteps)
{
    double gamma = pow(1 + simulationSettings.p0, simulationSettings.numberGainRegisters);

    int imageHeight = approximationSteps * simulationSettings.resolutionY;
    int imageWidth = approximationSteps * simulationSettings.resolutionX;

    double (*atomLocations)[2] = NULL;
    unsigned int atomCount = fillAtomLocations(potentialAtomLocations, potentialAtomCount, &atomLocations, truth);

    double (*normalizedAtomLocations)[2] = malloc(atomCount * 2 * sizeof(double));
    normalizeCameraCoords(normalizedAtomLocations, atomLocations, atomCount, cameraCoords);

    double *image = malloc(imageHeight * imageWidth * sizeof(double) * 4); // Times 4 since the array has to be zero-padded to circumvent wraparound errors from the convolution
    initImageAndSimulateOpticalEffects(image, imageHeight, imageWidth, normalizedAtomLocations, truth, simulationSettings.zernikeCoefficients, atomCount, approximationSteps);

    for (int i = imageHeight / 2; i < imageHeight + imageHeight / 2; i++)
    {
        for(int j = imageWidth / 2; j < imageWidth + imageWidth / 2; j++)
        {
            // Sample light plus spurious charges, only one sampling due to reproductivity of poissonian distribution
            image[i * imageWidth * 2 + j] = samplePoisson(image[i * imageWidth * 2 + j] + ((simulationSettings.strayLightRate + simulationSettings.darkCurrentRate) * simulationSettings.exposureTime + simulationSettings.cicChance) / (approximationSteps * approximationSteps));
        }
    }

    // Binning, emGain and readout
    for (int i = 0; i < simulationSettings.resolutionY / simulationSettings.binning; i++)
    {
        for(int j = 0; j < simulationSettings.resolutionX / simulationSettings.binning; j++)
        {
            // Binning
            int electrons = 0;

            for(int y = 0; y < simulationSettings.binning * approximationSteps; y++)
            {
                for(int x = 0; x < simulationSettings.binning * approximationSteps; x++)
                {
                    electrons += image[(imageHeight / 2 + i * simulationSettings.binning * approximationSteps + y) * imageWidth * 2 + imageWidth / 2 + j * simulationSettings.binning * approximationSteps + x];
                }
            }

            // Sample em gain
            electrons = sampleEMGain(electrons, gamma);

            // Sample sCIC
            double sCICChance = simulationSettings.numberGainRegisters * simulationSettings.sCICChance * pow(simulationSettings.sCICChance, simulationSettings.numberGainRegisters - 1);    // Gets updated each loop iteration to follow binomial distribution
            int sCICCharges = 0;
            while(randomZeroToOne() < sCICChance)
            {
                int remainingStages = randomZeroToOne() * simulationSettings.numberGainRegisters;
                electrons += sampleEMGain(1, pow(simulationSettings.p0, remainingStages));
                sCICCharges++;
                sCICChance = (simulationSettings.numberGainRegisters - sCICCharges) / (sCICCharges + 1) * simulationSettings.sCICChance / (1 - simulationSettings.sCICChance);
            }

            // Sample readout
            electrons = sampleGaussian(electrons / simulationSettings.preampgain + simulationSettings.biasClamp, simulationSettings.readoutStdev);

            binnedImage[i * simulationSettings.resolutionX / simulationSettings.binning + j] = electrons;
        }
    }

    free(image);
    free(atomLocations);
    free(normalizedAtomLocations);
}

void createImageCMOS(double *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, unsigned int potentialAtomCount, unsigned int approximationSteps)
{
    int imageHeight = approximationSteps * simulationSettings.resolutionY;
    int imageWidth = approximationSteps * simulationSettings.resolutionX;

    double (*atomLocations)[2] = NULL;
    unsigned int atomCount = fillAtomLocations(potentialAtomLocations, potentialAtomCount, &atomLocations, truth);

    double (*normalizedAtomLocations)[2] = malloc(atomCount * 2 * sizeof(double));
    normalizeCameraCoords(normalizedAtomLocations, atomLocations, atomCount, cameraCoords);

    double *image = malloc(imageHeight * imageWidth * sizeof(double) * 4); // Times 4 since the array has to be zero-padded to circumvent wraparound errors from the convolution
    initImageAndSimulateOpticalEffects(image, imageHeight, imageWidth, normalizedAtomLocations, truth, simulationSettings.zernikeCoefficients, atomCount, approximationSteps);

    for (int i = 0; i < imageHeight * 2; i++)
    {
        for(int j = 0; j < imageWidth * 2; j++)
        {
            double darkCurrent = sampleGamma(simulationSettings.darkCurrentSamplingAlpha, simulationSettings.darkCurrentSamplingBeta);
            // Sample light plus spurious charges, only one sampling due to reproductivity of poissonian distribution
            image[i * imageWidth * 2 + j] = samplePoisson(image[i * imageWidth * 2 + j] + 
                ((simulationSettings.strayLightRate + darkCurrent) * simulationSettings.exposureTime) / (approximationSteps * approximationSteps));
        }
    }

    double *columnNoises = malloc(simulationSettings.resolutionX * sizeof(double));
    // Set location of gumbel distribution so its mean is zero
    double zeroMeanGumbelLocation = -simulationSettings.columnNoiseScale * EulerMascheroni;
    for(int j = 0; j < simulationSettings.resolutionX; j++)
    {
        columnNoises[j] = sampleGumbel(zeroMeanGumbelLocation, simulationSettings.columnNoiseScale);
    }

    // Readout
    for (int i = 0; i < simulationSettings.resolutionY; i++)
    {
        double rowNoise = sampleGaussian(0, simulationSettings.rowNoiseStdev);
        for(int j = 0; j < simulationSettings.resolutionX; j++)
        {
            // Binning approximation steps
            int electrons = 0;

            for(int y = 0; y < approximationSteps; y++)
            {
                for(int x = 0; x < approximationSteps; x++)
                {
                    electrons += image[(imageHeight / 2 + i * approximationSteps + y) * imageWidth * 2 + imageWidth / 2 + j * approximationSteps + x];
                }
            }

            // Sample readout
            double bias = sampleGaussian(simulationSettings.biasClamp, simulationSettings.biasStdev);
            if(bias < 0)
            {
                bias = 0;
            }
            
            // Flicker, row and column noise
            zeroMeanGumbelLocation = -simulationSettings.flickerNoiseScale * EulerMascheroni;
            electrons += sampleGumbel(zeroMeanGumbelLocation, simulationSettings.flickerNoiseScale);
            electrons += rowNoise + columnNoises[j];

            electrons = sampleGaussian(electrons / simulationSettings.preampgain + bias, simulationSettings.readoutStdev);

            image[(imageHeight / 2 + i * approximationSteps) * imageWidth * 2 + imageWidth / 2 + j * approximationSteps] = electrons;
        }
    }

    // Binning
    for (int i = 0; i < simulationSettings.resolutionY / simulationSettings.binning; i++)
    {
        for(int j = 0; j < simulationSettings.resolutionX / simulationSettings.binning; j++)
        {
            int electrons = 0;

            for(int y = 0; y < simulationSettings.binning; y++)
            {
                for(int x = 0; x < simulationSettings.binning; x++)
                {
                    electrons += image[(imageHeight / 2 + i * simulationSettings.binning * approximationSteps + y * approximationSteps) * imageWidth * 2 + 
                        imageWidth / 2 + j * simulationSettings.binning * approximationSteps + x * approximationSteps];
                }
            }

            binnedImage[i * simulationSettings.resolutionX / simulationSettings.binning + j] = electrons;
        }
    }

    free(image);
    free(atomLocations);
    free(normalizedAtomLocations);
    free(columnNoises);
}
