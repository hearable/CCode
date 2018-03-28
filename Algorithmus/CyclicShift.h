#ifndef CYCLICSHIFT_H_INCLUDED
#define CYCLICSHIFT_H_INCLUDED

#ifndef PI
	#define PI 3.14159265359
#endif

void CyclicForwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency);
void CyclicBackwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency);
void CyclicCosineForwardShift(float* spectrumRe, float* spectrumIm, int windowSize, float sampleFrequency, float shiftFrequency);
void CyclicCosineForwardShiftInTime(float* data, int windowSize, float sampleFrequency, float shiftFrequency);
void CyclicForwardShiftInTime(float* data, int windowSize, float* vector);
void InitializeCosineVector(float* vector, int windowSize, float sampleFrequency, float shiftFrequency);

#endif // UTILS_H_INCLUDED
