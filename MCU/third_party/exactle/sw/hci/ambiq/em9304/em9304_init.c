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
#include "em9304_init.h"
#include "em9304_patches.h"
#include "hci_apollo_config.h"

#include <string.h>

#define INVALIDATE_UNKNOWN_PATCHES

// EM_PatchQuery field offsets
#define PATCH_INDEX_OFFSET    3

// EM_PatchQuery response field offsets
#define CONTAINER_COUNT_INDEX 7
#define CONTAINER_ADDR_INDEX 15
#define CONTAINER_SIZE_INDEX 19
#define BUILD_NUMBER_INDEX 27
#define USER_BUILD_NUMBER_INDEX 29
#define CONTAINER_VERSION_INDEX 32
#define CONTAINER_TYPE_INDEX 33
#define CONTAINER_ID_INDEX 34

// EM_PatchQuery response values
#define CONTAINER_TYPE_CONFIG_DATA_WORD     1
#define CONTAINER_TYPE_RANDOM_DATA_WORD     2
#define CONTAINER_TYPE_RANDOM_DATA_BYTE     3
#define CONTAINER_TYPE_CONFIG_DATA_BYTE     11

// EM_PatchWrite and EM_PatchContine field offsets
#define PATCH_LENGTH_OFFSET 2

// EM_PatchWrite and EM_PatchContinue response field offsets
#define HCI_STATUS_OFFSET 6
#define EM_PATCH_STATUS_OFFSET 7

// EM_PatchWrite and EM_PatchContinue Patch Status values
#define EM_PATCH_APPLIED 1
#define EM_PATCH_CONTINUE 2

//*****************************************************************************
//
// HCI Commands for EM9304
//
//*****************************************************************************
uint8_t g_pui8EM_SleepDisable[] = {0x2D, 0xFC, 0x01, 0x00};
uint8_t g_pui8EM_SetOTPOn[]         = {0x2B, 0xFC, 0x01, 0x01};
uint8_t g_pui8EM_PatchQuery[]     = {0x34, 0xFC, 0x02, 0x00, 0x00};
uint8_t g_pui8EM_SleepEnable[]     = {0x2D, 0xFC, 0x01, 0x01};
uint8_t g_pui8EM_CpuReset[]         = {0x32, 0xFC, 0x00};

//*****************************************************************************
//
// HCI RX packet buffer for EM9304 Driver.
//
//*****************************************************************************
static uint32_t g_pui32HCIRXBuffer[64];

//*****************************************************************************
//
// Static record of the EM9304 patch errors
//
//*****************************************************************************

uint32_t g_EMPatchErrors = 0;

//*****************************************************************************
//
//! @brief Patch Response helper functions for the EM9304 patches.  This
//!        routine blocks on a response from the EM9304 and filters the 
//!        vendor specific events.
//!
//! @return none.
//
//*****************************************************************************
void
waitEM9304Response(void)
{
  uint32_t numBytesRx;
  
  do
  {
    numBytesRx = am_devices_em9304_block_read(&g_sEm9304, g_pui32HCIRXBuffer,0);
  } while ((numBytesRx == 0) || ((numBytesRx == 4) && (0x0000FF04 == (g_pui32HCIRXBuffer[0] & 0x0000FFFF))));				
}

//*****************************************************************************
//
//! @brief Patch Response helper functions for the EM9304 patches.  This
//!        routine blocks on a response from the EM9304 and filters the 
//!        vendor specific events.
//!
//! @return none.
//
//*****************************************************************************
bool
validEM9304Patches(void)
{
  //
  // Check to see if we have valid patches.
  // NULL patch has a specific signature.
  // 
  if ((1 == EM9304_PATCHES_NUM_PATCHES) &&
      (0xFFFF == g_pEm9304Patches[0].buildNumber) &&
        (0xFFFF == g_pEm9304Patches[0].userBuildNumber) &&
          (0xFF == g_pEm9304Patches[0].containerVersion) &&
            (0xFF == g_pEm9304Patches[0].containerType) &&
              (0xFF == g_pEm9304Patches[0].containerID) &&
                (0x00 == g_pEm9304Patches[0].applyPatch) &&
                  (0x00 == g_pEm9304Patches[0].startingPatch) &&
		    (0x00 == g_pEm9304Patches[0].endingPatch))
  {
    return false;
  }
  else
  {
    return true;
  }
}

