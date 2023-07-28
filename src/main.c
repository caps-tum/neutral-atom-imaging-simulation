#include <stdio.h>
#include <stdlib.h>
#include "createSampleImage.h"
#include "distributionSampling.h"
#include "settings.h"

#define resolutionX 512
#define resolutionY 256

const double potentialAtomLocations[][2] = {{0.15625,0.26953125}, {0.3203125,0.26953125}, {0.48828125,0.26953125}, {0.66015625,0.26953125}, {0.828125,0.26953125},
    {0.15625,0.4375}, {0.3203125,0.4375}, {0.48828125,0.4375}, {0.66015625,0.4375}, {0.828125,0.4375},
    {0.15625,0.609375}, {0.3203125,0.609375}, {0.48828125,0.609375}, {0.66015625,0.609375}, {0.828125,0.609375},
    {0.15625,0.77734375}, {0.3203125,0.77734375}, {0.48828125,0.77734375}, {0.66015625,0.77734375}, {0.828125,0.77734375}};
const double atomLocations[][2] = {{0.15625,0.26953125}, {0.3203125,0.26953125}, {0.48828125,0.26953125}, {0.66015625,0.26953125}, {0.828125,0.26953125},
    {0.15625,0.4375}, {0.3203125,0.4375}, {0.48828125,0.4375}, {0.66015625,0.4375}, //{0.828125,0.4375},
    {0.15625,0.609375}, {0.3203125,0.609375}, {0.48828125,0.609375}, {0.66015625,0.609375}, {0.828125,0.609375},
    {0.15625,0.77734375}, {0.3203125,0.77734375}, {0.48828125,0.77734375}, {0.66015625,0.77734375}, {0.828125,0.77734375}};
// Zernike coefficients by Noll index
const double zernikeCoefficients[15] = {0,0,0,0.07232454,8.7644e-04,-0.01069755,0.00280808,0.00723265,0.00436401,0.00117688,0.02449155,-0.00427388,-0.00250116,-0.00477205,-5.4310e-04};

int main()
{
    FILE *output = fopen("modulated", "w");

    double *binnedImage = malloc(resolutionX * resolutionY * sizeof(double));

    readConfig("simulationSettings.cfg");
    setZernikeCoefficients(zernikeCoefficients);
    setResolution(resolutionX * simulationSettings.binning, resolutionY * simulationSettings.binning);
    setScatteringRate(29000);
    setFillingRatio(0.6);
    
    // Create simulated image
    createImageEMCCD(binnedImage, atomLocations, 1, NULL, 19, 1);


    printf("Image: \n");
    for(int i=0;i<resolutionY;i++){
        for(int j=0;j<resolutionX;j++){
            printf("%f ", binnedImage[i * resolutionX + j]);
        }
        printf("\n");
    }

    free(binnedImage);
}