/*************************************************************************************************/
/*!
 *  \file   sec_cmac.c
 *
 *  \brief  AES and random number security service implemented using HCI.
 *
 *          $Date: 2017-02-23 17:15:43 -0600 (Thu, 23 Feb 2017) $
 *          $Revision: 11263 $
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
#include "wstr.h"

enum
{
  SEC_CMAC_STATE_SUBKEY,
  SEC_CMAC_STATE_BLOCK,
  SEC_CMAC_STATE_COMPLETE,
};

/**************************************************************************************************
  External Variables
**************************************************************************************************/

extern secCb_t secCb;

/*************************************************************************************************/
/*!
 *  \fn     secCmacProcessBlock
 *
 *  \brief  Continue the execution of the CMAC algorithm over the next message block.
 *
 *  \param  pBuf    Security queue buffer containing CMAC algorithm control block.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void secCmacProcessBlock(secQueueBuf_t *pBuf)
{
  secCmacSecCb_t *pCmac = (secCmacSecCb_t*) pBuf->pCb;
  uint8_t buf[SEC_BLOCK_LEN];
  uint8_t *pMn = pCmac->pPlainText + pCmac->position;
  int8_t remaining = (int16_t) pCmac->len - pCmac->position;

  /* Check for Last Block */
  if (remaining <= SEC_BLOCK_LEN)
  {
    WStrReverseCpy(buf, pMn, SEC_BLOCK_LEN);

    /* Pad the message if necessary */
    if (remaining != SEC_BLOCK_LEN)
    {
      memset(buf, 0, SEC_BLOCK_LEN - remaining);
      buf[SEC_BLOCK_LEN-remaining-1] = 0x80;
    }

    /* XOr the subkey */
    Calc128Xor(buf, pCmac->subkey);
    pCmac->state = SEC_CMAC_STATE_COMPLETE;
  }
  else
  {
    /* Copy the block to the buffer */
    WStrReverseCpy(buf, pMn, SEC_BLOCK_LEN);
  }

  if (pCmac->position != 0)
  {
    /* Except for first block, XOr the previous AES calculation */
    Calc128Xor(buf, pBuf->ciphertext);
  }

  pCmac->position += SEC_BLOCK_LEN;

  /* Enqueue and perform AES operation */
  WsfMsgEnq(&secCb.queue, pCmac->handlerId, pBuf);
  HciLeEncryptCmd(pCmac->key, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     secCmacGenSubkey1
 *
 *  \brief  Step 1 to generate the subkey used in the CMAC algorithm.
 *
 *  \param  pBuf    Security queue buffer containing CMAC algorithm control block.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void secCmacGenSubkey1(secQueueBuf_t *pBuf)
{
  secCmacSecCb_t *pCmac = (secCmacSecCb_t*) pBuf->pCb;
  uint8_t buf[SEC_BLOCK_LEN];

  /* Perform aes on the key with a constant zero */
  memset(buf, 0, SEC_BLOCK_LEN);

  WsfMsgEnq(&secCb.queue, pCmac->handlerId, pBuf);
  HciLeEncryptCmd(pCmac->key, buf);
}

/*************************************************************************************************/
/*!
 *  \fn     secCmacKeyShift
 *
 *  \brief  Left shift a buffer of WSF_CMAC_KEY_LEN bytes by N bits.
 *
 *  \param  pBuf    Buffer to left shift.
 *  \param  shift   Number of bits to shift.
 *
 *  \return The overflow of the operaiton.
 */
/*************************************************************************************************/
static uint8_t secCmacKeyShift(uint8_t *pBuf, uint8_t shift)
{
  uint8_t bits, i, prevBits = 0;

  for (i = 0; i < SEC_CMAC_KEY_LEN; i++)
  {
    /* store shifted bits for next byte */
    bits = pBuf[i] >> (8 - shift);

    /* shift byte and OR in shifted bits from previous byte */
    pBuf[i] = (pBuf[i] << shift) | prevBits;

    prevBits = bits;
  }

  return prevBits;
}

/*************************************************************************************************/
/*!
 *  \fn     secCmacGenSubkey2
 *
 *  \brief  Complete generation of the subkey used in the CMAC algorithm.
 *
 *  \param  pBuf    Security queue buffer containing CMAC algorithm control block.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void secCmacGenSubkey2(secQueueBuf_t *pBuf)
{
  secCmacSecCb_t *pCmac = (secCmacSecCb_t*) pBuf->pCb;
  uint8_t overflow;

  /* Copy the result of the AES oepration */
  Calc128Cpy(pCmac->subkey, pBuf->ciphertext);

  /* Calculate the K1 subkey */
  overflow = secCmacKeyShift(pCmac->subkey, 1);

  if (overflow)
  {
    pCmac->subkey[0] ^= SEC_CMAC_RB;
  }

  if (pCmac->len % SEC_BLOCK_LEN != 0)
  {
    /* If the message len is not a multiple of SEC_BLOCK_LEN */
    /* Continue with generation of the K2 subkey based on the K1 key */
    overflow = secCmacKeyShift(pCmac->subkey, 1);

    if (overflow)
    {
      pCmac->subkey[0] ^= SEC_CMAC_RB;
    }
  }

  /* Begin CMAC calculation */
  pCmac->state = SEC_CMAC_STATE_BLOCK;
  secCmacProcessBlock(pBuf);
}

