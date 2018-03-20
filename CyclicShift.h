#ifndef CYCLICSHIFT_H_INCLUDED
#define CYCLICSHIFT_H_INCLUDED

void CyclicBackwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency);
void CyclicForwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency);
void CyclicCosineForwardShift(double* spectrumRe, double* spectrumIm, int windowSize, double sampleFrequency, double shiftFrequency);

#endif // CYCLICSHIFT_H_INCLUDED
