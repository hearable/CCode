#ifndef LOWPASS_H_INCLUDED
#define LOWPASS_H_INCLUDED

double* LowpassFilter(int windowSize, double sampleFrequency, double cutOffFrequency, int rolloff);

#endif // LOWPASS_H_INCLUDED
