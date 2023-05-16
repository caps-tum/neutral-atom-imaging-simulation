#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double randomZeroToOne()
{
    static _Bool wasExecuted = 0;
    if (!wasExecuted)
    {
        time_t t;
        srand((unsigned) time(&t));
        wasExecuted = 1;
    }
    return rand() / (double)RAND_MAX;
}

double sampleGaussian(double mean, double stdev)
{
    double boxMullerMethod = sqrt(-2 * log(randomZeroToOne())) * cos(2 * M_PI * randomZeroToOne());
    return boxMullerMethod * stdev + mean;
}

int samplePoisson(double lambda)
{
    double l = pow(M_E, -lambda);
    int k = 0;
    double p = 1;
    while(p > l)
    {
        k++;
        p = p * randomZeroToOne();
    }
    return k - 1;
}

// Marsaglia's transformation-rejection method
double marsagliaGamma(double shape) 
{
    double x,v,u;
    double d = shape - 1./3.; 
    double c = 1./sqrt(9. * d);
    while(1)
    {
        while(v <= 0) 
        {
            x = sampleGaussian(0,1);
            v = 1. + c * x;
        }
        v = v*v*v; 
        u = randomZeroToOne();
        if(u < 1. - 0.0331 * (x*x) * (x*x)) 
        {
            return (d*v);
        }
        if(log(u) < 0.5 * x*x + d * (1. - v + log(v)))
        {
            return (d*v); 
        }
    }
}

double sampleGamma(double shape, double rate)
{
    double val;
    if(shape < 1)
    {
        val = marsagliaGamma(shape + 1) * pow(randomZeroToOne(), 1 / shape);
    }
    else
    {
        val = marsagliaGamma(shape);
    }
    return val / rate;
}

double sampleGumbel(double location, double scale)
{
    return location - scale * log(-log(randomZeroToOne()));
}

/* 
 * Sample the probability distribution that is defined by 
 * P(n|x) = (n^(x-1) * exp(-n/g)) / (g^x * (x-1)!)
 * 
 * n: number of secondary electrons
 * x: number of primary electrons
 * g: em gain
 * CDF(n|x) = 1 - GammaRegularized(x, n/g)
 * Sample by finding positive root of i - GammaRegularized(x, n/g) for a given x
 * i: Random number between 0 and 1
 * 
 * Third-order Schröder iteration
 * f(b) = 1 - GammaRegularized(x, b)
 * f'(b) = exp(-b) * b^(x-1) / Gamma(x)
 * b = n/g
 * 
 * https://doi.org/10.1145/22721.23109
 * https://doi.org/10.1016/S0960-0779(00)00259-9
 */
int sampleEMGain(int primary, double emGain)
{
    if(primary == 0)
    {
        return 0;
    }
    else if(primary == 1)
    {
        return -emGain * log(randomZeroToOne());
    }
    else
    {
        // fOverDerivFac = sampledGain^(1-primary) * (a-1)!
        long double fOverDerivFac = M_E;
        for(double i = 1; i < primary; i++)
        {
            fOverDerivFac *= i / primary * M_E;
        }

        double sampledGain = primary;
        double rand = randomZeroToOne();
        double diff = 1;
        while(diff > 1. / (100 * emGain * emGain))
        {
            double sum = 1; // j = primary - 1 => summand = 1 already added
            double summand = 1;
            for(int j = primary - 1; j > 0; j--)
            {
                summand *= j / sampledGain;
                sum += summand;
            }
            // Factor after fOverDeriv changes sampledGain^(1-primary) to the new sampledGain
            double fOverDeriv = fOverDerivFac * pow(primary / sampledGain, primary - 1) * rand * pow(M_E, sampledGain - primary);
            fOverDeriv -= sum;
            double secondDerivOverDeriv = (primary - 1) / sampledGain - 1;

            double change = fOverDeriv * (1 + fOverDeriv * secondDerivOverDeriv / 2);
            sampledGain -= change;
            diff = change * change;
        }

        return sampledGain * emGain;
    }
}

double sampleTimeOfAtomLossImaging(double survivalProbability)
{
    return log((survivalProbability - 1) * randomZeroToOne() + 1) / log(survivalProbability);
}