#ifdef INVALIDATE_UNKNOWN_PATCHES
static bool invalidateEM9304Patch(uint32_t addr, uint32_t size)
{
  uint8_t   *bytePtr = (uint8_t *)&g_pui32HCIRXBuffer;
  
  uint8_t payload[] = { 0x22, 0xFC, //WriteAtAddr command
  0x0C, //HCI param length
  0, 0, 0, 0, // container address placeholder
  0x33, 0x39, 0x6D, 0x65, //signature
  0, 0, 0, 0 }; //size placeholder
  
  
  am_util_debug_printf("Invalidating patch at %x\n", addr);
  
  payload[3] = (uint8_t)(addr & 0xFF);
  payload[4] = (uint8_t)((addr & 0xFF00) >> 8);
  payload[5] = (uint8_t)((addr & 0xFF0000) >> 16);
  payload[6] = (uint8_t)((addr & 0xFF000000) >> 24);
  
  size |= 0x36000000;
  
  payload[11] = (uint8_t)(size & 0xFF);
  payload[12] = (uint8_t)((size & 0xFF00) >> 8);
  payload[13] = (uint8_t)((size & 0xFF0000) >> 16);
  payload[14] = (uint8_t)((size & 0xFF000000) >> 24);
  
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, payload, sizeof(payload));
  
  waitEM9304Response();
  
  return (bytePtr[HCI_STATUS_OFFSET] == 0);
}
#endif

//*****************************************************************************
//
//! @brief Query the EM9304 patches.  This routine uses the EM_PatchQuery HCI
//!        command to interogate the connected EM9304 about its current patch
//!        state and then update the patch Container Info data structure.
//!
//! @return none.
//
//*****************************************************************************
void
queryEM9304Patches(void)
{
  uint32_t	containerCount;
  uint32_t	buildNumber, userBuildNumber, containerVersion, containerType, containerID;
#ifdef INVALIDATE_UNKNOWN_PATCHES
  uint32_t  containerAddr, containerSize;
  bool      invalidatePatch = false;
#endif
  uint8_t	*pBuf = (uint8_t *)g_pui32HCIRXBuffer;
  
  // Initialize the container info patch status
  for (uint32_t patch = 0; patch < EM9304_PATCHES_NUM_PATCHES; patch++)
  {
    g_pEm9304Patches[patch].applyPatch = true;
  }
  
  // Send the EM_SetSleepOptions command to disable sleep and check the response.
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_SleepDisable, sizeof(g_pui8EM_SleepDisable) );
  
  waitEM9304Response();
  
  // Check that the response is to the Sleep Disable.
  if ((0x01040E04 != g_pui32HCIRXBuffer[0]) || (0x0000FC2D != (g_pui32HCIRXBuffer[1] & 0x0000FFFF)))
  {
    am_util_debug_printf("Invalid Response to EM9304 Sleep Disable\n");
    return;
  }
  
  // Send the EM_SetMemoryMode command to turn on OTP and check the response.
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_SetOTPOn, sizeof(g_pui8EM_SetOTPOn) );
  
  waitEM9304Response();
  
  // Check that the response is to the Sleep Disable.
  if ((0x01040E04 != g_pui32HCIRXBuffer[0]) || (0x0000FC2B != (g_pui32HCIRXBuffer[1] & 0x0000FFFF)))
  {
    am_util_debug_printf("Invalid Response to EM9304 OTP Enable\n");
    return;
  }
  
  // Query the EM9304 with the EM_PatchQuery and Patch Index = 0.  This will return the Container Count.
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_PatchQuery, sizeof(g_pui8EM_PatchQuery) );
  
  waitEM9304Response();
  
  // Check that the response is to the Patch Query.
  if ((0x01200E04 != g_pui32HCIRXBuffer[0]) || (0x0000FC34 != (g_pui32HCIRXBuffer[1] & 0x0000FFFF)))
  {
    am_util_debug_printf("Invalid Response to EM9304 Patch Query\n");
    return;
  }
  
  // Extract the container information from the query response.
  containerCount = (uint32_t)pBuf[CONTAINER_COUNT_INDEX] + 
    ((uint32_t)pBuf[CONTAINER_COUNT_INDEX + 1] << 8);
  
  // Assume the first patch is the manufacturing trim patch.
  // This is the only patch that never should be invalidated.
  am_util_debug_printf("Number of patch containers on EM9304: %d\n",containerCount);
  
  // For each container in Container Count
  for (uint32_t container = 1; container < containerCount; container++)
  {
    // Send the EM_PatchQuery for the Container.
    g_pui8EM_PatchQuery[PATCH_INDEX_OFFSET] = container;
    am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_PatchQuery, sizeof(g_pui8EM_PatchQuery) );
    
    waitEM9304Response();
    
    // Extract the container information from the query response.
    containerCount = (uint32_t)pBuf[CONTAINER_COUNT_INDEX] + 
      ((uint32_t)pBuf[CONTAINER_COUNT_INDEX + 1] << 8);
    buildNumber = (uint32_t)pBuf[BUILD_NUMBER_INDEX] + 
      ((uint32_t)(pBuf[BUILD_NUMBER_INDEX+1] << 8));
    userBuildNumber = (uint32_t)pBuf[USER_BUILD_NUMBER_INDEX] + 
      ((uint32_t)(pBuf[USER_BUILD_NUMBER_INDEX+1] << 8));
    containerVersion = pBuf[CONTAINER_VERSION_INDEX];
    containerType = pBuf[CONTAINER_TYPE_INDEX];
    containerID = pBuf[CONTAINER_ID_INDEX];
