#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//Define circular buffer to hold ADC samples
#define ADC_SAMPLE_BUF_SIZE 1024
#define ADC_SAMPLE_INDEX_M 0x3FF
uint32_t g_ui32ADCSampleBuffer[ADC_SAMPLE_BUF_SIZE];
uint32_t g_ui32ADCSampleIndex = 0;




//ITM Interface start
void itm_start (void)
{
	//Initialize the printf interface for ITM/SWO output
	am_util_stdio_printf_init((am_util_stdio_print_char_t) am_bsp_itm_string_print);
	//Initialize the SWO GPIO pin
	am_bsp_pin_enable(ITM_SWO);
	am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
	//Enable ITM
	am_hal_itm_enable();
	//Enable debug printf messages using ITM on SWO pin
	am_bsp_debug_printf_enable();
}

//Configure ADC
void adc_config(coid)
{
	//Variabeln deklaration
	am_hal_adc_config_t sADCConfig;
	//Enable ADC power domain
	am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_ADC);
	
	//ADC CONFIGURATIONS
	sADCConfig.ui32Clock = AM_HAL_ADC_CLOCK_HFRC;
	sADCConfig.ui32TriggerConfig = AM_HAL_ADC_TRIGGER_SOFT;
	sADCConfig.ui32Reference = AM_HAL_ADC_REF_INT_2P0;
	sADCConfig.ui32ClockMode = AM_HAL_ADC_CK_LOW_POWER;
	sADCConfig.ui32PowerMode = AM_HAL_ADC_LPMODE_0;
	sADCConfig.ui32Repeat = AM_HAL_ADC_REPEAT;
	am_hal_adc_config(&sADCConfig);
	
	//when to wake up
	//in example slow input, wake uup every conversion
	am_hal_adc_int_enable(AM_HAL_ADC_INT_FIFOOVR1);
	
	//Set up ADC slot
	am_hal_adc_slot_config(0, AM_HAL_ADC_SLOT_AVG_128 | AM_HAL_ADC_SLOT_14BIT | AM_HAL_ADC_SLOT_CHSEL_SE0 | AM_HAL_ADC_SLOT_ENABLE);
	//enable ADC
	am_hal_adc_enable();
}

//TimerNumber of ADC needs to be 3, TimerSegment?
void init_timerA3_for_ADC(void)
{
	//Start timer to trigger ADC periodically (1 second at the moment)
	//(TimerNumber, TimerSegment, CifigVal) => TODO
	am_hal_ctimer_config_single(3, AM_HAL_CTIMER_TIMERA, AM_HAL_CTIMER_HFRC_12MHZ | AM_HAL_CTIMER_FN_REPEAT | AM_HAL_CTIMER_INT_ENABLE | AM_HAL_CTIMER_PIN_ENABLE);
	//(Interrupt) =>TODO
	am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA3);
	//TimerNumber, TimerSegment, Period, OnTime) => TODO
	am_hal_ctimer_period_set(3, AM_HAL_CTIMER_TIMERA, 10, 5);
	
	//Enable the timer A3 to trigger the ADC directly
	am_hal_ctimer_adc_trigger_enable();
	//Start timer (TimerNumber, TimerSegment) => TODO
	am_hal_ctimer_start(3, AM_HAL_CTIMER_TIMERA);

}

void am_adc_isr(void)
{
	//Variabeln auf 32 Bit festsetzen, Status ist interrupt status
	uint32_t ui32Status, ui32FifoData;
	
	//Read interrupt status
	ui32Status = am_hal_adc_int_status_get(true);
	//Clear the ADC interrupt
	am_hal_adc_int_clear(ui32Status);
	
	//FIFO 75% full (only possible interrupt) => read the data normally
	if (ui32Status & AM_HAL_ADC_INT_FIFOOVR1)
	{
		do
		{
			//read value of FIFO into circular buffer
			ui32FifoData = am_hal_adc_fifo_pop();
			//am_util_stdio_printf("Interrupt: %u\n", ui32FifoData);
			g_ui32ADCSampleBuffer[g_ui32ADCSampleIndex] = AM_HAL_ADC_FIFO_FULL_SAMPLE(ui32FifoData);
			g_ui32ADCSampleIndex = (g_ui32ADCSampleIndex + 1) & ADC_SAMPLE_INDEX_M;
		} while (AM_HAL_ADC_FIFO_COUNT(ui32FifoData>0));
	}
	
}

int main() 
{
	//GENERAL CONFIGURATIONS
	//system clock set to maximum frequency with function (already programmed, sets also default low-power settings for board)
	am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);
	am_hal_vcomp_disable();
	//set default cache configurations
	am_hal_cachectrl_enable(&am_hal_cachectrl_defaults);
	//enable first 512KB bank of Flash (0)
	am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_FLASH512K);
	//disable flash(0)
	am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM16K);

	//CLOCK AND TIMER CONFIGURATIONS
	//XTAL oscillator turn off
	am_hal_clkgen_osc_stop(AM_HAL_CLKGEN_OSC_XT);
	//turn off voltage comparator
	am_hal_vcomp_disable();
	//ITM (instrumentation trace macrocell) start interface
	itm_start();
	//CTIMER A3 (generell purpose timer) start for timer-based ADC measurements
	init_timerA3_for_ADC();
	
	
	
	//INTERRUPTS
	//Enable interrups
	am_hal_interrupt_enable(AM_HAL_INTERRUPT_ADC);
	am_hal_interrupt_master_enable();
	
	//ADC CONFIGURATIONS
	//Set pin as ADC input
	 am_hal_gpio_pin_config(16, AM_HAL_PIN_16_ADCSE0);
	//Configure ADC
	adc_config();
	//trigger the ADC sampling for the first time manually (software)
	am_hal_adc_trigger();
	
	//print what happens => not needed
	am_util_stdio_terminal_clear();
	//Output buffer
	
	//zwischenspeicher von output
	uint32_t* fifo = (uint32_t*)(0x50010038);
	//Output fifo
	am_util_stdio_printf("%u\n", (*fifo) & 0xFFFFF);
	am_util_stdio_printf("ADC with 1.2Msps");
	am_util_delay_ms(10); //time-delay because of printing message
	//done printing-> disable debug printf messages on ITM
	am_bsp_debug_printf_disable();
	
	//FOREVER LOOP
	while(1)
	{
		//go to deep sleep
		am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
	}
}
