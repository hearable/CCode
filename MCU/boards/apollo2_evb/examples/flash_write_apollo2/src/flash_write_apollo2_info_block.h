//*****************************************************************************
//
//! @file flash_write_apollo2_info_block.h
//!
//! @brief Flash write examples for info block specifically for Apollo 2.
//!
//! This example shows how to modify the internal Flash using HAL flash helper
//! functions.
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
#ifndef FLASH_WRITEA2_INFO_BLOCK_H
#define FLASH_WRITEA2_INFO_BLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif
//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************

//*****************************************************************************
//
// External variable definitions
//
//*****************************************************************************

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern int info_block_erase(void);
extern int info_block_pattern_write(void);
extern int info_block_write_board_name(char *pChar);
extern int info_block_write_ble_address(uint64_t ui64BLEAddress);
extern int info_block_disable_erase(void);
extern int info_block_disable_program(uint32_t ui32Mask, uint32_t ui32ExpectMask);
extern int info_block_enable_flash_wipe(void);

#ifdef __cplusplus
}
#endif

#endif // FLASH_WRITEA2_INFO_BLOCK_H


