#include <complex.h>
#include <fftw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "imageModulation.h"

void getConvolutedLightSource(double *psf, int numPixels)
{
    memset(psf, 0, numPixels * numPixels * sizeof(double));

    double middle = (double)(numPixels - 1) / 2.;
    double gaussianNormalizationFactor = 1;
    if(simulationSettings.lightSourceStdev > 0)
    {
        gaussianNormalizationFactor = 1 / (2 * M_PI * simulationSettings.lightSourceStdev * simulationSettings.lightSourceStdev);
        for(int i = 0; i < numPixels; i++)
        {
            for(int j = 0; j < numPixels; j++)
            {
                psf[i * numPixels + j] = gaussianNormalizationFactor * pow(M_E, -((i - middle) * (i - middle) + (j - middle) * (j - middle)) / 
                    (2 * simulationSettings.lightSourceStdev * simulationSettings.lightSourceStdev));
            }
        }
    }
    else
    {
        psf[((int)middle) * numPixels + (int)middle] = 1;
    }
    simulateOptics(psf, numPixels, numPixels, simulationSettings.pixelSize, 1);
}

void getPSF(double *psf, int numPixels)
{
    double pupilRadius = numPixels * simulationSettings.physicalPixelSize / simulationSettings.magnification * simulationSettings.numericalAperture / simulationSettings.wavelength;   // Pupil radius in pixels

    // Allocating buffer memory
    // INEFFICIENT, buffers are only used once to make code readable
    // CHANGE FOR FINAL VERSION!
    fftw_complex *pupil = fftw_alloc_complex(numPixels * numPixels);
    fftw_complex *psfC = fftw_alloc_complex(numPixels * numPixels);
    
    // Construct complex pupil and apply fft to get psf
    fftw_plan p = fftw_plan_dft_2d(numPixels, numPixels, pupil, psfC, FFTW_FORWARD, FFTW_ESTIMATE);
    for (int i = 0; i < numPixels; i++)
    {
        for(int j = 0; j < numPixels; j++)
        {
            double y = i - (numPixels - 1) / 2;
            double x = j - (numPixels - 1) / 2;
            double r = sqrt(x * x + y * y);
            if(r < pupilRadius)
            {
                double theta = atan2(y, x);
                double phase = 2 * M_PI / simulationSettings.wavelength * ZernikePhase(r / pupilRadius, theta, simulationSettings.zernikeCoefficients);
                pupil[i * numPixels + j] = cos(phase) + sin(phase) * I;
            }
            else
            {
                pupil[i * numPixels + j] = 0;
            }
        }
    }
    fftw_execute(p);
    fftw_destroy_plan(p);

    double sum = 0;
    // Finalize psf and apply ifftshift
    for (int i = 0; i < numPixels; i++)
    {
        for(int j = 0; j < numPixels; j++)
        {
            double abs = cabs(psfC[i * numPixels + j]);
            psf[((i + numPixels / 2) % numPixels) * numPixels + ((j + numPixels / 2) % numPixels)] = abs * abs;
            sum += abs * abs;
        }
    }
    for (int i = 0; i < numPixels; i++)
    {
        for(int j = 0; j < numPixels; j++)
        {
            psf[i * numPixels + j] /= sum;
        }
    }

    fftw_free(pupil);
    fftw_free(psfC);
}