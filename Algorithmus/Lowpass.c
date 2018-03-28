#include "Lowpass.h"
#include "Hanning.h"
#include <stdlib.h>

// implements Hanning-Lowpass, n bin rolloff
// Implemented in frequency domain
float* LowpassFilter(int windowSize, float sampleFrequency, float cutOffFrequency, int rolloff){
    float* resultValue = (float*) calloc(windowSize, sizeof(float));

    int finalbin = (windowSize) * ((cutOffFrequency/sampleFrequency)+(1.0f/(windowSize+1)));


    // Pick a hanning window which is twice as wide, and only take half of it
    float* hanningWindow = HanningWindow(2*rolloff, SYMMETRIC);

    for(int i=0;i<finalbin-((rolloff+1)/2);i++){
        resultValue[i] = 1;
    }

    for(int i=0;i<rolloff;i++){
        resultValue[i+finalbin-((rolloff+1)/2)]= hanningWindow[i+rolloff];
    }

    for(int i=windowSize;i>(windowSize-finalbin-(rolloff/2));i--){
        resultValue[i] = resultValue[windowSize-i];
    }

    return resultValue;
}
