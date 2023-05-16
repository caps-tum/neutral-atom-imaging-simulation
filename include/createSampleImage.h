#include "platformDefines.h"

EXPORT void createImageEMCCD(double *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, int atomCount, int approximationSteps);
EXPORT void createImageCMOS(double *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, int atomCount, int approximationSteps);