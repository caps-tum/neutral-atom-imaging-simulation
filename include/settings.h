#include "platformDefines.h"

typedef struct Settings
{
    double strayLightRate;      // registered photons per second
    double darkCurrentRate;     // registered photons per second
    double darkCurrentSamplingAlpha;
    double darkCurrentSamplingBeta;
    double cicChance;           // registered photons
    double quantumEfficiency;
    double wavelength;          // wavelenght of the light in um
    double numericalAperture;   // Numerical aperture of the objective
    double physicalPixelSize;   // Physical pixel size in um
    double magnification;       // Magnification of the optical system
    double pixelSize;
    double biasClamp;
    double biasStdev;
    double rowNoiseStdev;
    double columnNoiseScale;
    double flickerNoiseScale;
    double preampgain;
    double sCICChance;
    double readoutStdev;
    double numberGainRegisters;
    double p0;
    double scatteringRate;
    double exposureTime;
    double survivalProbability;
    double fillingRatio;
    double lightSourceStdev;
    int binning;
    int resolutionX;
    int resolutionY;
    double zernikeCoefficients[15];
} settings;

EXPORT void readConfig(const char *path);
EXPORT void setStrayLightRate(double val);
EXPORT void setDarkCurrentRate(double val);
EXPORT void setDarkCurrentSamplingAlpha(double val);
EXPORT void setDarkCurrentSamplingBeta(double val);
EXPORT void setCicChance(double val);
EXPORT void setQuantumEfficiency(double val);
EXPORT void setWavelength(double val);
EXPORT void setNumericalAperture(double val);
EXPORT void setPhysicalPixelSize(double val);
EXPORT void setMagnification(double val);
EXPORT void setBiasClamp(double val);
EXPORT void setBiasStdev(double val);
EXPORT void setRowNoiseStdev(double val);
EXPORT void setColumnNoiseScale(double val);
EXPORT void setFlickerNoiseScale(double val);
EXPORT void setPreampgain(double val);
EXPORT void setSCICChance(double val);
EXPORT void setReadoutStdev(double val);
EXPORT void setNumberGainRegisters(double val);
EXPORT void setP0(double val);
EXPORT void setScatteringRate(double val);
EXPORT void setExposureTime(double val);
EXPORT void setSurvivalProbability(double val);
EXPORT void setFillingRatio(double val);
EXPORT void setLightSourceStdev(double val);
EXPORT void setBinning(int val);
EXPORT void setResolution(int x, int y);
EXPORT void setZernikeCoefficients(const double val[15]);

extern settings simulationSettings;