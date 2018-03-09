#include "Hanning.h"
#include <string.h>

float* Hanning(int windowSize, HanningPeriodicity periodicity){
    float *hanningValues;
    int effectiveWindowSize = 0;
    int halfWindowSize = 0;
    int copyIndex = 0;

    hanningValues = (float*) calloc(windowSize, sizeof(float)); // Allocate needed memory
    memset(hanningValues, 0, windowSize*sizeof(float)); // Make sure allocated memory is all zeros t first

    if(periodicity == PERIODIC){
        effectiveWindowSize = windowSize - 1;
    } else {
        effectiveWindowSize = windowSize;
    }

    if(effectiveWindowSize%2==0){
        halfWindowSize = effectiveWindowSize/2;
        for(int i=0; i<halfWindowSize; i++){ // Need only to calculate half the values; the function is symmetric
            hanningValues[i] = 0.5*(1-cos(2*PI*(i+1)/(effectiveWindowSize+1)));
        }
        copyIndex = halfWindowSize - 1;
        for(int i=halfWindowSize; i<effectiveWindowSize; i++){ // Copy symmetric values
            hanningValues[i] = hanningValues[copyIndex];
            copyIndex--;
        }
    } else {
        halfWindowSize = (effectiveWindowSize+1)/2;
        for(int i=0; i<halfWindowSize; i++){ // Need only to calculate half the values; the function is symmetric
            hanningValues[i] = 0.5*(1-cos(2*PI*(i+1)/(effectiveWindowSize+1)));
        }
        copyIndex = halfWindowSize - 2;
        for(int i=halfWindowSize; i<effectiveWindowSize; i++){ // Copy symmetric values
            hanningValues[i] = hanningValues[copyIndex];
            copyIndex--;
        }
    }

    if(periodicity == PERIODIC){ // periodic approach leads to constant amplitude with overlap-add approach
        for(int i=windowSize-1; i>0; i--){ // Push every value one position forward
            hanningValues[i] = hanningValues[i-1];
        }
        hanningValues[0] = 0.0f; // Set leftmost value to zero
    }

    return hanningValues;
}
