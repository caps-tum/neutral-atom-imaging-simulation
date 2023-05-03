#include "settings.h"
#include "platformDefines.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

settings simulationSettings;
__attribute__((constructor)) EXPORT void initSettings()
{
    simulationSettings.strayLightRate = 0.4;
    simulationSettings.darkCurrentRate = 0.00029;
    simulationSettings.darkCurrentSamplingAlpha = 0.006;
    simulationSettings.darkCurrentSamplingBeta = 1;
    simulationSettings.cicChance = 0.00037;
    simulationSettings.quantumEfficiency = 0.86;
    simulationSettings.wavelength = 0.4619;
    simulationSettings.numericalAperture = 0.65;
    simulationSettings.physicalPixelSize = 16;
    simulationSettings.magnification = 156.25;
    simulationSettings.pixelSize = 0.1024;
    simulationSettings.biasClamp = 500;
    simulationSettings.biasStdev = 1;
    simulationSettings.rowNoiseStdev = 0.5;
    simulationSettings.columnNoiseScale = 0.5;
    simulationSettings.flickerNoiseScale = 0.2;
    simulationSettings.preampgain = 4.11;
    simulationSettings.sCICChance = 0.00002;
    simulationSettings.readoutStdev = 4;
    simulationSettings.numberGainRegisters = 536;
    simulationSettings.p0 = 0.0106982061; // em gain 300
    simulationSettings.scatteringRate = 30000;
    simulationSettings.exposureTime = 0.1;
    simulationSettings.survivalProbability = 1;
    simulationSettings.fillingRatio = 1;
    simulationSettings.binning = 1;
    simulationSettings.resolutionX = 512;
    simulationSettings.resolutionY = 512;
}

EXPORT void readConfig(const char *path)
{
    FILE *file = fopen(path, "r");
    if(!file)
    {
        return;
    }
    char line[1024];
    while(fgets(line, 1023, file))
    {
        if (!strstr(line, "=")) {
            // No assignment, so the line is skipped
            continue;
        }
        char *name = strtok(line, " =");
        char *value = strtok(NULL, " =");
        if(!name || !value)
        {
            continue;
        }

        if(!strcmp(name, "strayLightRate"))
        {
            double valueC = atof(value);
            simulationSettings.strayLightRate = valueC;
        }
        else if(!strcmp(name, "darkCurrentRate"))
        {
            double valueC = atof(value);
            simulationSettings.darkCurrentRate = valueC;
        }
        else if(!strcmp(name, "darkCurrentSamplingAlpha"))
        {
            double valueC = atof(value);
            simulationSettings.darkCurrentSamplingAlpha = valueC;
        }
        else if(!strcmp(name, "darkCurrentSamplingBeta"))
        {
            double valueC = atof(value);
            simulationSettings.darkCurrentSamplingBeta = valueC;
        }
        else if(!strcmp(name, "cicChance"))
        {
            double valueC = atof(value);
            simulationSettings.cicChance = valueC;
        }
        else if(!strcmp(name, "quantumEfficiency"))
        {
            double valueC = atof(value);
            simulationSettings.quantumEfficiency = valueC;
        }
        else if(!strcmp(name, "wavelength"))
        {
            double valueC = atof(value);
            simulationSettings.wavelength = valueC;
        }
        else if(!strcmp(name, "numericalAperture"))
        {
            double valueC = atof(value);
            simulationSettings.numericalAperture = valueC;
        }
        else if(!strcmp(name, "physicalPixelSize"))
        {
            double valueC = atof(value);
            simulationSettings.physicalPixelSize = valueC;
            simulationSettings.pixelSize = valueC / simulationSettings.magnification;
        }
        else if(!strcmp(name, "magnification"))
        {
            double valueC = atof(value);
            simulationSettings.magnification = valueC;
            simulationSettings.pixelSize = simulationSettings.physicalPixelSize / valueC;
        }
        else if(!strcmp(name, "biasClamp"))
        {
            double valueC = atof(value);
            simulationSettings.biasClamp = valueC;
        }
        else if(!strcmp(name, "biasStdev"))
        {
            double valueC = atof(value);
            simulationSettings.biasStdev = valueC;
        }
        else if(!strcmp(name, "rowNoiseStdev"))
        {
            double valueC = atof(value);
            simulationSettings.rowNoiseStdev = valueC;
        }
        else if(!strcmp(name, "columnNoiseScale"))
        {
            double valueC = atof(value);
            simulationSettings.columnNoiseScale = valueC;
        }
        else if(!strcmp(name, "flickerNoiseScale"))
        {
            double valueC = atof(value);
            simulationSettings.flickerNoiseScale = valueC;
        }
        else if(!strcmp(name, "preampgain"))
        {
            double valueC = atof(value);
            simulationSettings.preampgain = valueC;
        }
        else if(!strcmp(name, "sCICChance"))
        {
            double valueC = atof(value);
            simulationSettings.sCICChance = valueC;
        }
        else if(!strcmp(name, "readoutStdev"))
        {
            double valueC = atof(value);
            simulationSettings.readoutStdev = valueC;
        }
        else if(!strcmp(name, "numberGainRegisters"))
        {
            double valueC = atof(value);
            simulationSettings.numberGainRegisters = valueC;
        }
        else if(!strcmp(name, "p0"))
        {
            double valueC = atof(value);
            simulationSettings.p0 = valueC;
        }
        else if(!strcmp(name, "scatteringRate"))
        {
            double valueC = atof(value);
            simulationSettings.scatteringRate = valueC;
        }
        else if(!strcmp(name, "exposureTime"))
        {
            double valueC = atof(value);
            simulationSettings.exposureTime = valueC;
        }
        else if(!strcmp(name, "survivalProbability"))
        {
            double valueC = atof(value);
            simulationSettings.survivalProbability = valueC;
        }
        else if(!strcmp(name, "fillingRatio"))
        {
            double valueC = atof(value);
            simulationSettings.fillingRatio = valueC;
        }
        else if(!strcmp(name, "binning"))
        {
            int valueC = atoi(value);
            simulationSettings.binning = valueC;
        }
        else if(!strcmp(name, "resolution"))
        {
            int valueX = atoi(strtok(value, ", "));
            int valueY = atoi(strtok(NULL, ", "));
            simulationSettings.resolutionX = valueX;
            simulationSettings.resolutionY = valueY;
        }
        else if(!strcmp(name, "zernikeCoefficients"))
        {
            char *valueZ = strtok(value, ", ");
            for(int index = 0; index < 15 && valueZ; index++)
            {
                double zernikeValue = atof(valueZ);
                simulationSettings.zernikeCoefficients[index] = zernikeValue;
                valueZ = strtok(NULL, ", ");
            }
        }
    }
    fclose(file);
}

