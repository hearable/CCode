//*****************************************************************************
//
//! @file em9304_patches.h
//!
//! @brief This is a generated file.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef EM9304_PATCHES_H
#define EM9304_PATCHES_H

//*****************************************************************************
//
// Length of the binary array in bytes.
//
//*****************************************************************************
#define EM9304_PATCHES_NUM_PATCHES          4

//*****************************************************************************
//
// EM9304 Container Info type
//
//*****************************************************************************
typedef struct
{
  uint16_t	buildNumber;		// Firmware Build Number
  uint16_t  userBuildNumber;	// User defined Build Number (determines patch precedence)
  uint8_t	containerVersion;	// Container Version
  uint8_t	containerType;		// Container Type
  uint8_t	containerID;		// Container ID
  bool		applyPatch;			// Flag to apply this patch.
  uint8_t   startingPatch;		// Starting patch index.
  uint8_t	endingPatch;		// Ending patch index + 1.
} em9304_container_info_t;

//*****************************************************************************
//
// Extracted binary array.
//
//*****************************************************************************
extern em9304_container_info_t g_pEm9304Patches[4];
extern const uint8_t g_pEm9304PatchesHCICmd[136][68];

#endif // EM9304_PATCHES_H
