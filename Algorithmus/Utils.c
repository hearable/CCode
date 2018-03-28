// General utilities to separate functionality from main.c

#include <stdio.h>
#include <stdlib.h>
#include "Hanning.h"
#include "FFT.h"
#include "math.h"
#include "Lowpass.h"
#include "CyclicShift.h"
#include "am_util.h"

#define XT_PERIOD               32768

void
stimer_init(void)
{

    //
    // Configure the STIMER and run
    //
    am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
    am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);

}

// Sets the previous data and applies the window function to the current sequence.
void ApplyWindow(int windowSize, float* currentWindowRe, float* currentWindowIm, float* previousWindowRe, float* previousWindowIm, float* currentSample, float* window){
    for(int i=0;i<windowSize;i++){
        previousWindowRe[i] = currentWindowRe[i];
        previousWindowIm[i] = currentWindowIm[i];
        currentWindowRe[i] = currentSample[i] * window[i];
        currentWindowIm[i] = 0; // All sampled data is real
    }
}

// Helper function to get a window full of ones
void GetSamples(int windowSize, float* currentSample){
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
void GetSamples2(int windowSize, float* currentSample){
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

// Does all the processing with one function call:
// ApplyWindow -> FFT -> Lowpass -> CosineShift -> IFFT -> OverlapAdd -> ReturnWindowOutput
float* ProcessData(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* currentSample, OverlapPart part){

    int overlapIndex = (int)((windowSize+1)*(1.0f-overlappercentage));
    ApplyWindow(windowSize,currentWindowRe,currentWindowIm,previousWindowRe,previousWindowIm,currentSample,window);
    Fft_transform(currentWindowRe, currentWindowIm, windowSize);

    for(int i=0;i<windowSize;i++){
            currentWindowIm[i] *= lowpassFilter[i];
            currentWindowRe[i] *= lowpassFilter[i];
    }

    CyclicCosineForwardShift(currentWindowRe, currentWindowIm, windowSize, sampleFrequency, shiftFrequency);
    Fft_inverseTransform(currentWindowRe, currentWindowIm, windowSize);
    float* currentResult = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);
    return ReturnWindowOutputHanning(windowSize, overlappercentage,currentResult, part);
}
										
float* ProcessDataReduced(int windowSize, float overlappercentage, float sampleFrequency, float shiftFrequency,
                    float* lowpassFilter, float* window, float* previousWindowRe, float* previousWindowIm,
                    float* currentWindowRe, float* currentWindowIm, float* cosinevector, float* currentSample, OverlapPart part){

    int overlapIndex = (int)((windowSize+1)*(1.0f-overlappercentage));
    ApplyWindow(windowSize,currentWindowRe,currentWindowIm,previousWindowRe,previousWindowIm,currentSample,window);
    CyclicCosineForwardShiftInTime(currentWindowRe,windowSize,sampleFrequency,shiftFrequency);
    float* currentResult = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);
    return ReturnWindowOutputHanning(windowSize, overlappercentage,currentResult, part);
}

// Test functionality with some pre-specified data
// Needs about 20*windowSize*sizeof(float)
void TestSetup(void){

    // Use powers of 2 only
    int windowSize = 64; // Sample data as discussed with Paula

    // Declaration + Initialization of used data arrays
    float* currentOutput;
    currentOutput = (float*) calloc(2*windowSize, sizeof(float));

    float* hanningWindow;
    hanningWindow = HanningWindow(windowSize, PERIODIC);

    float* previousWindowRe;
    previousWindowRe = (float*) calloc(windowSize, sizeof(float));
    float* previousWindowIm;
    previousWindowIm = (float*) calloc(windowSize, sizeof(float));

    float* currentWindowRe;
    currentWindowRe = (float*) calloc(windowSize, sizeof(float));
    float* currentWindowIm;
    currentWindowIm = (float*) calloc(windowSize, sizeof(float));

    float* currentSample;
    currentSample = (float*) calloc(windowSize, sizeof(float));

    float sampleFrequency = 32000.0f;
    float cutOffFrequency = 20000.0f; // Sensible audible frequency cutoff
    float shiftFrequency = 2000.0f;
    int lowpassRolloff = 6;

    float* lowpass = LowpassFilter(windowSize,sampleFrequency,cutOffFrequency,lowpassRolloff);

    float overlappercentage = 0.5f;
    int overlapIndex = (int)((windowSize+1)*(1.0f-overlappercentage)); // Upward adjusted index for overlapping windows

    int Iterations = 1;
		float* cosinevector = (float*) calloc(windowSize, sizeof(float));
		
		InitializeCosineVector(cosinevector, windowSize, sampleFrequency, shiftFrequency);

		uint32_t time = 0;
		stimer_init();

    for(int i=0; i<Iterations; i++){
        GetSamples(windowSize,currentSample);

        if(i == 0){
            currentOutput = ProcessDataReduced(windowSize, overlappercentage, sampleFrequency, shiftFrequency, lowpass, hanningWindow, previousWindowRe, previousWindowIm, currentWindowRe, currentWindowIm, cosinevector, currentSample, HEAD);
            //for(int j=0;j<overlapIndex;j++){
                //am_util_stdio_printf("%f \n",currentOutput[j]);
            //}
            free(currentOutput);
        } else if(i == (Iterations-1)){
            currentOutput = ProcessDataReduced(windowSize, overlappercentage, sampleFrequency, shiftFrequency, lowpass, hanningWindow, previousWindowRe, previousWindowIm, currentWindowRe, currentWindowIm, cosinevector, currentSample, MID);
           //for(int j=0;j<(windowSize-overlapIndex);j++){
               //am_util_stdio_printf("%f \n",currentOutput[j]);
            //}
            currentOutput = ProcessDataReduced(windowSize, overlappercentage, sampleFrequency, shiftFrequency, lowpass, hanningWindow, previousWindowRe, previousWindowIm, currentWindowRe, currentWindowIm, cosinevector, currentSample, TAIL);
            //for(int j=0;j<overlapIndex;j++){
               //am_util_stdio_printf("%f \n",currentOutput[j]);
            //}
            free(currentOutput);
        } else {
            currentOutput = ProcessDataReduced(windowSize, overlappercentage, sampleFrequency, shiftFrequency, lowpass, hanningWindow, previousWindowRe, previousWindowIm, currentWindowRe, currentWindowIm, cosinevector, currentSample, MID);
            //for(int j=0;j<(windowSize-overlapIndex);j++){
              //am_util_stdio_printf("%f \n",currentOutput[j]);
            //}
            free(currentOutput);
        }
    }
		
		time = am_hal_stimer_counter_get();
		am_util_stdio_printf("%u \n",time);
		
    return;
}
