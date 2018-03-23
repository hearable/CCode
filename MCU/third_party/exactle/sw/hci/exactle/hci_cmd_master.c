/*************************************************************************************************/
/*!
 *  \file   hci_cmd_master.c
 *
 *  \brief  HCI command module for master.
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
#include "hci_cmd.h"
#include "hci_tr.h"
#include "hci_api.h"
#include "hci_main.h"

#include "ll_api.h"

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetScanEnableCmd
 *
 *  \brief  HCI LE set scan enable command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetScanEnableCmd(uint8_t enable, uint8_t filterDup)
{
  LlScanEnable(enable, filterDup);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetScanParamCmd
 *
 *  \brief  HCI set scan parameters command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetScanParamCmd(uint8_t scanType, uint16_t scanInterval, uint16_t scanWindow,
                          uint8_t ownAddrType, uint8_t scanFiltPolicy)
{
  LlScanParam_t param;
  uint8_t status;
  (void)status;

  param.scanType = scanType;
  param.scanInterval = scanInterval;
  param.scanWindow = scanWindow;
  param.ownAddrType = ownAddrType;
  param.scanFiltPolicy = scanFiltPolicy;
  
  status = LlSetScanParam(&param);

  WSF_ASSERT(status == LL_SUCCESS);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeCreateConnCmd
 *
 *  \brief  HCI LE create connection command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeCreateConnCmd(uint16_t scanInterval, uint16_t scanWindow, uint8_t filterPolicy,
                        uint8_t peerAddrType, uint8_t *pPeerAddr, uint8_t ownAddrType,
                        hciConnSpec_t *pConnSpec)
{
  LlInitParam_t initParam;
  
  initParam.scanInterval = scanInterval;
  initParam.scanWindow = scanWindow;
  initParam.filterPolicy = filterPolicy;
  initParam.ownAddrType = ownAddrType;
  initParam.peerAddrType = peerAddrType;
  initParam.pPeerAddr = pPeerAddr;
  
  LlCreateConn(&initParam, (LlConnSpec_t *)pConnSpec);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeCreateConnCancelCmd
 *
 *  \brief  HCI LE create connection cancel command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeCreateConnCancelCmd(void)
{
  LlCreateConnCancel();
}
