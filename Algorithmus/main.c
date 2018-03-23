#include <stdio.h>
#include <stdlib.h>
#include "Hanning.c"
#include "FFT.c"
#include "math.h"
#include "Lowpass.c"
#include "CyclicShift.c"


// Sets the previous data and applies the window function to the current sequence.
void ApplyWindow(int windowSize, double* currentWindowRe, double* currentWindowIm, double* previousWindowRe, double* previousWindowIm, double* currentSample, double* window){
    for(int i=0;i<windowSize;i++){
        previousWindowRe[i] = currentWindowRe[i];
        previousWindowIm[i] = currentWindowIm[i];
        currentWindowRe[i] = currentSample[i] * window[i];
        currentWindowIm[i] = 0; // All sampled data is real
    }
}
// Helper function to get a window full of ones
void GetSamples(int windowSize, double* currentSample){
    for(int i=0; i<windowSize; i++){
        if(i%2==0){
            if(i%4==0){
                currentSample[i] = 1;
            }
            else{
                currentSample[i] = 1;
            }
        }
        else {
            currentSample[i] = 1;
        }
    }
}

// Helper function to get window full of twos
void GetSamples2(int windowSize, double* currentSample){
    for(int i=0; i<windowSize; i++){
        if(i%2==0){
            if(i%4==0){
                currentSample[i] = 2;
            }
            else{
                currentSample[i] = 2;
            }
        }
        else {
            currentSample[i] = 2;
        }
    }
}
// GetSamples -> ApplyWindow -> (Any Algorithm you like) -> OverlapAdd -> ReturnWindowOutput

int main(){
    double* currentResult;
    double* currentOutput;

    double* hanningWindow;

    double* previousWindowRe;
    double* previousWindowIm;

    double* currentWindowRe;
    double* currentWindowIm;

    double* currentSample;

    // Use powers of 2 only
    int windowSize = 512;

    double sampleFrequency = 64000.0d;
    double cutOffFrequency = 16500.0d;
    double shiftFrequency = 0000.0d;
    int lowpassRolloff = 6;

    double* lowpass = LowpassFilter(windowSize,sampleFrequency,cutOffFrequency,lowpassRolloff);

    double overlappercentage = 0.5d;
    int overlapIndex = (int)((windowSize+1)*(1.0d-overlappercentage)); // Upward adjusted index for overlapping windows

    // Allocation of space for data
    previousWindowRe = (double*) calloc(windowSize, sizeof(double));
    previousWindowIm = (double*) calloc(windowSize, sizeof(double));
    currentWindowRe = (double*) calloc(windowSize, sizeof(double));
    currentWindowIm = (double*) calloc(windowSize, sizeof(double));
    currentSample = (double*) calloc(windowSize, sizeof(double));
    currentOutput = (double*) calloc(2*windowSize, sizeof(double));

    hanningWindow = HanningWindow(windowSize, PERIODIC);

    int Iterations = 4;

    for(int i=0; i<Iterations; i++){
        GetSamples(windowSize,currentSample);
        ApplyWindow(windowSize,currentWindowRe,currentWindowIm,previousWindowRe,previousWindowIm,currentSample,hanningWindow);

        for(int i=0; i<windowSize;i++){
            //printf("%f \n", currentWindowRe[i]);
        }

        Fft_transform(currentWindowRe, currentWindowIm, windowSize);

        for(int i=0;i<windowSize;i++){
            currentWindowIm[i] *= lowpass[i];
            currentWindowRe[i] *= lowpass[i];
        }

        CyclicCosineForwardShift(currentWindowRe, currentWindowIm, windowSize, sampleFrequency, shiftFrequency);

        Fft_inverseTransform(currentWindowRe, currentWindowIm, windowSize);

        currentResult = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);

        if(i == 0){
            currentOutput = ReturnWindowOutputHanning(windowSize,overlappercentage,currentResult,HEAD);
            for(int j=0;j<overlapIndex;j++){
                printf("%f \n",currentOutput[j]);
            }
            free(currentOutput);
        } else if(i == (Iterations-1)){
            currentOutput = ReturnWindowOutputHanning(windowSize,overlappercentage,currentResult,MID);
            for(int j=0;j<(windowSize-overlapIndex);j++){
               printf("%f \n",currentOutput[j]);
            }
            currentOutput = ReturnWindowOutputHanning(windowSize,overlappercentage,currentResult,TAIL);
            for(int j=0;j<overlapIndex;j++){
               printf("%f \n",currentOutput[j]);
            }
            free(currentOutput);
        } else {
            currentOutput = ReturnWindowOutputHanning(windowSize,overlappercentage,currentResult,MID);
            for(int j=0;j<(windowSize-overlapIndex);j++){
               printf("%f \n",currentOutput[j]);
            }
            free(currentOutput);
        }
    }

    return 0;
}
