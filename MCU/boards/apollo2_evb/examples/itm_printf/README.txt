This example walks through the ASCII table (starting at character 033('!')
and ending on 126('~')) and prints the character to the ITM. This output
can be decoded by running a terminal emulator and configuring the console
for SWO at 1M Baud. This example works by configuring a timer and printing
a new character after ever interrupt and sleeps in between timer interrupts.

