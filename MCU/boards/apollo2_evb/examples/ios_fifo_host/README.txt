This program implements the host part of a protocol for data exchange with IOS
Host sends one byte commands on SPI/I2C by writing at offset 0x80.

The command is used by the host to Start/Stop Data accumulation, and also
to acknowledge read-complete of a block of data
Once Asked to start accumulate data (using START command), two CTimer based
events emulate sensors sending data to IOS.

When IOS has some data for host, it implements a state
machine, synchronizing with the host.

Host is interrupted to indicate data availability. Host then reads the available
data (as indicated by FIFOCTR) by READing using IOS FIFO (at address 0x7F).

IOS keeps accumulating any new data coming in the background.

Host sends an acknowledgement to IOS once it has finished reading a block
of data initiated by IOS (partitally or complete). IOS interrupts the host
again if and when it has more data for the host to read, and the cycle
repeats - till host indicates that it is no longer interested in receiving
data by sending STOP command.


SWO is configured in 1M baud, 8-n-1 mode.
@verbatim
PIN Fly Lead Assumptions for the I/O Master (IOM0) Board (this one):
GPIO[10] == GPIO Interrupt from slave board
GPIO[5] == IOM0 SPI SCK/I2C SCL
GPIO[6] == IOM0 SPI MISO/I2C SDA
GPIO[7] == IOM0 SPI MOSI
GPIO[11] == IOM0 SPI nCE

PIN Fly Lead Assumptions for the I/O Slave (IOS) Board (the other one):
GPIO[0] == IOS SPI SCK/I2C SCL
GPIO[1] == IOS SPI MISO/I2C SDA
GPIO[2] == IOS SPI MOSI
GPIO[3] == IOS SPI nCE
GPIO[4] == Interrupt from slave board
@endverbatim

