#ifndef HANNING_H_INCLUDED
#define HANNING_H_INCLUDED

#ifndef PI
	#define PI 3.14159265359
#endif

// Defines whether the periodic or the symmetric version should be used
typedef enum {SYMMETRIC, PERIODIC} HanningPeriodicity;

// Returns an array with the hanning function
/* PRE:
log2(windowSize) is an integer.
periodicity is either symmetric for creating FIR filters from IIR filters or
periodic for STFT applications.
*/
float* HanningWindow(int windowSize, HanningPeriodicity periodicity);
/* POST:
Returns the corresponding Hanning-Window
*/

/*
PRE:
windowSize is the dimension of currentWindowRe and previousWindowRe
*/
float* OverlapAddHanning(int windowSize, float overlapPercentage, float* currentWindowRe, float* previousWindowRe);
/* POST:
Returns the overlapped sum of two Hanning-Windows
*/

// HEAD is used for the initial part of the first window
// MID is used for any part of the window that is neither HEAD nor TAIL
// TAIL is the final part of the last window
typedef enum {HEAD, MID, TAIL} OverlapPart;

/*
Parses the HEAD/MID/TAIL portion of the output of OverlapAddHanning
*/
float* ReturnWindowOutputHanning(int windowSize, float overlapPercentage, float* overlapAddResult, OverlapPart part);
#endif // HANNING_H_INCLUDED
