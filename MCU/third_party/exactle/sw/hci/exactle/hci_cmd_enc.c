/*************************************************************************************************/
/*!
 *  \file   hci_cmd.c
 *
 *  \brief  HCI command module.
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_queue.h"
#include "wsf_timer.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "bstream.h"
#include "calc128.h"
#include "hci_cmd.h"
#include "hci_tr.h"
#include "hci_api.h"
#include "hci_main.h"

#include "ll_api.h"

/*************************************************************************************************/
/*!
 *  \fn     HciLeEncryptCmd
 *
 *  \brief  HCI LE encrypt command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeEncryptCmd(uint8_t *pKey, uint8_t *pData)
{
  hciLeEncryptCmdCmplEvt_t evt;
  uint8_t status;
  (void)status;

  status = LlEncrypt(pKey, pData);
  WSF_ASSERT(status == LL_SUCCESS);
  evt.hdr.event = HCI_LE_ENCRYPT_CMD_CMPL_CBACK_EVT;
  evt.status = evt.hdr.status = status;
  Calc128Cpy(evt.data, pData);
  hciCb.secCback((hciEvt_t *) &evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeLtkReqNegReplCmd
 *
 *  \brief  HCI LE long term key request negative reply command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeLtkReqNegReplCmd(uint16_t handle)
{
  LlLtkReqNegReply(handle);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeLtkReqReplCmd
 *
 *  \brief  HCI LE long term key request reply command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeLtkReqReplCmd(uint16_t handle, uint8_t *pKey)
{
  LlLtkReqReply(handle, pKey);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeStartEncryptionCmd
 *
 *  \brief  HCI LE start encryption command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeStartEncryptionCmd(uint16_t handle, uint8_t *pRand, uint16_t diversifier, uint8_t *pKey)
{
  LlStartEncryption(handle, pRand, diversifier, pKey);
}