#ifdef INVALIDATE_UNKNOWN_PATCHES
    containerAddr = (uint32_t)((pBuf[CONTAINER_ADDR_INDEX+3] << 24) +
                               (pBuf[CONTAINER_ADDR_INDEX+2] << 16) +
                                 (pBuf[CONTAINER_ADDR_INDEX+1] << 8) +
                                   pBuf[CONTAINER_ADDR_INDEX]);
    containerSize = (uint32_t)((pBuf[CONTAINER_SIZE_INDEX+3] << 24) +
                               (pBuf[CONTAINER_SIZE_INDEX+2] << 16) +
                                 (pBuf[CONTAINER_SIZE_INDEX+1] << 8) +
                                   pBuf[CONTAINER_SIZE_INDEX]);
    // Check for patches that are likely not configuration managed by the customer.
    // Avoid invalidating these patches.
    if ( ((CONTAINER_TYPE_CONFIG_DATA_WORD == containerType) || 
          (CONTAINER_TYPE_RANDOM_DATA_WORD == containerType)  ||
            (CONTAINER_TYPE_CONFIG_DATA_BYTE == containerType) || 
              (CONTAINER_TYPE_RANDOM_DATA_BYTE == containerType)) &&
        ((0 == buildNumber) || (3089 == buildNumber)) &&
          (0 == userBuildNumber) )
    {
      invalidatePatch = false;
    }
    else
    {
      // Initialize the invalidate flag.
      invalidatePatch = true;
    }
#endif	
    
    // For each local patch, compare the Container Version, Container Type, and Container ID to the container info.
    for (uint32_t patch = 0; patch < EM9304_PATCHES_NUM_PATCHES; patch++)
    {
      if ((g_pEm9304Patches[patch].buildNumber == buildNumber) &&
          (g_pEm9304Patches[patch].userBuildNumber == userBuildNumber) &&
            (g_pEm9304Patches[patch].containerVersion == containerVersion) &&
              (g_pEm9304Patches[patch].containerType == containerType) &&
                (g_pEm9304Patches[patch].containerID == containerID))
      {
        g_pEm9304Patches[patch].applyPatch = false;		// Patch is already installed, so don't apply.
#ifdef INVALIDATE_UNKNOWN_PATCHES
        // Note that we will "re-enable" patches here even if they met the criteria above (which can happen!)
        invalidatePatch = false;
#endif	
        break;
      }
    }
    
