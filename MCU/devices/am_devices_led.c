//*****************************************************************************
//
//! @file am_devices_led.c
//!
//! @brief Functions for controlling an array of LEDs
//!
//! @addtogroup devices External Device Control Library
//! @addtogroup LED SPI Device Control for programmable LEDs.
//! @ingroup devices
//! @{
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

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_devices_led.h"

//*****************************************************************************
//
//! @brief Configures the necessary pins for an array of LEDs
//!
//! @param psLED is a pointer to an LED structure.
//!
//! This function configures a GPIO to drive an LED in a low-power way.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_init(am_devices_led_t *psLED)
{
    //
    // Handle Direct Drive Versus 3-State (with pull-up or no buffer).
    //
    if ( AM_DEVICES_LED_POL_DIRECT_DRIVE_M & psLED->ui32Polarity )
    {
        //
        // Configure the pin as a push-pull GPIO output.
        //
        am_hal_gpio_pin_config(psLED->ui32GPIONumber, AM_HAL_GPIO_OUTPUT);

        //
        // Enable the output driver, and set the output value to the LEDs "ON"
        // state.
        //
        am_hal_gpio_out_enable_bit_set(psLED->ui32GPIONumber);
        am_hal_gpio_out_bit_replace(psLED->ui32GPIONumber,
                                    psLED->ui32Polarity &
                                    AM_DEVICES_LED_POL_POLARITY_M);
    }
    else
    {
        //
        // Configure the pin as a tri-state GPIO.
        //
        am_hal_gpio_pin_config(psLED->ui32GPIONumber, AM_HAL_GPIO_3STATE);

        //
        // Disable the output driver, and set the output value to the LEDs "ON"
        // state.
        //
        am_hal_gpio_out_enable_bit_clear(psLED->ui32GPIONumber);
        am_hal_gpio_out_bit_replace(psLED->ui32GPIONumber,
                                    psLED->ui32Polarity &
                                    AM_DEVICES_LED_POL_POLARITY_M );
    }
}

//*****************************************************************************
//
//! @brief Disables an array of LEDs
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32NumLEDs is the total number of LEDs in the array.
//!
//! This function disables the GPIOs for an array of LEDs.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_array_disable(am_devices_led_t *psLEDs, uint32_t ui32NumLEDs)
{
    //
    // Loop through the list of LEDs, configuring each one individually.
    //
    for ( uint32_t i = 0; i < ui32NumLEDs; i++ )
    {
        am_hal_gpio_pin_config((psLEDs + i)->ui32GPIONumber, AM_HAL_GPIO_DISABLE);
    }
}

//*****************************************************************************
//
//! @brief Configures the necessary pins for an array of LEDs
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32NumLEDs is the total number of LEDs in the array.
//!
//! This function configures the GPIOs for an array of LEDs.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_array_init(am_devices_led_t *psLEDs, uint32_t ui32NumLEDs)
{
    uint32_t i;

    //
    // Loop through the list of LEDs, configuring each one individually.
    //
    for ( i = 0; i < ui32NumLEDs; i++ )
    {
        am_devices_led_init(psLEDs + i);
    }
}

//*****************************************************************************
//
//! @brief Turns on the requested LED.
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32LEDNum is the LED number for the light to turn on.
//!
//! This function turns on a single LED.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_on(am_devices_led_t *psLEDs, uint32_t ui32LEDNum)
{
    //
    // Handle Direct Drive Versus 3-State (with pull-up or no buffer).
    //
    if ( AM_DEVICES_LED_POL_DIRECT_DRIVE_M & psLEDs[ui32LEDNum].ui32Polarity )
    {
        //
        // Set the output to the correct state for the LED.
        //
        am_hal_gpio_out_bit_replace(psLEDs[ui32LEDNum].ui32GPIONumber,
                                    psLEDs[ui32LEDNum].ui32Polarity &
                                    AM_DEVICES_LED_POL_POLARITY_M );
    }
    else
    {
        //
        // Turn on the output driver for the LED.
        //
        am_hal_gpio_out_enable_bit_set(psLEDs[ui32LEDNum].ui32GPIONumber);
    }
}