/*************************************************************************************************/
/*!
 *  \fn     secCmacComplete
 *
 *  \brief  Send a message to the handler with CMAC result.
 *
 *  \param  pBuf    Security queue buffer containing CMAC algorithm control block.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void secCmacComplete(secQueueBuf_t *pBuf)
{
  /* CMAC is complete, copy and send result to handler */
  secCmacMsg_t *pMsg = (secCmacMsg_t *) &pBuf->msg;
  secCmacSecCb_t *pCmac = (secCmacSecCb_t*) pBuf->pCb;

  WStrReverse(pBuf->ciphertext, SEC_CMAC_KEY_LEN);
  pMsg->pCiphertext = pBuf->ciphertext;
  WsfBufFree(pCmac->pPlainText);

  WsfMsgSend(pCmac->handlerId, pMsg);
}

/*************************************************************************************************/
/*!
 *  \fn     SecCmacHciCback
 *
 *  \brief  Callback for HCI encryption for CMAC operations.
 *
 *  \param  pBuf        Pointer to sec queue element.
 *  \param  pEvent      Pointer to HCI event.
 *  \param  handlerId   WSF handler ID.
 *
 *  \return none.
 */
/*************************************************************************************************/
void SecCmacHciCback(secQueueBuf_t *pBuf, hciEvt_t *pEvent, wsfHandlerId_t handlerId)
{
  secCmacSecCb_t *pCmac = (secCmacSecCb_t *) pBuf->pCb;

  if (pCmac)
  {
    Calc128Cpy(pBuf->ciphertext, pEvent->leEncryptCmdCmpl.data);

    switch (pCmac->state)
    {
    case SEC_CMAC_STATE_SUBKEY:
      secCmacGenSubkey2(pBuf);
      break;

    case SEC_CMAC_STATE_BLOCK:
      secCmacProcessBlock(pBuf);
      break;

    case SEC_CMAC_STATE_COMPLETE:
      secCmacComplete(pBuf);
      break;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \fn     SecAesCmac
 *
 *  \brief  Execute the CMAC algorithm.
 *
 *  \param  pKey        Key used in CMAC operation.
 *  \param  pPlainText  Data to perform CMAC operation over
 *  \param  len         Size of pPlaintext in bytes.
 *  \param  handlerId   WSF handler ID for client.
 *  \param  param       Optional parameter sent to client's WSF handler.
 *  \param  event       Event for client's WSF handler.
 *
 *  \return TRUE if successful, else FALSE.
 */
/*************************************************************************************************/
bool_t SecCmac(const uint8_t *pKey, uint8_t *pPlainText, uint8_t textLen, wsfHandlerId_t handlerId,
               uint16_t param, uint8_t event)
{
  secQueueBuf_t *pBuf;
  uint16_t bufSize = sizeof(secQueueBuf_t) + sizeof(secCmacSecCb_t);

  if ((pBuf = WsfMsgAlloc(bufSize)) != NULL)
  {
    secCmacSecCb_t *pCmacCb = (secCmacSecCb_t *) (pBuf + 1);

    /* Setup queue buffer */
    pBuf->pCb = pCmacCb;
    pBuf->type = SEC_TYPE_CMAC;

    pBuf->msg.hdr.status = secCb.token++;
    pBuf->msg.hdr.param = param;
    pBuf->msg.hdr.event = event;

    pCmacCb->pPlainText = pPlainText;

    pCmacCb->len = textLen;
    pCmacCb->position = 0;
    pCmacCb->handlerId = handlerId;
    pCmacCb->state = SEC_CMAC_STATE_SUBKEY;

    /* Copy key */
    WStrReverseCpy(pCmacCb->key, pKey, SEC_CMAC_KEY_LEN);

    /* Start the CMAC process by calculating the subkey */
    secCmacGenSubkey1(pBuf);

    return TRUE;
  }

  return FALSE;
}

/*************************************************************************************************/
/*!
 *  \fn     SecCmacInit
 *
 *  \brief  Called to initialize CMAC security.
 *
 *  \param  None.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SecCmacInit()
{
  secCb.hciCbackTbl[SEC_TYPE_CMAC] = SecCmacHciCback;
}

