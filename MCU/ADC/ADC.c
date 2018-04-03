//*****************************************************************************
//
//! @file adc_lpmode0.c
//!
//! @brief Example that takes samples with the ADC at high-speed.
//!
//! This example shows the CTIMER-A3 triggering repeated samples of an external
//! input at 1.2Msps in LPMODE0.  The example uses the CTIMER-A3 to trigger
//! ADC sampling.  Each data point is 128 sample average and is read from the
//! ADC FIFO into an SRAM circular buffer.
//!
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

//*****************************************************************************
//
// Define a circular buffer to hold the ADC samples
//
//*****************************************************************************
//#define ADC_SAMPLE_BUF_SIZE 1024 //windowsize hier anpassen => auskommentiert
//definiere main Funktion um Variabeln nachher zu haben
int main(uint32_t ADC_SAMPLE_BUF_SIZE, int* daten[3][ADC_SAMPLE_BUF_SIZE]);
//Index für den Buffer
#define ADC_SAMPLE_INDEX_M  0x3FF
uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE];
uint32_t g_ui32ADCSampleIndex = 0;

//*****************************************************************************
//
// Start up the ITM interface => Instrumentation Trace Macrocell
// Used for Debugging
//*****************************************************************************
void
itm_start(void)
{
    //
    // Initialize the printf interface for ITM/SWO output. (SWO => Single Wire Output)
    //
    am_util_stdio_printf_init((am_util_stdio_print_char_t) am_bsp_itm_string_print);

    //
    // Initialize the SWO GPIO pin
    //
    am_bsp_pin_enable(ITM_SWO);
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);

    //
    // Enable the ITM.
    //
    am_hal_itm_enable();

    //
    // Enable debug printf messages using ITM on SWO pin
    //
    am_bsp_debug_printf_enable();
}

//*****************************************************************************
//
// Configure the ADC.
//
//*****************************************************************************
void
adc_config(void)
{
		// "Variabeln" deklaration, Datenstruktur benutzt um den ADC zu konfigurieren
    am_hal_adc_config_t sADCConfig;

    //
    // Enable the ADC power domain.
    //
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_ADC);

    //ADC CONFIGURATIONS
    // Set up the ADC configuration parameters. These settings are reasonable
    // for accurate measurements at a low sample rate.
    //
    sADCConfig.ui32Clock = AM_HAL_ADC_CLOCK_HFRC;
    sADCConfig.ui32TriggerConfig = AM_HAL_ADC_TRIGGER_SOFT;
    sADCConfig.ui32Reference = AM_HAL_ADC_REF_INT_2P0;
    sADCConfig.ui32ClockMode = AM_HAL_ADC_CK_LOW_POWER;
    sADCConfig.ui32PowerMode = AM_HAL_ADC_LPMODE_0;
    sADCConfig.ui32Repeat = AM_HAL_ADC_REPEAT;
    am_hal_adc_config(&sADCConfig);

    // when to wake up
    // For this example, the samples will be coming in slowly. This means we
    // can afford to wake up for every conversion.
    //
    am_hal_adc_int_enable(AM_HAL_ADC_INT_FIFOOVR1);

    //
    // Set up an ADC slot
    //
    am_hal_adc_slot_config(0, AM_HAL_ADC_SLOT_AVG_128 |
                              AM_HAL_ADC_SLOT_14BIT |
                              AM_HAL_ADC_SLOT_CHSEL_SE0 |
                              AM_HAL_ADC_SLOT_ENABLE);
    //
    // Enable the ADC.
    //
    am_hal_adc_enable();
}

//*****************************************************************************
// TimeNumber of ADC needs to be 3, TimerSegment?
// Initialize the ADC repetitive sample timer A3.
//
//*****************************************************************************
void
init_timerA3_for_ADC(void)
{
    //
    // Start a timer to trigger the ADC periodically (1 second).
    // (TimerNumber, TimerSegment, CifigVal)
    am_hal_ctimer_config_single(3, AM_HAL_CTIMER_TIMERA,
                                   AM_HAL_CTIMER_HFRC_12MHZ |
                                   AM_HAL_CTIMER_FN_REPEAT |
                                   AM_HAL_CTIMER_INT_ENABLE |
                                   AM_HAL_CTIMER_PIN_ENABLE);
	//parameter: which interrput is to be used
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA3);
		//TimerNumber, TimerSegment, Period, OnTime
    am_hal_ctimer_period_set(3, AM_HAL_CTIMER_TIMERA, 10, 5);

    //
    // Enable the timer A3 to trigger the ADC directly
    //
    am_hal_ctimer_adc_trigger_enable();

    //
    // Start the timer (TimerNumber, TimerSegment)
    //
    am_hal_ctimer_start(3, AM_HAL_CTIMER_TIMERA);
}

