#include "platformDefines.h"

EXPORT double randomZeroToOne();
EXPORT double sampleGaussian(double mean, double stdev);
EXPORT int samplePoisson(double lambda);
EXPORT int sampleEMGain(int primary, double emGain);
EXPORT double sampleTimeOfAtomLossImaging(double survivalProbability);
EXPORT double sampleGamma(double shape, double rate);
EXPORT double sampleGumbel(double location, double scale);