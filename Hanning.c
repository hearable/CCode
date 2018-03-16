#include "Hanning.h"
#include <string.h>

double* HanningWindow(int windowSize, HanningPeriodicity periodicity){
    double *hanningValues;
    int effectiveWindowSize = 0;
    int halfWindowSize = 0;
    int copyIndex = 0;

    hanningValues = (double*) calloc(windowSize, sizeof(double)); // Allocate needed memory
    memset(hanningValues, 0, windowSize*sizeof(double)); // Make sure allocated memory is all zeros t first

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

double* ReturnWindowOutputHanning(int windowSize, double overlapPercentage, double* overlapAddResult, OverlapPart part){
    double* returnValue;
    int overlapIndex = (int)(windowSize*(1-overlapPercentage))+1;
    switch(part){
        case(HEAD):
            returnValue = (double*) calloc(overlapIndex, sizeof(double));
            for(int i=overlapIndex;i<2*overlapIndex;i++){
                returnValue[i-overlapIndex] = overlapAddResult[i];
            }
        break;
        case(MID):
            returnValue = (double*) calloc(windowSize-overlapIndex+1, sizeof(double));
            for(int i=overlapIndex;i<windowSize+1;i++){
                returnValue[i-overlapIndex] = overlapAddResult[i];
            }
        break;
        case(TAIL):
            returnValue = (double*) calloc(overlapIndex, sizeof(double));
            for(int i=windowSize;i<windowSize+overlapIndex;i++){
                returnValue[i-windowSize] = overlapAddResult[i];
            }
        break;
    }
    return returnValue;
}

double* OverlapAddHanning(int windowSize, double overlapPercentage, double* currentWindowRe, double* previousWindowRe){
    double* returnValue;
    int overlapIndex = (int)((windowSize+1)*(1-overlapPercentage));

    returnValue = (double*) calloc(windowSize+overlapIndex, sizeof(double));

    for(int i=0;i<overlapIndex;i++){
        returnValue[i] = previousWindowRe[i];
    }
    for(int i=overlapIndex;i<windowSize;i++){
        returnValue[i] = currentWindowRe[i-overlapIndex] + previousWindowRe[i];
    }
    for(int i=windowSize;i<windowSize+overlapIndex;i++){
        returnValue[i] = currentWindowRe[i-overlapIndex];
    }

    return returnValue;
}

