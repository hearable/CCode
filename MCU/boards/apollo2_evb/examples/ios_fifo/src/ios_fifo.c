//*****************************************************************************
//
//! @file ios_fifo.c
//!
//! @brief Example demonstrating use of IOS FIFO.(use with ios_fifo_host)
//!
//! This program implements a protocol for data exchange with host.
//! Host sends one byte commands on SPI/I2C by writing at offset 0x80.
//!
//! The command is used by the host to Start/Stop Data accumulation, and also
//! to acknowledge read-complete of a block of data
//! Once Asked to start accumulate data (using START command), two CTimer based
//! events emulate sensors sending data to IOS.
//!
//! When IOS has some data for host, it implements a state
//! machine, synchronizing with the host.
//!
//! Host is interrupted to indicate data availability. Host then reads the available
//! data (as indicated by FIFOCTR) by READing using IOS FIFO (at address 0x7F).
//!
//! IOS keeps accumulating any new data coming in the background.
//!
//! Host sends an acknowledgement to IOS once it has finished reading a block
//! of data initiated by IOS (partitally or complete). IOS interrupts the host
//! again if and when it has more data for the host to read, and the cycle
//! repeats - till host indicates that it is no longer interested in receiving
//! data by sending STOP command.
//!
//!
//! SWO is configured in 1M baud, 8-n-1 mode.
//! @verbatim
//! PIN Fly Lead Assumptions for the I/O Master (IOM0) Board (the other one):
//!        GPIO[10] == GPIO Interrupt from slave board
//!        GPIO[5] == IOM0 SPI SCK/I2C SCL
//!        GPIO[6] == IOM0 SPI MISO/I2C SDA
//!        GPIO[7] == IOM0 SPI MOSI
//!        GPIO[11] == IOM0 SPI nCE
//!
//! PIN Fly Lead Assumptions for the I/O Slave (IOS) Board (this one):
//!        GPIO[0] == IOS SPI SCK/I2C SCL
//!        GPIO[1] == IOS SPI MISO/I2C SDA
//!        GPIO[2] == IOS SPI MOSI
//!        GPIO[3] == IOS SPI nCE
//!        GPIO[4] == Interrupt from slave board
//! @endverbatim
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2017, Ambiq Micro
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 1.2.11 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define     USE_SPI             1   // 0 = I2C, 1 = SPI
#define     I2C_ADDR            0x10

#define     SENSOR0_DATA_SIZE           200
#define     SENSOR1_DATA_SIZE           500

// Sensor Frequencies as factor of 12KHz
// This test silently 'drops' the sensor data if the FIFO can not accomodate it
// Hence the host data validation test would still pass as long as all the data
// written to the FIFO made it to the host intact
// This allows us to configure these values to unrealistically high values for
// testing purpose
#if 1
#define     SENSOR0_FREQ   12 // 12 times a second
#define     SENSOR1_FREQ    7 // 7 times a second
#else // Just to test the throughput
#define     SENSOR0_FREQ   1000 // 1KHz
#define     SENSOR1_FREQ   1700 // 1.7 KHz
#endif

#define     XOR_BYTE            0
#define     AM_HAL_IOS_INT_ERR  (AM_HAL_IOS_INT_FOVFL | AM_HAL_IOS_INT_FUNDFL | AM_HAL_IOS_INT_FRDERR)

typedef enum
{
    AM_IOSTEST_CMD_START_DATA    = 0,
    AM_IOSTEST_CMD_STOP_DATA     = 1,
    AM_IOSTEST_CMD_ACK_DATA      = 2,
} AM_IOSTEST_CMD_E;

#define AM_IOSTEST_IOSTOHOST_DATAAVAIL_INTMASK  1

typedef enum
{
    AM_IOSTEST_SLAVE_STATE_NODATA   = 0,
    AM_IOSTEST_SLAVE_STATE_DATA     = 1,
} AM_IOSTEST_SLAVE_STATE_E;

volatile AM_IOSTEST_SLAVE_STATE_E g_iosState;
volatile uint32_t g_sendIdx = 0;
volatile bool g_bSensor0Data, g_bSensor1Data;

//*****************************************************************************
//
// Message buffers.
//
// data from the IOS interface, which is only 8 bits wide.
//
//*****************************************************************************
#define AM_TEST_REF_BUF_SIZE    512
uint8_t g_pui8TestBuf[AM_TEST_REF_BUF_SIZE];


