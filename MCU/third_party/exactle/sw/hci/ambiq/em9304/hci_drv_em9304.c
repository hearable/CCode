//*****************************************************************************
//
//! @file hci_drv.c
//!
//! @brief HCI driver interface.
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

#include "wsf_types.h"
#include "wsf_msg.h"
#include "wsf_cs.h"
#include "hci_defs.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_tr_apollo.h"
#include "hci_core.h"

#include "am_mcu_apollo.h"
#include "am_util.h"
#include "am_devices_em9304.h"
#include "hci_drv_em9304.h"
#include "em9304_patches.h"
#include "em9304_init.h"
#include "hci_apollo_config.h"

#include <string.h>

//*****************************************************************************
//
// Unless the config file overwrites this option, the HCI driver will use a
// direct call to the HAL when it needs to sleep.
//
//*****************************************************************************
#ifndef HCI_DRV_SLEEP
#define HCI_DRV_SLEEP                       am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP)
#endif

//*****************************************************************************
//
// If the config file doesn't say anything about MAC addresses, use a EM Microelectronic
// manufacturer ID combined with the Apollo CHIPID1 register.
//
//*****************************************************************************
#ifndef HCI_APOLLO_MAC
#define HCI_APOLLO_MAC                  {0x01, 0x00, 0x00, 0xEE, 0xF3, 0x0C}
#endif

#ifndef HCI_APOLLO_USE_CHIPID_FOR_MAC
#define HCI_APOLLO_USE_CHIPID_FOR_MAC   true
#endif

uint8_t radio_boot_complete = 0;

//*****************************************************************************
//
// Mac address for the EM.
//
//*****************************************************************************
static uint8_t g_pui8BLEMacAddress[6] = HCI_APOLLO_MAC;

//*****************************************************************************
//
// HCI RX packet buffer for EM9304 Driver.
//
//*****************************************************************************
static uint32_t g_pui32HCIRXBuffer[64];

//*****************************************************************************
//
// Static record of the EM9304 vendor specific events
//
//*****************************************************************************
g_EMVSEvent_t g_EMVendorSpecificEvents = {0,0,0,0,0,0,0};

//*****************************************************************************
//
// Workaround for Keil memcpy()
//
// Keil's version of memcpy() contains an optimization that allows it to copy
// data more efficiently when both the source and destination pointers are well
// aligned. Unforunately, some of exactLE's complex callback structures confuse
// Keil's memcpy implementation. Left unchecked, this can lead to intermittent
// hard-faults.
//
// This function definition will intercept calls to this optimized version of
// memcpy and avoid the problem when the pointers are unexpectedly unaligned.
//
//*****************************************************************************
#if defined(__ARMCC_VERSION)

void $Super$$__aeabi_memcpy4(void *dest, const void *src, size_t n);

void
$Sub$$__aeabi_memcpy4(void *dest, const void *src, size_t n)
{
    //
    // If the pointers are aligned, we can use Keil's normal memcpy.
    //
    if ((((uint32_t)dest % 4) == 0) && (((uint32_t)src % 4) == 0))
    {
        $Super$$__aeabi_memcpy4(dest, src, n);
        return;
    }

    //
    // Otherwise, make sure we use 8-bit pointers.
    //
    uint8_t *tempSrc = (uint8_t *)(src);
    uint8_t *tempDest = (uint8_t *)(dest);

    //
    // Copy from src to dest, one byte at a time.
    //
    for (uint32_t i = 0; i < n; i++)
    {
        *tempDest++ = *tempSrc++;
    }
}
#endif

