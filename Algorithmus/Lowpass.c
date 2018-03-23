#include "Lowpass.h"
#include "Hanning.h"

// implements Hanning-Lowpass, n bin rolloff
double* LowpassFilter(int windowSize, double sampleFrequency, double cutOffFrequency, int rolloff){
    double* resultValue = (double*) calloc(windowSize, sizeof(double));

    int finalbin = (windowSize) * ((cutOffFrequency/sampleFrequency)+(1.0d/(windowSize+1)));

    double* hanningWindow = HanningWindow(2*rolloff, SYMMETRIC);

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