#define AM_IOS_TX_BUFSIZE_MAX   1023
uint8_t g_pui8TxFifoBuffer[AM_IOS_TX_BUFSIZE_MAX];

//*****************************************************************************
//
// SPI Slave Configuration
//
//*****************************************************************************
static am_hal_ios_config_t g_sIOSSpiConfig =
{
    // Configure the IOS in SPI mode.
    .ui32InterfaceSelect = AM_HAL_IOS_USE_SPI,

    // Eliminate the "read-only" section, so an external host can use the
    // entire "direct write" section.
    .ui32ROBase = 0x78,

    // Making the "FIFO" section as big as possible.
    .ui32FIFOBase = 0x80,

    // We don't need any RAM space, so extend the FIFO all the way to the end
    // of the LRAM.
    .ui32RAMBase = 0x100,

    // FIFO Threshold - set to half the size
    .ui32FIFOThreshold = 0x20,
};

//*****************************************************************************
//
// I2C Slave Configuration
//
//*****************************************************************************
am_hal_ios_config_t g_sIOSI2cConfig =
{
    // Configure the IOS in I2C mode.
    .ui32InterfaceSelect = AM_HAL_IOS_USE_I2C | AM_HAL_IOS_I2C_ADDRESS(I2C_ADDR << 1),

    // Eliminate the "read-only" section, so an external host can use the
    // entire "direct write" section.
    .ui32ROBase = 0x78,

    // Set the FIFO base to the maximum value, making the "direct write"
    // section as big as possible.
    .ui32FIFOBase = 0x80,

    // We don't need any RAM space, so extend the FIFO all the way to the end
    // of the LRAM.
    .ui32RAMBase = 0x100,
    // FIFO Threshold - set to half the size
    .ui32FIFOThreshold = 0x40,
};

//*****************************************************************************
//
// Timer handling to emulate sensor data
//
//*****************************************************************************
static am_hal_ctimer_config_t g_sTimer =
{
    // Don't link timers.
    0,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_HFRC_12KHZ),

    // No configuration for TimerB.
    0,
};

// Timer Interrupt Service Routine (ISR)
void
am_ctimer_isr(void)
{
    uint32_t ui32Status;

    ui32Status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(ui32Status);

    am_hal_ctimer_int_service(ui32Status);
}

// Emulate Sensor0 New Data
void
timer0_handler(void)
{

    // Inform main loop of sensor 0 Data availability
    g_bSensor0Data = true;
}

// Emulate Sensor1 New Data
void
timer1_handler(void)
{
    // Inform main loop of sensor 1 Data availability
    g_bSensor1Data = true;
}

void
stop_sensors(void)
{
    //
    // Stop timer A0
    //
    am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);
    //
    // Stop timer A1
    //
    am_hal_ctimer_stop(1, AM_HAL_CTIMER_TIMERA);
}

void
start_sensors(void)
{
    stop_sensors(); // Just in case host died without sending STOP last time
    // Initialize Data Buffer Index
    g_sendIdx = 0;
    //
    // Start timer A0
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
    //
    // Start timer A1
    //
    am_hal_ctimer_start(1, AM_HAL_CTIMER_TIMERA);
    g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
}

void
init_sensors(void)
{
    uint32_t ui32Period;

    //
    // Set up timer A0 & A1.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer);
    am_hal_ctimer_clear(1, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(1, &g_sTimer);

    //
    // Set up timerA0 for Sensor 0 Freq
    //
    ui32Period = 12000 / SENSOR0_FREQ ;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));
    //
    // Set up timerA1 for Sensor 1 Freq
    //
    ui32Period = 12000 / SENSOR1_FREQ ;
    am_hal_ctimer_period_set(1, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA1);

    //
    // Enable the timer Interrupt.
    //
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0,
                               timer0_handler);
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA1,
                               timer1_handler);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA1);

    //
    // Enable the timer interrupt in the NVIC.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_CTIMER);
    am_hal_interrupt_master_enable();
}

