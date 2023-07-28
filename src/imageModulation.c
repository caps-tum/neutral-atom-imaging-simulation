#include <complex.h>
#include <fftw3.h>
#include <math.h>
#include "settings.h"

double ZernikePhase(double r, double u, const double zernikeCoefficients[15])
{
    double Z = 0;
    double rSq = r * r;
    Z += zernikeCoefficients[0];
    Z += zernikeCoefficients[1] * 2 * r * cos(u);
    Z += zernikeCoefficients[2] * 2 * r * sin(u);
    Z += zernikeCoefficients[3] * sqrt(3) * (2 * rSq - 1);
    Z += zernikeCoefficients[4] * sqrt(6) * rSq * sin(2*u);
    Z += zernikeCoefficients[5] * sqrt(6) * rSq * cos(2*u);
    Z += zernikeCoefficients[6] * sqrt(8) * (3 * rSq - 2) * r * sin(u);
    Z += zernikeCoefficients[7] * sqrt(8) * (3 * rSq - 2) * r * cos(u);
    Z += zernikeCoefficients[8] * sqrt(8) * rSq * r * sin(3*u);
    Z += zernikeCoefficients[9] * sqrt(8) * rSq * r * cos(3*u);
    Z += zernikeCoefficients[10] * sqrt(5) * (1 - 6 * rSq + 6 * rSq * rSq);
    Z += zernikeCoefficients[11] * sqrt(10) * (4 * rSq - 3) * rSq * cos(2*u);
    Z += zernikeCoefficients[12] * sqrt(10) * (4 * rSq - 3) * rSq * sin(2*u);
    Z += zernikeCoefficients[13] * sqrt(10) * rSq * rSq * cos(4*u);
    Z += zernikeCoefficients[14] * sqrt(10) * rSq * rSq * sin(4*u);
    return Z;
}

void simulateOptics(double *inputImage, int imageHeight, int imageWidth, double effectivePixelSize, double photonsPerAtom)
{
    double xFac = 1;
    double yFac = 1;
    int smallerDimension;
    if(imageHeight < imageWidth)
    {
        smallerDimension = imageHeight;
        xFac = (double)imageHeight / imageWidth;
    }
    else
    {
        smallerDimension = imageWidth;
        yFac = (double)imageWidth / imageHeight;
    }

    double pupilRadius = smallerDimension * effectivePixelSize * simulationSettings.numericalAperture / simulationSettings.wavelength;   // Pupil radius in pixels

    // Allocating buffer memory
    // INEFFICIENT, buffers are only used once to make code readable
    // CHANGE FOR FINAL VERSION!
    fftw_complex *pupil = fftw_alloc_complex(imageHeight * imageWidth);
    fftw_complex *psf = fftw_alloc_complex(imageHeight * imageWidth);
    fftw_complex *otf = fftw_alloc_complex(imageHeight * imageWidth);
    double *mtf = fftw_alloc_real(imageHeight * imageWidth);
    fftw_complex *image = fftw_alloc_complex(imageHeight * imageWidth);
    fftw_complex *imageFT = fftw_alloc_complex(imageHeight * imageWidth);

    // Construct complex pupil and apply fft to get psf
    fftw_plan p = fftw_plan_dft_2d(imageHeight, imageWidth, pupil, psf, FFTW_FORWARD, FFTW_ESTIMATE);
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            double y = (i - (imageHeight - 1) / 2) * yFac;
            double x = (j - (imageHeight - 1) / 2) * xFac;
            double r = sqrt(x * x + y * y);
            if(r < pupilRadius)
            {
                double theta = atan2(y, x);
                double phase = 2 * M_PI / simulationSettings.wavelength * ZernikePhase(r / pupilRadius, theta, simulationSettings.zernikeCoefficients);
                pupil[i * imageWidth + j] = cos(phase) + sin(phase) * I;
            }
            else
            {
                pupil[i * imageWidth + j] = 0;
            }
        }
    }
    fftw_execute(p);
    fftw_destroy_plan(p);

    // Finalize psf and apply fft to get otf
    p = fftw_plan_dft_2d(imageHeight, imageWidth, psf, otf, FFTW_FORWARD, FFTW_ESTIMATE);
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            double abs = cabs(psf[i * imageWidth + j]);
            psf[i * imageWidth + j] = abs * abs;
        }
    }
    fftw_execute(p);
    fftw_destroy_plan(p);

    // Construct mtf and apply ifftshift
    double max_val = cabs(otf[0]);
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            mtf[i * imageWidth + j] = cabs(otf[i * imageWidth + j] / max_val);
        }
    }

    // Construct test input and apply fft
    // In this case single illuminated pixels at approximate atom location
    double sumInitial = 0;
    p = fftw_plan_dft_2d(imageHeight, imageWidth, image, imageFT, FFTW_FORWARD, FFTW_ESTIMATE);
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            image[i * imageWidth + j] = inputImage[i * imageWidth + j];
            sumInitial += inputImage[i * imageWidth + j];
        }
    }
    fftw_execute(p);
    fftw_destroy_plan(p);
    
    // Multiply fft of image with mtf and apply ifft to get final image
    p = fftw_plan_dft_2d(imageHeight, imageWidth, imageFT, image, FFTW_BACKWARD, FFTW_ESTIMATE);
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            imageFT[i * imageWidth + j] = mtf[i * imageWidth + j] * imageFT[i * imageWidth + j];
        }
    }
    fftw_execute(p);
    fftw_destroy_plan(p);

    double sumEnd = 0;
    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            inputImage[i * imageWidth + j] = cabs(image[i * imageWidth + j]) / (imageHeight * imageWidth);
            sumEnd += inputImage[i * imageWidth + j];
        }
    }

    for (int i = 0; i < imageHeight; i++)
    {
        for(int j = 0; j < imageWidth; j++)
        {
            inputImage[i * imageWidth + j] = inputImage[i * imageWidth + j] / sumEnd * sumInitial * photonsPerAtom;
        }
    }

    fftw_free(pupil);
    fftw_free(psf);
    fftw_free(otf);
    fftw_free(mtf);
    fftw_free(image);
    fftw_free(imageFT);
}