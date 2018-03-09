#include "BlackmanNutall.h"

float* BlackmanNutall(int effectiveWindowSize){

    float* filterValues;
    double c0,c1,c2,c3;

    filterValues = (float*) calloc(effectiveWindowSize, sizeof(float)); // Allocate needed memory
    memset(filterValues, 0, effectiveWindowSize*sizeof(float)); // Make sure allocated memory is all zeros t first

    c0 = 0.355768;
    c1 = -0.487396;
    c2 = 0.144232;
    c3 = -0.012604;

    int copyIndex;
    int halfWindowSize;

    if(effectiveWindowSize%2==0){
        halfWindowSize = effectiveWindowSize/2;
        for(int i=0; i<halfWindowSize; i++){ // Need only to calculate half the values; the function is symmetric
            filterValues[i] = c0 + c1*cos(2*PI*(i+1)/(effectiveWindowSize+1)) + c2*cos(4*PI*(i+1)/(effectiveWindowSize+1)) + c3*cos(6*PI*(i+1)/(effectiveWindowSize+1));
        }
        copyIndex = halfWindowSize - 1;
        for(int i=halfWindowSize; i<effectiveWindowSize; i++){ // Copy symmetric values
            filterValues[i] = filterValues[copyIndex];
            copyIndex--;
        }
    } else {
        halfWindowSize = (effectiveWindowSize+1)/2;
        for(int i=0; i<halfWindowSize; i++){ // Need only to calculate half the values; the function is symmetric
            filterValues[i] = c0 + c1*cos(2*PI*(i+1)/(effectiveWindowSize+1)) + c2*cos(4*PI*(i+1)/(effectiveWindowSize+1)) + c3*cos(6*PI*(i+1)/(effectiveWindowSize+1));
        }
        copyIndex = halfWindowSize - 2;
        for(int i=halfWindowSize; i<effectiveWindowSize; i++){ // Copy symmetric values
            filterValues[i] = filterValues[copyIndex];
            copyIndex--;
        }
    }
    return filterValues;
}
