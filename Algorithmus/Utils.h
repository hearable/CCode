#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "Hanning.h"

typedef enum {MUTE, VERBOSE} Verbosity;

// ApplyWindow manages the transition from getting a new sample into the system
void ApplyWindow(int windowSize, float* currentWindowRe, float* currentWindowIm, float* previousWindowRe, float* previousWindowIm, float* currentSample, float* window);
void GetSamples(int windowSize, float* currentSample);
void GetSamples2(int windowSize, float* currentSample);

// ProcessData is deprecated, ProcessData_tweaked does the same, only way more efficiently
// ProcessData Fouriertransforms, lowpasses and frequencyshifts the available data
float* ProcessData(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* currentSample, OverlapPart part);
										
// ProcessDataReduced implements only a frequency-upshift without the FFT
float* ProcessDataReduced(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* cosinevector, float* currentSample, OverlapPart part);
										
// ProcessData, only faster
float* ProcessData_tweaked(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* sinTable, float* cosTable, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* currentSample, OverlapPart part);
										
// TestSetup simulates a usual run, measuring the time it takes to process new data
void TestSetup(Verbosity verb, int windowSize, int iterations);
#endif // UTILS_H_INCLUDED
