#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "Hanning.h"

void ApplyWindow(int windowSize, float* currentWindowRe, float* currentWindowIm, float* previousWindowRe, float* previousWindowIm, float* currentSample, float* window);
void GetSamples(int windowSize, float* currentSample);
void GetSamples2(int windowSize, float* currentSample);
float* ProcessData(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* currentSample, OverlapPart part);
void TestSetup(void);
#endif // UTILS_H_INCLUDED