#ifdef INVALIDATE_UNKNOWN_PATCHES		
    // Check to see if we need to invalidate the patch.
    if (invalidatePatch)
    {
      invalidateEM9304Patch(containerAddr, containerSize);
    }
#endif
    
  }
  
  // Send the EM_SetSleepOptions command to disable sleep and check the response.
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_SleepEnable, sizeof(g_pui8EM_SleepEnable) );
  
  waitEM9304Response();
  
  // Check that the response is to the Sleep Disable.
  if ((0x01040E04 != g_pui32HCIRXBuffer[0]) || (0x0000FC2D != (g_pui32HCIRXBuffer[1] & 0x0000FFFF)))
  {
    am_util_debug_printf("Invalid Response to EM9304 Sleep Enable\n");
    return;
  }
  
}

//*****************************************************************************
//
//! @brief Apply the EM9304 patches.  This routine uses the EM_PatchQuery HCI
//!        command to interogate the connected EM9304 about its current patch
//!        state and then update the patch Container Info data structure.
//!
//!
//! @return Returns the status of the patch application (< 0 is an error).
//
//*****************************************************************************
int32_t
applyEM9304Patches(void)
{
  uint8_t   *bytePtr = (uint8_t *)&g_pui32HCIRXBuffer;
  
  g_EMPatchErrors = 0;
  
  // Loop through the patches and apply those that are not already there.
  // For each local patch, compare the Container Version, Container Type, and Container ID to the container info.
  for (uint32_t patch = 0; patch < EM9304_PATCHES_NUM_PATCHES; patch++)
  {
    if (g_pEm9304Patches[patch].applyPatch)
    {
      for (uint32_t index = g_pEm9304Patches[patch].startingPatch; index < g_pEm9304Patches[patch].endingPatch; index ++)
      {
        am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, (uint8_t *)g_pEm9304PatchesHCICmd[index], 
                                      g_pEm9304PatchesHCICmd[index][PATCH_LENGTH_OFFSET]+3 );
	
        waitEM9304Response();
        
        if ((g_pEm9304PatchesHCICmd[index][1] == 0x27) &&
            (bytePtr[EM_PATCH_STATUS_OFFSET] != EM_PATCH_APPLIED))
        {
          return -1;
        }
        else if (g_pEm9304PatchesHCICmd[index][1] == 0x28)
        {
          if (((index + 1) == g_pEm9304Patches[patch].endingPatch) && (bytePtr[EM_PATCH_STATUS_OFFSET] != EM_PATCH_APPLIED))
          {
            return -1;
          }
          else if (bytePtr[EM_PATCH_STATUS_OFFSET] != EM_PATCH_CONTINUE)
          {
            return -1;
          }
        }
      }
    }
  }
  
  return 0;
}

//*****************************************************************************
//
// Configure the necessary pins and start the EM9304 radio.
//
//*****************************************************************************
void
initEM9304(void)
{
  uint32_t numBytesRx;
  
  if (validEM9304Patches())
  {
    //
    // Query the EM9304 for patches
    //
    queryEM9304Patches();
    
    //
    // Apply the patches not already in the EM9304
    //
    if (applyEM9304Patches() < 0)
    {
      am_util_debug_printf("EM9304 Patching Failed\n");
    }
    
  }
  
  // Send EM_CpuReset HCI command.
  am_devices_em9304_block_write(&g_sEm9304, HCI_CMD_TYPE, g_pui8EM_CpuReset, sizeof(g_pui8EM_CpuReset) );
  do
  {
    numBytesRx = am_devices_em9304_block_read(&g_sEm9304, g_pui32HCIRXBuffer,0);
    if ((numBytesRx == 7) && (0x0000FC32 == (g_pui32HCIRXBuffer[0] & 0x0000FFFF)))
    {
      am_util_debug_printf("EM9304 CPU Reset Successfully\n");
      break;
    }
  } while ((numBytesRx == 0) || ((numBytesRx == 4) && (0x0000FF04 == (g_pui32HCIRXBuffer[0] & 0x0000FFFF))));
  
}