//*****************************************************************************
//
// Configure the SPI slave.
//
//*****************************************************************************
static void
ios_set_up(bool bSpi)
{
    if (bSpi)
    {
        // Configure SPI interface
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_SCK, AM_BSP_GPIO_CFG_IOS_SCK);
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_MISO, AM_BSP_GPIO_CFG_IOS_MISO);
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_MOSI, AM_BSP_GPIO_CFG_IOS_MOSI);
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_nCE, AM_BSP_GPIO_CFG_IOS_nCE);
        //
        // Configure the IOS interface and LRAM structure.
        //
        am_hal_ios_config(&g_sIOSSpiConfig);
    }
    else
    {
        // Configure I2C interface
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_SCL, AM_BSP_GPIO_CFG_IOS_SCL);
        am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_SDA, AM_BSP_GPIO_CFG_IOS_SDA);
        //
        // Configure the IOS interface and LRAM structure.
        //
        am_hal_ios_config(&g_sIOSI2cConfig);
    }


    //
    // Clear out any IOS register-access interrupts that may be active, and
    // enable interrupts for the registers we're interested in.
    //
    am_hal_ios_access_int_clear(AM_HAL_IOS_ACCESS_INT_ALL);
    am_hal_ios_int_clear(AM_HAL_IOS_INT_ALL);
    am_hal_ios_access_int_enable(AM_HAL_IOS_ACCESS_INT_00);
    am_hal_ios_int_enable(AM_HAL_IOS_INT_ERR | AM_HAL_IOS_INT_FSIZE);

    // Preparation of FIFO
    am_hal_ios_fifo_buffer_init( &g_pui8TxFifoBuffer[0], AM_IOS_TX_BUFSIZE_MAX);

    //
    // Set the bit in the NVIC to accept access interrupts from the IO Slave.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSACC);
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSLAVE);

    // Set up the IOSINT interrupt pin
    am_hal_gpio_pin_config(AM_BSP_GPIO_IOS_INT, AM_BSP_GPIO_CFG_IOS_INT);

}

// Inform host of new data available to read
void
inform_host(void)
{
    // Update FIFOCTR for host to read
    am_hal_ios_update_fifoctr();
    // Interrupt the host
    am_hal_ios_host_int_set(AM_IOSTEST_IOSTOHOST_DATAAVAIL_INTMASK);
}

//*****************************************************************************
//
// IO Slave Register Access ISR.
//
//*****************************************************************************
void
am_ioslave_acc_isr(void)
{
    uint32_t ui32Status;
    uint8_t  *pui8Packet;

    //
    // Set up a pointer for writing 32-bit aligned packets through the IO slave
    // interface.
    //
    pui8Packet = (uint8_t *) am_hal_ios_pui8LRAM;

    //
    // Check to see what caused this interrupt, then clear the bit from the
    // interrupt register.
    //
    ui32Status = am_hal_ios_access_int_status_get(false);
    am_hal_ios_access_int_clear(ui32Status);

    if ( ui32Status & AM_HAL_IOS_ACCESS_INT_00 )
    {
        // Received command from Host
        // Figure out what to do next based on the command.
        //
        switch(pui8Packet[0])
        {
            case AM_IOSTEST_CMD_START_DATA:
                // Host wants to start data exchange
                // Start the Sensor Emulation
                start_sensors();
                break;

            case AM_IOSTEST_CMD_STOP_DATA:
                // Host no longer interested in data from us
                // Stop the Sensor emulation
                stop_sensors();
                g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
                break;

            case AM_IOSTEST_CMD_ACK_DATA:
                // Host done reading the last block signalled
                // Check if any more data available
                if (am_hal_ios_fifo_space_used())
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_DATA;
                    inform_host();
                }
                else
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
                }
                break;

            default:
                break;
        }
    }
}

//*****************************************************************************
//
// IO Slave Main ISR.
//
//*****************************************************************************
void
am_ioslave_ios_isr(void)
{
    uint32_t ui32Status;

    //
    // Check to see what caused this interrupt, then clear the bit from the
    // interrupt register.
    //
    ui32Status = am_hal_ios_int_status_get(false);
    am_hal_ios_int_clear(ui32Status);

    if (ui32Status & AM_HAL_IOS_INT_FUNDFL)
    {
        am_util_stdio_printf("Hitting underflow for the requested IOS FIFO transfer\n");
        // We should never hit this case unless the threshold has beeen set
        // incorrect, or we are unable to handle the data rate
        // ERROR!
        am_hal_debug_assert_msg(0,
            "Hitting underflow for the requested IOS FIFO transfer.");
    }

    if (ui32Status & AM_HAL_IOS_INT_ERR)
    {
        // We should never hit this case
        // ERROR!
        am_hal_debug_assert_msg(0,
            "Hitting ERROR case.");
    }

    if (ui32Status & AM_HAL_IOS_INT_FSIZE)
    {
        //
        // Service the I2C slave FIFO if necessary.
        //
        am_hal_ios_fifo_service(ui32Status);
    }
}

