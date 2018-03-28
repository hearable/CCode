#ifndef LOWPASS_H_INCLUDED
#define LOWPASS_H_INCLUDED

float* LowpassFilter(int windowSize, float sampleFrequency, float cutOffFrequency, int rolloff);

#endif // LOWPASS_H_INCLUDED
