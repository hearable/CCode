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
 *  \fn     HciDisconnectCmd
 *
 *  \brief  HCI disconnect command.
 */
/*************************************************************************************************/
void HciDisconnectCmd(uint16_t handle, uint8_t reason)
{
  LlDisconnect(handle, reason);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeAddDevWhiteListCmd
 *
 *  \brief  HCI LE add device white list command.
 */
/*************************************************************************************************/
void HciLeAddDevWhiteListCmd(uint8_t addrType, uint8_t *pAddr)
{
  LlAddDeviceToWhitelist(addrType, pAddr);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeClearWhiteListCmd
 *
 *  \brief  HCI LE clear white list command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeClearWhiteListCmd(void)
{
  LlClearWhitelist();
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeConnUpdateCmd
 *
 *  \brief  HCI connection update command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeConnUpdateCmd(uint16_t handle, hciConnSpec_t *pConnSpec)
{
  LlConnUpdate(handle, (LlConnSpec_t *)pConnSpec);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeRandCmd
 *
 *  \brief  HCI LE random command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeRandCmd(void)
{
  hciLeRandCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_RAND_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = HCI_SUCCESS;

  evt.status = HCI_SUCCESS;

  LlGetRandNum(evt.randNum);

  hciCb.secCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadAdvTXPowerCmd
 *
 *  \brief  HCI LE read advertising TX power command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadAdvTXPowerCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadBufSizeCmd
 *
 *  \brief  HCI LE read buffer size command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadBufSizeCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadChanMapCmd
 *
 *  \brief  HCI LE read channel map command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadChanMapCmd(uint16_t handle)
{
  hciReadChanMapCmdCmplEvt_t evt;

  evt.hdr.param = handle;
  evt.hdr.event = HCI_LE_READ_CHAN_MAP_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlGetChannelMap(handle, evt.chanMap);

  evt.handle = handle;
  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadLocalSupFeatCmd
 *
 *  \brief  HCI LE read local supported feautre command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadLocalSupFeatCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadRemoteFeatCmd
 *
 *  \brief  HCI LE read remote feature command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadRemoteFeatCmd(uint16_t handle)
{
  LlReadRemoteFeat(handle);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadSupStatesCmd
 *
 *  \brief  HCI LE read supported states command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadSupStatesCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadWhiteListSizeCmd
 *
 *  \brief  HCI LE read white list size command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadWhiteListSizeCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeRemoveDevWhiteListCmd
 *
 *  \brief  HCI LE remove device white list command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeRemoveDevWhiteListCmd(uint8_t addrType, uint8_t *pAddr)
{
  LlRemoveDeviceFromWhitelist(addrType, pAddr);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetAdvEnableCmd
 *
 *  \brief  HCI LE set advanced enable command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetAdvEnableCmd(uint8_t enable)
{
  LlAdvEnable(enable);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetAdvDataCmd
 *
 *  \brief  HCI LE set advertising data command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetAdvDataCmd(uint8_t len, uint8_t *pData)
{
  LlSetAdvData(len, pData);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetAdvParamCmd
 *
 *  \brief  HCI LE set advertising parameters command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetAdvParamCmd(uint16_t advIntervalMin, uint16_t advIntervalMax, uint8_t advType,
                         uint8_t ownAddrType, uint8_t peerAddrType, uint8_t *pPeerAddr,
                         uint8_t advChanMap, uint8_t advFiltPolicy)
{
  LlSetAdvParam(advIntervalMin,
                advIntervalMax,
                advType,
                ownAddrType,
                peerAddrType,
                pPeerAddr,
                advChanMap,
                advFiltPolicy);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetEventMaskCmd
 *
 *  \brief  HCI LE set event mask command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetEventMaskCmd(uint8_t *pLeEventMask)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetHostChanClassCmd
 *
 *  \brief  HCI set host channel class command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetHostChanClassCmd(uint8_t *pChanMap)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetRandAddrCmd
 *
 *  \brief  HCI LE set random address command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetRandAddrCmd(uint8_t *pAddr)
{
  LlSetRandAddr(pAddr);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetScanRespDataCmd
 *
 *  \brief  HCI LE set scan response data.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetScanRespDataCmd(uint8_t len, uint8_t *pData)
{
  LlSetScanRespData(len, pData);
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadBdAddrCmd
 *
 *  \brief  HCI read BD address command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadBdAddrCmd(void)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadBufSizeCmd
 *
 *  \brief  HCI read buffer size command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadBufSizeCmd(void)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadLocalSupFeatCmd
 *
 *  \brief  HCI read local supported feature command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadLocalSupFeatCmd(void)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadLocalVerInfoCmd
 *
 *  \brief  HCI read local version info command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadLocalVerInfoCmd(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadRemoteVerInfoCmd
 *
 *  \brief  HCI read remote version info command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadRemoteVerInfoCmd(uint16_t handle)
{
  LlReadRemoteVerInfo(handle);
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadRssiCmd
 *
 *  \brief  HCI read RSSI command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadRssiCmd(uint16_t handle)
{
  hciReadRssiCmdCmplEvt_t evt;

  evt.hdr.param = handle;
  evt.hdr.event = HCI_READ_RSSI_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlGetRssi(handle, &evt.rssi);

  evt.handle = handle;
  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadTxPwrLvlCmd
 *
 *  \brief  HCI read Tx power level command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadTxPwrLvlCmd(uint16_t handle, uint8_t type)
{
  hciReadTxPwrLvlCmdCmplEvt_t evt;

  evt.hdr.param = handle;
  evt.hdr.event = HCI_READ_TX_PWR_LVL_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlGetTxPowerLevel(handle, type, &evt.pwrLvl);

  evt.handle = handle;
  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciResetCmd
 *
 *  \brief  HCI reset command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciResetCmd(void)
{
  LlReset();
}

/*************************************************************************************************/
/*!
 *  \fn     HciSetEventMaskCmd
 *
 *  \brief  HCI set event mask command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciSetEventMaskCmd(uint8_t *pEventMask)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeAddDeviceToResolvingListCmd
 *
 *  \brief  HCI add device to resolving list command.
 *
 *  \param  peerAddrType        Peer identity address type.
 *  \param  pPeerIdentityAddr   Peer identity address.
 *  \param  pPeerIrk            Peer IRK.
 *  \param  pLocalIrk           Local IRK.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeAddDeviceToResolvingListCmd(uint8_t peerAddrType, const uint8_t *pPeerIdentityAddr,
                                      const uint8_t *pPeerIrk, const uint8_t *pLocalIrk)
{ 
  hciLeAddDevToResListCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_ADD_DEV_TO_RES_LIST_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlAddDeviceToResolvingList(peerAddrType, pPeerIdentityAddr, pPeerIrk, pLocalIrk);

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeRemoveDeviceFromResolvingList
 *
 *  \brief  HCI remove device from resolving list command.
 *
 *  \param  peerAddrType        Peer identity address type.
 *  \param  pPeerIdentityAddr   Peer identity address.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeRemoveDeviceFromResolvingList(uint8_t peerAddrType, const uint8_t *pPeerIdentityAddr)
{
  hciLeRemDevFromResListCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_REM_DEV_FROM_RES_LIST_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlRemoveDeviceFromResolvingList(peerAddrType, pPeerIdentityAddr);

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeClearResolvingList
 *
 *  \brief  HCI clear resolving list command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeClearResolvingList(void)
{ 
  hciLeClearResListCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_CLEAR_RES_LIST_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlClearResolvingList();

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadResolvingListSize
 *
 *  \brief  HCI read resolving list command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadResolvingListSize(void)
{
  /* unused */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadPeerResolvableAddr
 *
 *  \brief  HCI read peer resolvable address command.
 *
 *  \param  addrType        Peer identity address type.
 *  \param  pIdentityAddr   Peer identity address.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadPeerResolvableAddr(uint8_t addrType, const uint8_t *pIdentityAddr)
{
  hciLeReadPeerResAddrCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_READ_PEER_RES_ADDR_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlReadPeerResolvableAddr(addrType, pIdentityAddr, evt.peerRpa);

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadLocalResolvableAddr
 *
 *  \brief  HCI read local resolvable address command.
 *
 *  \param  addrType        Peer identity address type.
 *  \param  pIdentityAddr   Peer identity address.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadLocalResolvableAddr(uint8_t addrType, const uint8_t *pIdentityAddr)
{
  hciLeReadLocalResAddrCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_READ_LOCAL_RES_ADDR_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlReadLocalResolvableAddr(addrType, pIdentityAddr, evt.localRpa);

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetAddrResolutionEnable
 *
 *  \brief  HCI enable or disable address resolution command.
 *
 *  \param  enable          Set to TRUE to enable address resolution or FALSE to disable address
 *                          resolution.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetAddrResolutionEnable(uint8_t enable)
{
  hciLeSetAddrResEnableCmdCmplEvt_t evt;

  evt.hdr.param = 0;
  evt.hdr.event = HCI_LE_SET_ADDR_RES_ENABLE_CMD_CMPL_CBACK_EVT;
  evt.hdr.status = LlSetAddrResolutionEnable(enable);

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetResolvablePrivateAddrTimeout
 *
 *  \brief  HCI set resolvable private address timeout command.
 *
 *  \param  rpaTimeout      Timeout measured in seconds.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetResolvablePrivateAddrTimeout(uint16_t rpaTimeout)
{
  LlSetResolvablePrivateAddrTimeout(rpaTimeout);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetPrivacyModeCmd
 *
 *  \brief  HCI LE set privacy mode command.
 *
 *  \param  peerAddrType    Peer identity address type.
 *  \param  pPeerAddr       Peer identity address.
 *  \param  mode            Privacy mode.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetPrivacyModeCmd(uint8_t addrType, uint8_t *pAddr, uint8_t mode)
{
  LlSetPrivacyMode(addrType, pAddr, mode);
}

/*************************************************************************************************/
/*!
 *  \fn     HciVendorSpecificCmd
 *
 *  \brief  HCI vencor specific command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciVendorSpecificCmd(uint16_t opcode, uint8_t len, uint8_t *pData)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeRemoteConnParamReqReply
 *
 *  \brief  HCI Remote Connection Parameter Request Reply.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeRemoteConnParamReqReply(uint16_t handle, uint16_t intervalMin, uint16_t intervalMax, uint16_t latency,
                                  uint16_t timeout, uint16_t minCeLen, uint16_t maxCeLen)
{
  LlConnSpec_t connSpec;

  connSpec.connIntervalMax = intervalMax;
  connSpec.connIntervalMin = intervalMin;
  connSpec.connLatency = latency;
  connSpec.maxCeLen = maxCeLen;
  connSpec.minCeLen = minCeLen;
  connSpec.supTimeout = timeout;
  
  LlRemoteConnParamReqReply(handle, &connSpec);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeRemoteConnParamReqNegReply
 *
 *  \brief  HCI Remote Connection Parameter Request Negative Reply.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeRemoteConnParamReqNegReply(uint16_t handle, uint8_t reason)
{
  LlRemoteConnParamReqNegReply(handle, reason);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeSetDataLen
 *
 *  \brief  HCI LE Set Data Length.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeSetDataLen(uint16_t handle, uint16_t txOctets, uint16_t txTime)
{
  LlSetDataLen(handle, txOctets, txTime);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadDefDataLen
 *
 *  \brief  HCI LE Read Default Data Length.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadDefDataLen(void)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeWriteDefDataLen
 *
 *  \brief  HCI LE Write Default Data Length.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeWriteDefDataLen(uint16_t suggestedMaxTxOctets, uint16_t suggestedMaxTxTime)
{
  LlWriteDefaultDataLen(suggestedMaxTxOctets, suggestedMaxTxTime);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadLocalP256PubKey
 *
 *  \brief  HCI LE Read Local P-256 Public Key.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadLocalP256PubKey(void)
{
  LlGenerateP256KeyPair();
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeGenerateDHKey
 *
 *  \brief  HCI LE Generate DH Key.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeGenerateDHKey(uint8_t *pPubKeyX, uint8_t *pPubKeyY)
{
  LlGenerateDhKey(pPubKeyX, pPubKeyY);
}

/*************************************************************************************************/
/*!
 *  \fn     HciLeReadMaxDataLen
 *
 *  \brief  HCI LE Read Maximum Data Length.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciLeReadMaxDataLen(void)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciReadAuthPayloadTimeout
 *
 *  \brief  HCI read authenticated payload timeout command.
 *
 *  \param  handle    Connection handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciReadAuthPayloadTimeout(uint16_t handle)
{
  /* not used */
}

/*************************************************************************************************/
/*!
 *  \fn     HciWriteAuthPayloadTimeout
 *
 *  \brief  HCI write authenticated payload timeout command.
 *
 *  \param  handle    Connection handle.
 *  \param  timeout   Timeout value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciWriteAuthPayloadTimeout(uint16_t handle, uint16_t timeout)
{
  hciWriteAuthPayloadToCmdCmplEvt_t evt;

  evt.status = LlWriteAuthPayloadTimeout(handle, timeout);
  evt.hdr.event = HCI_WRITE_AUTH_PAYLOAD_TO_CMD_CMPL_CBACK_EVT;
  evt.handle = handle;

  evt.status = evt.hdr.status;

  hciCb.evtCback((hciEvt_t *)&evt);
}

/*************************************************************************************************/
/*!
 *  \fn     HciSetEventMaskPage2Cmd
 *
 *  \brief  HCI set event page 2 mask command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void HciSetEventMaskPage2Cmd(uint8_t *pEventMask)
{
  /* unused */
}
