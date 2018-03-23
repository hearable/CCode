/*************************************************************************************************/
/*!
 *  \file   sec_ecc.c
 *        
 *  \brief  Security ECC implementation using uECC.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *  
 *  Copyright (c) 2010-2017 ARM Ltd., all rights reserved.
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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "wsf_types.h"
#include "wsf_queue.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "sec_api.h"
#include "sec_main.h"
#include "wsf_buf.h"
#include "hci_api.h"
#include "calc128.h"
#include "uECC.h"

#ifndef SEC_ECC_CFG
#define SEC_ECC_CFG SEC_ECC_CFG_UECC
#endif

#if SEC_ECC_CFG == SEC_ECC_CFG_UECC

/**************************************************************************************************
  External Variables
**************************************************************************************************/

extern secCb_t secCb;

/*************************************************************************************************/
/*!
 *  \fn     secEccRng
 *        
 *  \brief  Random number generator used by uECC.
 *
 *  \param  p_dest      Buffer to hold random number
 *  \param  p_size      Size of p_dest in bytes .
 *
 *  \return TRUE if successful.
 */
/*************************************************************************************************/
static int secEccRng(uint8_t *p_dest, unsigned p_size)
{
  SecRand(p_dest, p_size);
  return TRUE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccHciCback
 *
 *  \brief  Callback for HCI encryption for ECC operations.
 *
 *  \param  pBuf        Pointer to sec queue element.
 *  \param  pEvent      Pointer to HCI event.
 *  \param  handlerId   WSF handler ID.
 *
 *  \return none.
 */
/*************************************************************************************************/
void SecEccHciCback(secQueueBuf_t *pBuf, hciEvt_t *pEvent, wsfHandlerId_t handlerId)
{
  /* TBD */
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccGenKey
 *        
 *  \brief  Generate an ECC key.
 *
 *  \param  handlerId   WSF handler ID for client.
 *  \param  param       Optional parameter sent to client's WSF handler.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return TRUE if successful, else FALSE.
 */
/*************************************************************************************************/
bool_t SecEccGenKey(wsfHandlerId_t handlerId, uint16_t param, uint8_t event)
{
  secEccMsg_t *pMsg = WsfMsgAlloc(sizeof(secEccMsg_t));

  if (pMsg)
  {
    /* Generate the keys */ 
    uECC_make_key(pMsg->data.key.pubKey_x, pMsg->data.key.privKey);

    /* Send shared secret to handler */ 
    pMsg->hdr.event = event;
    pMsg->hdr.param = param;
    pMsg->hdr.status = HCI_SUCCESS;
    WsfMsgSend(handlerId, pMsg);

    return TRUE;
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccGenSharedSecret
 *        
 *  \brief  Generate an ECC key.
 *
 *  \param  pKey        ECC Key structure.
 *  \param  handlerId   WSF handler ID for client.
 *  \param  param       Optional parameter sent to client's WSF handler.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return TRUE if successful, else FALSE.
 */
/*************************************************************************************************/
bool_t SecEccGenSharedSecret(secEccKey_t *pKey, wsfHandlerId_t handlerId, uint16_t param, uint8_t event)
{
  secEccMsg_t *pMsg = WsfMsgAlloc(sizeof(secEccMsg_t));

  if (pMsg)
  {
    uECC_shared_secret(pKey->pubKey_x, pKey->privKey, pMsg->data.sharedSecret.secret);

    /* Send shared secret to handler */  
    pMsg->hdr.event = event;
    pMsg->hdr.param = param;
    pMsg->hdr.status = HCI_SUCCESS;
    WsfMsgSend(handlerId, pMsg);

    return TRUE;
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecEccInit
 *        
 *  \brief  Called to initialize ECC security.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecEccInit()
{
  srand((unsigned int)time(NULL));
  uECC_set_rng(secEccRng);
}

#endif /* SEC_ECC_CFG */