//*****************************************************************************
//
//! @brief Get the EM9304 vendor specific event counters.
//!
//! @return Returns a pointer to the EM9304 vendor specific event counters.
//
//*****************************************************************************
g_EMVSEvent_t *getEM9304VSEventCounters(void)
{
    return &g_EMVendorSpecificEvents;
}
//*****************************************************************************
//
//! @brief Write data the driver.
//!
//! @param type HCI packet type
//! @param len Number of bytes to write
//! @param pData Byte array to write
//!
//! @return Returns the number of bytes written.
//
//*****************************************************************************
uint16_t
hciDrvWrite(uint8_t type, uint16_t len, uint8_t *pData)
{
    //
    // Turn on the IOM for this operation.
    //
    am_devices_em9304_spi_awake(g_sEm9304.ui32IOMModule);

	  //
	  // Write the HCI packet.
	  //
    am_devices_em9304_block_write(&g_sEm9304, type, pData, len );
	
    //
    // Disable IOM SPI pins and turn off the IOM after operation
    //
    am_devices_em9304_spi_sleep(g_sEm9304.ui32IOMModule);

    return len;
}

//*****************************************************************************
//
// hciDrvReadyToSleep - Stub provided to allow other layers to run correctly.
//
//*****************************************************************************
bool_t
hciDrvReadyToSleep(void)
{
    return TRUE;
}

void
HciDataReadyISR(void)
{
    uint32_t    ui32HCIPacketSize;

    //
    // If the radio boot has not yet completed, then do not process HCI packets
  	if (!radio_boot_complete)
    {
    return;
    }

  //
  // Turn on the IOM for this operation.
  //
  am_devices_em9304_spi_awake(g_sEm9304.ui32IOMModule);

  ui32HCIPacketSize = am_devices_em9304_block_read(&g_sEm9304, g_pui32HCIRXBuffer, 0);

  // Check for EM9304 Vendor Specific events and record them.
	if ( (ui32HCIPacketSize > 3) && (0x0001FF04 == (g_pui32HCIRXBuffer[0] & 0x00FFFFFF)) )
	{
		switch((g_pui32HCIRXBuffer[0] & 0xFF000000) >> 24)
		{
			case 0x01:
				g_EMVendorSpecificEvents.EM_ActiveStateEntered++;
				am_util_debug_printf("Received EM_ActiveStateEntered Event\n");					
				break;
			case 0x03:
				g_EMVendorSpecificEvents.EM_TestModeEntered++;
				am_util_debug_printf("Received EM_TestModeEntered Event\n");					
				break;
			case 0x04:
				g_EMVendorSpecificEvents.EM_HalNotification++;
				am_util_debug_printf("Received EM_HalNotification Event\n");		
				break;
			default:
				am_util_debug_printf("Received Unknown Vendor Specific Event from EM9304\n");
				break;
		}
		
		//
		// Reset the packet size to 0 so that this packet will not be processed by the host stack.
		//
		ui32HCIPacketSize = 0;
	}
		
	if (ui32HCIPacketSize > 0)
  	{    
	  hciTrSerialRxIncoming((uint8_t *)g_pui32HCIRXBuffer, ui32HCIPacketSize);
  	}

  //
  // Disable IOM SPI pins and turn off the IOM after operation
  //
  am_devices_em9304_spi_sleep(g_sEm9304.ui32IOMModule);

}

