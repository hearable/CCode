#include "CyclicShift.h"

void CyclicForwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency){

    int offset = (windowSize) * ((shiftFrequency/sampleFrequency)+(1.0d/(windowSize+1)));
    double* tempDataRe = (double*) calloc(windowSize, sizeof(double));
    double* tempDataIm = (double*) calloc(windowSize, sizeof(double));

    for(int i=0;i<windowSize;i++){
        tempDataRe[i] = spectrumRe[i];
        tempDataIm[i] = spectrumIm[i];
    }

    for(int i=0;i<windowSize;i++){
        spectrumRe[((i+offset)%windowSize)] = tempDataRe[i];
        spectrumIm[((i+offset)%windowSize)] = tempDataIm[i];
    }

    free(tempDataRe);
    free(tempDataIm);
    return;
}
void CyclicBackwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency){

    int offset = (windowSize) * ((shiftFrequency/sampleFrequency)+(1.0d/(windowSize+1)));
    double* tempDataRe = (double*) calloc(windowSize, sizeof(double));
    double* tempDataIm = (double*) calloc(windowSize, sizeof(double));

    for(int i=0;i<windowSize;i++){
        tempDataRe[i] = spectrumRe[i];
        tempDataIm[i] = spectrumIm[i];
    }

    for(int i=0;i<windowSize;i++){
        spectrumRe[i] = tempDataRe[((i+offset)%windowSize)];
        spectrumIm[i] = tempDataIm[((i+offset)%windowSize)];
    }

    free(tempDataRe);
    free(tempDataIm);
    return;
}

void CyclicCosineForwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency){

    double* tempDataRe1 = (double*) calloc(windowSize, sizeof(double));
    double* tempDataIm1 = (double*) calloc(windowSize, sizeof(double));

    double* tempDataRe2 = (double*) calloc(windowSize, sizeof(double));
    double* tempDataIm2 = (double*) calloc(windowSize, sizeof(double));

    for(int i=0;i<windowSize;i++){
        tempDataRe1[i] = spectrumRe[i];
        tempDataIm1[i] = spectrumIm[i];
        tempDataRe2[i] = spectrumRe[i];
        tempDataIm2[i] = spectrumIm[i];
    }

    CyclicForwardShift(tempDataRe1, tempDataIm1, windowSize, sampleFrequency, shiftFrequency);
    CyclicBackwardShift(tempDataRe2, tempDataIm2, windowSize, sampleFrequency, shiftFrequency);

    for(int i=0;i<windowSize;i++){
        spectrumRe[i] = (1.0d/2)*(tempDataRe1[i]+tempDataRe2[i]);
        spectrumIm[i] = (1.0d/2)*(tempDataIm1[i]+tempDataIm2[i]);
    }

    free(tempDataRe1);
    free(tempDataRe2);
    free(tempDataIm1);
    free(tempDataIm2);
    return;
}
