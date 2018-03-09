#ifndef HANNING_H_INCLUDED
#define HANNING_H_INCLUDED

#include <stdio.h>
#include <math.h>

#define PI 3.14159265359

// Defines whether the periodic or the symmetric version should be used
typedef enum {SYMMETRIC, PERIODIC} HanningPeriodicity ;

// Returns an array with the hanning function
/* PRE:
log2(windowSize) is an integer.
periodicity is either symmetric for creating FIR filters from IIR filters or
periodic for STFT applications.
*/
float* Hanning(int windowSize, HanningPeriodicity periodicity);
/* POST:
Returns the corresponding Hanning-Window
*/
#endif // HANNING_H_INCLUDED
