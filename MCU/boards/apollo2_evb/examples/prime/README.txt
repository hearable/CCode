This example consists of a non-optimized, brute-force routine for computing
the number of prime numbers between 1 and a given value, N. The routine
uses modulo operations to determine whether a value is prime or not. While
obviously not optimal, it is very useful for exercising the core.

For this example, N is 100000, for which the answer is 9592.
Apollo2 at 48MHz takes less than 2 1/4 minutes to determine that answer
when compiled with IAR v8.11.

The goal of this example is to measure power consumption while the core
is working to compute the answer.

The example prints an initial banner to the UART port.  After each prime
loop, it enables the UART long enough to print the answer, disables the
UART and starts the computation again.

Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
Please note that text end-of-line is a newline (LF) character only.
Therefore, the UART terminal must be set to simulate a CR/LF.