//*****************************************************************************
//
//! @brief Turns off the requested LED.
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32LEDNum is the LED number for the light to turn off.
//!
//! This function turns off a single LED.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_off(am_devices_led_t *psLEDs, uint32_t ui32LEDNum)
{
    //
    // Handle Direct Drive Versus 3-State (with pull-up or no buffer).
    //
    if ( AM_DEVICES_LED_POL_DIRECT_DRIVE_M & psLEDs[ui32LEDNum].ui32Polarity )
    {
        //
        // Set the output to the correct state for the LED.
        //
        am_hal_gpio_out_bit_replace(psLEDs[ui32LEDNum].ui32GPIONumber,
                                    !(psLEDs[ui32LEDNum].ui32Polarity &
                                      AM_DEVICES_LED_POL_POLARITY_M) );
    }
    else
    {
        //
        // Turn off the output driver for the LED.
        //
        am_hal_gpio_out_enable_bit_clear(psLEDs[ui32LEDNum].ui32GPIONumber);
    }
}

//*****************************************************************************
//
//! @brief Toggles the requested LED.
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32LEDNum is the LED number for the light to toggle.
//!
//! This function toggles a single LED.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_led_toggle(am_devices_led_t *psLEDs, uint32_t ui32LEDNum)
{
    //
    // Handle Direct Drive Versus 3-State (with pull-up or no buffer).
    //
    if ( AM_DEVICES_LED_POL_DIRECT_DRIVE_M & psLEDs[ui32LEDNum].ui32Polarity )
    {
        am_hal_gpio_out_bit_toggle(psLEDs[ui32LEDNum].ui32GPIONumber);
    }
    else
    {
        //
        // Check to see if the LED pin is enabled.
        //
        if ( am_hal_gpio_out_enable_bit_get(psLEDs[ui32LEDNum].ui32GPIONumber) )
        {
            //
            // If it was enabled, turn if off.
            //
            am_hal_gpio_out_enable_bit_clear(psLEDs[ui32LEDNum].ui32GPIONumber);
        }
        else
        {
            //
            // If it was not enabled, turn if on.
            //
            am_hal_gpio_out_enable_bit_set(psLEDs[ui32LEDNum].ui32GPIONumber);
        }
    }
}

//*****************************************************************************
//
//! @brief Gets the state of the requested LED.
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32LEDNum is the LED to check.
//!
//! This function checks the state of a single LED.
//!
//! @return true if the LED is on.
//
//*****************************************************************************
bool
am_devices_led_get(am_devices_led_t *psLEDs, uint32_t ui32LEDNum)
{
    //
    // Handle Direct Drive Versus 3-State (with pull-up or no buffer).
    //
    if ( AM_DEVICES_LED_POL_DIRECT_DRIVE_M & psLEDs[ui32LEDNum].ui32Polarity )
    {
        //
        // Mask to the GPIO bit position for this GPIO number.
        //
        uint64_t ui64Mask = 0x01l << psLEDs[ui32LEDNum].ui32GPIONumber;

        //
        // Extract the state of this bit and return it.
        //
        return !!(am_hal_gpio_input_read() & ui64Mask);
    }
    else
    {
        return am_hal_gpio_out_enable_bit_get(
                    psLEDs[ui32LEDNum].ui32GPIONumber);
    }
}

//*****************************************************************************
//
//! @brief Display a binary value using LEDs.
//!
//! @param psLEDs is an array of LED structures.
//! @param ui32NumLEDs is the number of LEDs in the array.
//! @param ui32Value is the value to display on the LEDs.
//!
//! This function displays a value in binary across an array of LEDs.
//!
//! @return true if the LED is on.
//
//*****************************************************************************
void
am_devices_led_array_out(am_devices_led_t *psLEDs, uint32_t ui32NumLEDs,
                         uint32_t ui32Value)
{
    uint32_t i;

    for ( i = 0; i < ui32NumLEDs; i++ )
    {
        if ( ui32Value & (1 << i) )
        {
            am_devices_led_on(psLEDs, i);
        }
        else
        {
            am_devices_led_off(psLEDs, i);
        }
    }
}
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
