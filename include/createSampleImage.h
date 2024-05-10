#include "platformDefines.h"

EXPORT void createImageEMCCD(int *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, int atomCount, int approximationSteps);
EXPORT void createImageCMOS(int *binnedImage, const double potentialAtomLocations[][2], unsigned short cameraCoords, double *truth, int atomCount, int approximationSteps);