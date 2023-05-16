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
    int binning;
    int resolutionX;
    int resolutionY;
    double zernikeCoefficients[15];
} settings;

void readConfig(const char *path);
void setStrayLightRate(double val);
void setDarkCurrentRate(double val);
void setDarkCurrentSamplingAlpha(double val);
void setDarkCurrentSamplingBeta(double val);
void setCicChance(double val);
void setQuantumEfficiency(double val);
void setWavelength(double val);
void setNumericalAperture(double val);
void setPhysicalPixelSize(double val);
void setMagnification(double val);
void setBiasClamp(double val);
void setBiasStdev(double val);
void setRowNoiseStdev(double val);
void setColumnNoiseScale(double val);
void setFlickerNoiseScale(double val);
void setPreampgain(double val);
void setSCICChance(double val);
void setReadoutStdev(double val);
void setNumberGainRegisters(double val);
void setP0(double val);
void setScatteringRate(double val);
void setExposureTime(double val);
void setSurvivalProbability(double val);
void setFillingRatio(double val);
void setBinning(int val);
void setResolution(int x, int y);
void setZernikeCoefficients(const double val[15]);

extern settings simulationSettings;