//*****************************************************************************
//
// Interrupt handler for the ADC.
//
//*****************************************************************************
void //TODO: ZirkularBuffer evt. komplett entfernen und nurnoch Output Buffer brauchen
am_adc_isr(void)
{
		// Variabeln auf 32 Bit festsetzen, Status ist interrupt status
    uint32_t ui32Status, ui32FifoData;

    //
    // Read the interrupt status.
    //
    ui32Status = am_hal_adc_int_status_get(true);

    //
    // Clear the ADC interrupt.
    //
    am_hal_adc_int_clear(ui32Status);

    //
    // If we got a FIFO 75% full (which should be our only
    // ADC interrupt), go ahead and read the data.
    //
    if (ui32Status & AM_HAL_ADC_INT_FIFOOVR1)
    {
      do
      {
        //
        // Read the value from the FIFO into the circular buffer.
        // and print it
        ui32FifoData = am_hal_adc_fifo_pop();
				am_util_stdio_printf("%d \n", ui32FifoData);
				daten[datenpushnr][g_ui32ADCSampleIndex] = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32FifoData);
        //g_ui32ADCSampleBuffer[g_ui32ADCSampleIndex] = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32FifoData); => für Zirkularbuffer, nicht mehr benötigt
        g_ui32ADCSampleIndex = (g_ui32ADCSampleIndex + 1) & ADC_SAMPLE_INDEX_M;
      } while (AM_HAL_ADC_FIFO_COUNT(ui32FifoData) > 0);

    }

}

//*****************************************************************************
//
// Main function.
// ADC_SAMPLE_BUF_SIZE => windowsize
//*****************************************************************************
int
main(uint32_t ADC_SAMPLE_BUF_SIZE, int* daten[3][ADC_SAMPLE_BUF_SIZE])
{
    // GENERAL CONFIGURATIONS	
    // Set the system clock to maximum frequency, and set the default low-power
    // settings for this board.
    //
    am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);

    am_hal_vcomp_disable();

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_enable(&am_hal_cachectrl_defaults);

    //
    // Enable only the first 512KB bank of Flash (0).  Disable Flash(1)
    //
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_FLASH512K);
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM16K);

    // CLOCK AND TIMER CONFIGURATIONS
    // Allow the XTAL (oscillator) to turn off.
    //
    am_hal_clkgen_osc_stop(AM_HAL_CLKGEN_OSC_XT);

    //
    // Turn off the voltage comparator
    //
    am_hal_vcomp_disable();

    //
    // Start the ITM interface.
    //
    itm_start();

    //
    // Start the CTIMER A3 (generell purpose timer) for timer-based ADC measurements.
    //
    init_timerA3_for_ADC();

    // INTERRUPTS
    // Enable interrupts.
    //
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_ADC);
    am_hal_interrupt_master_enable();

    // INPUT CONFIGURATIONS
    // Set a pin to act as our ADC input
    //
    am_hal_gpio_pin_config(16, AM_HAL_PIN_16_ADCSE0);

    // CONFIGURE ADC
    // Configure the ADC
    //
    adc_config();

    //
    // Trigger the ADC sampling for the first time manually. (with the software)
    //
    am_hal_adc_trigger();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("ADC Example with 1.2Msps and LPMODE=0\n");

    //
    // Allow time for all printing to finish.
    //
    am_util_delay_ms(10);

    //
    // We are done printing. Disable debug printf messages on ITM.
    //
    //am_bsp_debug_printf_disable();

    //
    // Loop forever.
    //
    while(1)
    {
      //
      // Go to Deep Sleep.
      //
      am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    }
}