//*****************************************************************************
//
// Start up the ITM interface.
//
//*****************************************************************************
void
itm_start(void)
{
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_util_stdio_printf_init((am_util_stdio_print_char_t) am_bsp_itm_string_print);

    //
    // Initialize the SWO GPIO pin
    //
    am_bsp_pin_enable(ITM_SWO);

    //
    // Enable the ITM.
    //
    am_hal_itm_enable();

    //
    // Enable debug printf messages using ITM on SWO pin
    //
    am_bsp_debug_printf_enable();
    //
    // Clear the terminal.
    //
    am_util_stdio_terminal_clear();
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    int i;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_enable(&am_hal_cachectrl_defaults);

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    itm_start();

#if 0 // For Debug only
    //
    // Set up GPIO on Pin 23, 24 & 48 for timing debug.
    //
    am_hal_gpio_out_bit_clear(23);
    am_hal_gpio_pin_config(23, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(24);
    am_hal_gpio_pin_config(24, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(48);
    am_hal_gpio_pin_config(48, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(43);
    am_hal_gpio_pin_config(43, AM_HAL_GPIO_OUTPUT);
#endif

    // Initialize Test Data
    for (i = 0; i < AM_TEST_REF_BUF_SIZE; i++)
    {
        g_pui8TestBuf[i] = (i & 0xFF) ^ XOR_BYTE;
    }

    init_sensors();
    //
    // Enable the IOS. Choose the correct protocol based on USE_SPI
    //
    ios_set_up(USE_SPI);

    //
    // Enable interrupts so we can receive messages from the boot host.
    //
    am_hal_interrupt_master_enable();

    //
    // Loop forever.
    //
    while(1)
    {
        uint32_t numWritten = 0;
        uint32_t chunk1;
        //
        uint32_t ui32IntStatus = am_hal_interrupt_master_disable();
        if (g_bSensor0Data || g_bSensor1Data)
        {
            // Enable the interrupts
            am_hal_interrupt_master_set(ui32IntStatus);
            if (g_bSensor0Data)
            {
                chunk1 = AM_TEST_REF_BUF_SIZE - g_sendIdx;
                if (chunk1 > SENSOR0_DATA_SIZE)
                {
                    numWritten = am_hal_ios_fifo_write(&g_pui8TestBuf[g_sendIdx], SENSOR0_DATA_SIZE);
                }
                else
                {
                    numWritten = am_hal_ios_fifo_write(&g_pui8TestBuf[g_sendIdx], chunk1);
                    if (numWritten == chunk1)
                    {
                        numWritten += am_hal_ios_fifo_write(&g_pui8TestBuf[0], SENSOR0_DATA_SIZE - chunk1);
                    }
                }

                g_sendIdx += numWritten;
                g_sendIdx %= AM_TEST_REF_BUF_SIZE;
                g_bSensor0Data = false;
            }
            if (g_bSensor1Data)
            {
                chunk1 = AM_TEST_REF_BUF_SIZE - g_sendIdx;
                if (chunk1 > SENSOR1_DATA_SIZE)
                {
                    numWritten = am_hal_ios_fifo_write(&g_pui8TestBuf[g_sendIdx], SENSOR1_DATA_SIZE);
                }
                else
                {
                    numWritten = am_hal_ios_fifo_write(&g_pui8TestBuf[g_sendIdx], chunk1);
                    if (numWritten == chunk1)
                    {
                        numWritten += am_hal_ios_fifo_write(&g_pui8TestBuf[0], SENSOR1_DATA_SIZE - chunk1);
                    }
                }

                g_sendIdx += numWritten;
                g_sendIdx %= AM_TEST_REF_BUF_SIZE;
                g_bSensor1Data = false;
            }
            // If we were Idle - need to inform Host if there is new data
            if (g_iosState == AM_IOSTEST_SLAVE_STATE_NODATA)
            {
                if (am_hal_ios_fifo_space_used())
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_DATA;
                    inform_host();
                }
            }
        }
        else
        {
            //
            // Go to Deep Sleep.
            //
            am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_NORMAL);
            // Enable the interrupts
            am_hal_interrupt_master_set(ui32IntStatus);
        }
    }
}
