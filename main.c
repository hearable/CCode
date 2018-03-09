#include <stdio.h>
#include <stdlib.h>
#include "Hanning.c"
#include "FFT.c"
#include "BlackmanNuttall.c"

int main()
{
    double sampleRe[] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    double sampleIm[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    double resultRe[64];
    double resultIm[64];

    int windowSize = 32;
    float* hanning = Hanning(windowSize, PERIODIC);
    float* blackman = BlackmanNutall(windowSize);
    //float hanning[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    /*for(int i=0;i<windowSize;i++){
        printf("%f \n", hanning[i]);
    }*/

    for(int i=0;i<windowSize;i++){
        printf("%f \n", blackman[i]);
    }

    double window1Re[32];
    double window1Im[32];
    double window2Re[32];
    double window2Im[32];
    double window3Re[32];
    double window3Im[32];

    for(int i=0;i<32;i++){
        window1Re[i] = sampleRe[i] * hanning[i];
        window1Im[i] = sampleIm[i] * hanning[i];
        window2Re[i] = sampleRe[i+16] * hanning[i];
        window2Im[i] = sampleIm[i+16] * hanning[i];
        window3Re[i] = sampleRe[i+32] * hanning[i];
        window3Im[i] = sampleIm[i+32] * hanning[i];
    }

    for(int i=0;i<16;i++){
        resultRe[i] = window1Re[i];
        resultRe[i+16] = window1Re[i+16] + window2Re[i];
        resultRe[i+32] = window2Re[i+16] + window3Re[i];
        resultRe[i+48] = window3Re[i+16];

        resultIm[i] = window1Im[i];
        resultIm[i+16] = window1Im[i+16] + window2Im[i];
        resultIm[i+32] = window2Im[i+16] + window3Im[i];
        resultIm[i+48] = window3Im[i+16];
    }

    Fft_transform(resultRe, resultIm, 64);

    for(int i=0;i<64;i++){
        printf("%f \n", resultRe[i]);
    }

    return 0;
}