//*****************************************************************************
//
// Configure the necessary pins and start the EM9304 radio.
//
//*****************************************************************************
void
HciDrvRadioBoot(uint32_t ui32UartModule)
{
    // disable interrupt during EM9304 initialization.
    am_devices_em9304_disable_interrupt();

    radio_boot_complete = 0;

    //
    // Enable the radio pins.
    //
#ifdef HCI_APOLLO_POWER_PIN

    //
    // Insert a power on reset to TLSR8269
    // (with TLSR8269 EVK, this is only done via power pin)
    //
    am_hal_gpio_pin_config(HCI_APOLLO_POWER_PIN, HCI_APOLLO_POWER_CFG);
    am_hal_gpio_out_bit_clear(HCI_APOLLO_POWER_PIN);
    am_util_delay_ms(100);
    am_hal_gpio_out_bit_set(HCI_APOLLO_POWER_PIN);
    am_util_delay_ms(100);
#endif

    //
    // Assert RESET to the EM9304 device.
    //
    am_hal_gpio_pin_config(HCI_APOLLO_RESET_PIN, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(HCI_APOLLO_RESET_PIN);

    //
    // Setup SPI interface for EM9304
    //
    am_devices_em9304_config_pins();
    am_devices_em9304_spi_init(g_sEm9304.ui32IOMModule, &g_sEm9304IOMConfigSPI);
  
    //
    // Enable the IOM and GPIO interrupt handlers.
    //
    am_hal_gpio_out_bit_set(HCI_APOLLO_RESET_PIN);

    //
    // Delay for 20ms to make sure the em device gets ready for commands.
    //
    am_util_delay_ms(20);

    //
    // Initialize the EM9304.
    //
    initEM9304();

    // delay here to make sure EM9304 is ready for operation after
    // patch is loaded.
    am_util_delay_ms(20);
    
    //
    // Set the MAC address. Use the CHIPID1 register for additional
    // randomization if the config file requested it.
    //
    if (HCI_APOLLO_USE_CHIPID_FOR_MAC)
    {
        g_pui8BLEMacAddress[0] = AM_REG(MCUCTRL, CHIPID1);
        g_pui8BLEMacAddress[1] = AM_REG(MCUCTRL, CHIPID1) >> 8;
        g_pui8BLEMacAddress[2] = AM_REG(MCUCTRL, CHIPID1) >> 16;
    }

    am_devices_em9304_mac_set(g_pui8BLEMacAddress);

    // Initialization of the EM9304 is complete.
    radio_boot_complete = 1;
    am_util_debug_printf("HciDrvRadioBoot complete\n");

    // enable interrupt after EM9304 initialization is done.
    am_devices_em9304_enable_interrupt();
}

void
HciDrvRadioShutdown(void)
{  
  radio_boot_complete = 0;
  am_hal_gpio_pin_config(HCI_APOLLO_RESET_PIN, AM_HAL_PIN_OUTPUT);
  am_hal_gpio_out_bit_clear(HCI_APOLLO_RESET_PIN);
}

/*************************************************************************************************/
/*!
 *  \fn     HciVsSetRfPowerLevelEx
 *
 *  \brief  Vendor-specific command for settting Radio transmit power level
 *          for EM9304.
 *
 *  \param  txPowerlevel    valid range from 0 to 17 in decimal.
 *
 *  \return true when success, otherwise false
 */
/*************************************************************************************************/
bool_t HciVsEM_SetRfPowerLevelEx(txPowerLevel_t txPowerlevel)
{
  // make sure it's 8 bit
  uint8_t tx_power_level = (uint8_t)txPowerlevel;

  if(tx_power_level < TX_POWER_LEVEL_INVALID) {
    HciVendorSpecificCmd(0xFC26, sizeof(tx_power_level), &tx_power_level);
    return true;
  }
  else {
    return false;
  }

}

/*************************************************************************************************/
/*!
 *  \fn     HciVsEM_TransmitterTest
 *
 *  \brief  Vendor-specific command for start transmitter testing
 *
 *  \param  test_mode       refer to em9304 datasheet
 *  \param  channel_number  refer to em9304 datasheet
 *  \param  packet_len      refer to em9304 datasheet
 *  \param  packet_payload_type    refer to em9304 datasheet
 *
 *  \return None
 */
/*************************************************************************************************/
void HciVsEM_TransmitterTest(uint8_t test_mode, uint8_t channel_number, uint8_t packet_len, uint8_t packet_payload_type)
{
    uint8_t params[4] = {
      test_mode,
      channel_number,
      packet_len,
      packet_payload_type
    };

    HciVendorSpecificCmd(0xFC11, sizeof(params), &params[0]);

}


/*************************************************************************************************/
/*!
 *  \fn     HciVsEM_TransmitterTestEnd
 *
 *  \brief  Vendor-specific command for ending Radio transmitter testing.
 *
 *  \param  None    
 *
 *  \return None
 */
/*************************************************************************************************/
void HciVsEM_TransmitterTestEnd(void)
{
    HciVendorSpecificCmd(0xFC12, 0, NULL);
}

