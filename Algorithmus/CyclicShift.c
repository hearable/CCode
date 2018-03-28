#include "CyclicShift.h"
#include <stdlib.h>
#include <math.h>



// This function is implemented in the frequency domain
void CyclicForwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency){

    int offset = (windowSize) * ((shiftFrequency/sampleFrequency)+(1.0f/(windowSize+1))); // Offset in Binarray; Round up to the next bin
    float* tempDataRe = (float*) calloc(windowSize, sizeof(float));
    float* tempDataIm = (float*) calloc(windowSize, sizeof(float));

    for(int i=0;i<windowSize;i++){
        tempDataRe[i] = spectrumRe[i];
        tempDataIm[i] = spectrumIm[i];
    }

    for(int i=0;i<windowSize;i++){
        spectrumRe[((i+offset)%windowSize)] = tempDataRe[i]; // Shift the spectrum by offset
        spectrumIm[((i+offset)%windowSize)] = tempDataIm[i];
    }

    free(tempDataRe);
    free(tempDataIm);
    return;
}

// This function is implemented in the frequency domain
void CyclicBackwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency){

    int offset = (windowSize) * ((shiftFrequency/sampleFrequency)+(1.0f/(windowSize+1)));
    float* tempDataRe = (float*) calloc(windowSize, sizeof(float));
    float* tempDataIm = (float*) calloc(windowSize, sizeof(float));

    for(int i=0;i<windowSize;i++){
        tempDataRe[i] = spectrumRe[i];
        tempDataIm[i] = spectrumIm[i];
    }

    for(int i=0;i<windowSize;i++){
        spectrumRe[i] = tempDataRe[((i+offset)%windowSize)]; // Shift the spectrum backwards
        spectrumIm[i] = tempDataIm[((i+offset)%windowSize)];
    }

    free(tempDataRe);
    free(tempDataIm);
    return;
}

// CosineForwardShift means that the time function stays real by multiplying it with a Cosine.
// This function is implemented in the frequency domain
void CyclicCosineForwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency){

    float* tempDataRe1 = (float*) calloc(windowSize, sizeof(float));
    float* tempDataIm1 = (float*) calloc(windowSize, sizeof(float));

    float* tempDataRe2 = (float*) calloc(windowSize, sizeof(float));
    float* tempDataIm2 = (float*) calloc(windowSize, sizeof(float));

    for(int i=0;i<windowSize;i++){
        tempDataRe1[i] = spectrumRe[i];
        tempDataIm1[i] = spectrumIm[i];
        tempDataRe2[i] = spectrumRe[i];
        tempDataIm2[i] = spectrumIm[i];
    }

    // Cos(x) = 1/2(e^(ix)+e^(-ix))

    CyclicForwardShift(tempDataRe1, tempDataIm1, windowSize, sampleFrequency, shiftFrequency);
    CyclicBackwardShift(tempDataRe2, tempDataIm2, windowSize, sampleFrequency, shiftFrequency);

    for(int i=0;i<windowSize;i++){
        spectrumRe[i] = (1.0f/2)*(tempDataRe1[i]+tempDataRe2[i]);
        spectrumIm[i] = (1.0f/2)*(tempDataIm1[i]+tempDataIm2[i]);
    }

    free(tempDataRe1);
    free(tempDataRe2);
    free(tempDataIm1);
    free(tempDataIm2);
    return;
}

void CyclicCosineForwardShiftInTime(float* data, int windowSize, float sampleFrequency, float shiftFrequency){
		for(int i=0;i<windowSize;i++){
			data[i] *= cos(2*PI*i*(shiftFrequency/sampleFrequency));
		}
}

void CyclicForwardShiftInTime(float* data, int windowSize, float* vector){
		for(int i=0;i<windowSize;i++){
			data[i] *= vector[i];
		}
}

void InitializeCosineVector(float* vector, int windowSize, float sampleFrequency, float shiftFrequency){
		for(int i=0;i<windowSize;i++){
			vector[i] = cos(2*PI*(shiftFrequency/sampleFrequency)*i);
		}
}
