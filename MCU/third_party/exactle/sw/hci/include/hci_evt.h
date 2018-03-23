/*************************************************************************************************/
/*!
 *  \file   hci_evt.h
 *
 *  \brief  HCI event module.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2009-2017 ARM Ltd., all rights reserved.
 *  ARM Ltd. confidential and proprietary.
 *
 *  IMPORTANT.  Your use of this file is governed by a Software License Agreement
 *  ("Agreement") that must be accepted in order to download or otherwise receive a
 *  copy of this file.  You may not use or copy this file for any purpose other than
 *  as described in the Agreement.  If you do not agree to all of the terms of the
 *  Agreement do not use this file and delete all copies in your possession or control;
 *  if you do not have a copy of the Agreement, you must contact ARM Ltd. prior
 *  to any use, copying or further distribution of this software.
 */
/*************************************************************************************************/
#ifndef HCI_EVT_H
#define HCI_EVT_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/* HCI event statistics */
typedef struct 
{
  uint16_t numDiscCmplEvt;
  uint16_t numEncChangeEvt;
  uint16_t numReadRemoteVerInfoCmpEvt;
  uint16_t numCmdCmplEvt;
  uint16_t numCmdStatusEvt;
  uint16_t numHwErrorEvt;
  uint16_t numCmplPktsEvt;
  uint16_t numDataBufOverflowEvt;
  uint16_t numEncKeyRefreshCmplEvt;
  uint16_t numLeMetaEvt;
  uint16_t numVendorSpecEvt;
  uint16_t numAuthToEvt;
} hciEvtStats_t;

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void hciEvtProcessMsg(uint8_t *pEvt);
hciEvtStats_t *hciEvtGetStats(void);

#ifdef __cplusplus
};
#endif

#endif /* HCI_EVT_H */