EXPORT void setStrayLightRate(double val)
{
    simulationSettings.strayLightRate = val;
}

EXPORT void setDarkCurrentRate(double val)
{
    simulationSettings.darkCurrentRate = val;
}

EXPORT void setDarkCurrentSamplingAlpha(double val)
{
    simulationSettings.darkCurrentSamplingAlpha = val;
}

EXPORT void setDarkCurrentSamplingBeta(double val)
{
    simulationSettings.darkCurrentSamplingBeta = val;
}

EXPORT void setCicChance(double val)
{
    simulationSettings.cicChance = val;
}

EXPORT void setQuantumEfficiency(double val)
{
    simulationSettings.quantumEfficiency = val;
}

EXPORT void setWavelength(double val)
{
    simulationSettings.wavelength = val;
}

EXPORT void setNumericalAperture(double val)
{
    simulationSettings.numericalAperture = val;
}

EXPORT void setPhysicalPixelSize(double val)
{
    simulationSettings.physicalPixelSize = val;
    simulationSettings.pixelSize = val / simulationSettings.magnification;
}

EXPORT void setMagnification(double val)
{
    simulationSettings.magnification = val;
    simulationSettings.pixelSize = simulationSettings.physicalPixelSize / val;
}

EXPORT void setBiasClamp(double val)
{
    simulationSettings.biasClamp = val;
}

EXPORT void setBiasStdev(double val)
{
    simulationSettings.biasStdev = val;
}

EXPORT void setRowNoiseStdev(double val)
{
    simulationSettings.rowNoiseStdev = val;
}

EXPORT void setColumnNoiseScale(double val)
{
    simulationSettings.columnNoiseScale = val;
}

EXPORT void setFlickerNoiseScale(double val)
{
    simulationSettings.flickerNoiseScale = val;
}

EXPORT void setPreampgain(double val)
{
    simulationSettings.preampgain = val;
}

EXPORT void setSCICChance(double val)
{
    simulationSettings.sCICChance = val;
}

EXPORT void setReadoutStdev(double val)
{
    simulationSettings.readoutStdev = val;
}

EXPORT void setNumberGainRegisters(double val)
{
    simulationSettings.numberGainRegisters = val;
}

EXPORT void setP0(double val)
{
    simulationSettings.p0 = val;
}

EXPORT void setScatteringRate(double val)
{
    simulationSettings.scatteringRate = val;
}

EXPORT void setExposureTime(double val)
{
    simulationSettings.exposureTime = val;
}

EXPORT void setSurvivalProbability(double val)
{
    simulationSettings.survivalProbability = val;
}

EXPORT void setFillingRatio(double val)
{
    simulationSettings.fillingRatio = val;
}

EXPORT void setBinning(int val)
{
    simulationSettings.binning = val;
}

EXPORT void setResolution(int x, int y)
{
    simulationSettings.resolutionX = x;
    simulationSettings.resolutionY = y;
}

EXPORT void setZernikeCoefficients(const double val[15])
{
    memcpy(simulationSettings.zernikeCoefficients, val, 15 * sizeof(double));
}