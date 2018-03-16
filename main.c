#include <stdio.h>
#include <stdlib.h>
#include "Hanning.c"
#include "FFT.c"
#include "math.h"

void ApplyWindow(int windowSize, double* currentWindowRe, double* currentWindowIm, double* previousWindowRe, double* previousWindowIm, double* currentSample, double* window){
    for(int i=0;i<windowSize;i++){
        previousWindowRe[i] = currentWindowRe[i];
        previousWindowIm[i] = currentWindowIm[i];
        currentWindowRe[i] = currentSample[i] * window[i];
        currentWindowIm[i] = 0;
    }
}

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
// Getsamples -> ApplyWindow -> OverlapAdd -> ReturnWindowOutput
int main(){
//    double* result1;
//    double* result2;
//
//    double* hanningWindow;
//
//    double* previousWindowRe;
//    double* previousWindowIm;
//
//    double* currentWindowRe;
//    double* currentWindowIm;
//
//    double* currentSample;
//
//    int windowSize = 32;
//    double overlappercentage = 0.5;
//    int overlapIndex = (int)((windowSize+1)*(1-overlappercentage));
//
//    previousWindowRe = (double*) calloc(windowSize, sizeof(double));
//    previousWindowIm = (double*) calloc(windowSize, sizeof(double));
//    currentWindowRe = (double*) calloc(windowSize, sizeof(double));
//    currentWindowIm = (double*) calloc(windowSize, sizeof(double));
//    currentSample = (double*) calloc(windowSize, sizeof(double));
//    hanningWindow = HanningWindow(windowSize, PERIODIC);
//
//    int imax = 10;
//
//    double* result = (double*) calloc(10*windowSize, sizeof(double));;
//    int t = 0;
//
//    for(int i=0;i<=imax;i++){
//        if(i == 0){
//            GetSamples(windowSize, currentSample);
//            ApplyWindow(windowSize, currentWindowRe, currentWindowIm, previousWindowRe, previousWindowIm, currentSample, hanningWindow);
//            result1 = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);
//            result1 = ReturnWindowOutputHanning(windowSize,overlappercentage,result1,HEAD);
//            for(int i=0;i<overlapIndex;i++){
//                printf("%f\t%f\n", result1[i], currentSample[i]);
//                result[t] = result1[i];
//                t++;
//            }
//        } else if(i == imax){
//            GetSamples(windowSize, currentSample);
//            ApplyWindow(windowSize, currentWindowRe, currentWindowIm, previousWindowRe, previousWindowIm, currentSample, hanningWindow);
//            result1 = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);
//            result2 = ReturnWindowOutputHanning(windowSize,overlappercentage,result1,TAIL);
//            result1 = ReturnWindowOutputHanning(windowSize,overlappercentage,result1,MID);
//            for(int i=0;i<windowSize-overlapIndex;i++){
//                printf("%f\t%f\n", result1[i], currentSample[i]);
//                result[t] = result1[i];
//                t++;
//            }
//            for(int i=0;i<overlapIndex;i++){
//                printf("%f\t%f\n", result2[i], currentSample[windowSize-overlapIndex+i]);
//                result[t] = result2[i];
//                t++;
//            }
//        } else {
//            GetSamples(windowSize, currentSample);
//            ApplyWindow(windowSize, currentWindowRe, currentWindowIm, previousWindowRe, previousWindowIm, currentSample, hanningWindow);
//            result1 = OverlapAddHanning(windowSize, overlappercentage, currentWindowRe, previousWindowRe);
//            result1 = ReturnWindowOutputHanning(windowSize,overlappercentage,result1,MID);
//            for(int i=0;i<windowSize-overlapIndex;i++){
//                printf("%f\t%f\n", result1[i], currentSample[i]);
//                result[t] = result1[i];
//                t++;
//            }
//        }
//    }
//
//    double* samplere = (double*) calloc(t, sizeof(double));
//    double* sampleim = (double*) calloc(t, sizeof(double));
//
//    double* resultim = (double*) calloc(t, sizeof(double));
//    double* resultre = (double*) calloc(t, sizeof(double));
//
//
//    for(int i=0;i<t;i++){
//        resultre[i] = result[i];
//        samplere[i] = 1;
//    }
//
//    Fft_transform(resultre,resultim,t);
//    Fft_transform(samplere,sampleim,t);
//
//    for(int i=0;i<t;i++){
//        printf("%f\t%f\t%f\t%f\n", resultre[i],resultim[i],samplere[i],sampleim[i]);
//    }

    int windowSize = 32;

    double* currentSample = (double*) calloc(windowSize, sizeof(double));
    double* hanningWindow = HanningWindow(windowSize, PERIODIC);
    double* cosine = (double*) calloc(windowSize, sizeof(double));

    GetSamples(windowSize, currentSample);
    for(int i=0;i<windowSize;i++){
        cosine[i] =
    }

    return 0;
}
