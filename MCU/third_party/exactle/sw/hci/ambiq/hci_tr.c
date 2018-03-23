/*************************************************************************************************/
/*!
 *  \file   hci_tr.c
 *
 *  \brief  HCI transport module.
 *
 *          $Date: 2017-03-10 14:08:37 -0600 (Fri, 10 Mar 2017) $
 *          $Revision: 11501 $
 *
 *  Copyright (c) 2011-2017 ARM Ltd., all rights reserved.
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

#include "wsf_types.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "bstream.h"
#include "hci_api.h"
#include "hci_core.h"
#include "hci_drv.h"


/**************************************************************************************************
  State variable
**************************************************************************************************/
static volatile bool_t g_bHCIReceivingPacket = FALSE;

/**************************************************************************************************
  Macros
**************************************************************************************************/

#define HCI_HDR_LEN_MAX           HCI_ACL_HDR_LEN

/**************************************************************************************************
  Data Types
**************************************************************************************************/

typedef enum
{
  HCI_RX_STATE_IDLE,
  HCI_RX_STATE_HEADER,
  HCI_RX_STATE_DATA,
  HCI_RX_STATE_COMPLETE
} hciRxState_t;

/*************************************************************************************************/
/*!
 *  \fn     hciTrSendAclData
 *        
 *  \brief  Send a complete HCI ACL packet to the transport. 
 *
 *  \param  pContext Connection context.
 *  \param  pData    WSF msg buffer containing an ACL packet.
 *
 *  \return None.
 */
/*************************************************************************************************/
void hciTrSendAclData(void *pContext, uint8_t *pData)
{
  uint16_t   len;

  /* get 16-bit length */
  BYTES_TO_UINT16(len, &pData[2]);
  len += HCI_ACL_HDR_LEN;

  /* dump event for protocol analysis */
  HCI_PDUMP_TX_ACL(len, pData);

  /* transmit ACL header and data */
  if (hciDrvWrite(HCI_ACL_TYPE, len, pData) == len)
  {
    /* free buffer */
    hciCoreTxAclComplete(pContext, pData);
  }
}


/*************************************************************************************************/
/*!
 *  \fn     hciTrSendCmd
 *
 *  \brief  Send a complete HCI command to the transport.
 *
 *  \param  pData    WSF msg buffer containing an HCI command.
 *
 *  \return None.
 */
/*************************************************************************************************/
void hciTrSendCmd(uint8_t *pData)
{
  uint8_t   len;

  /* get length */
  len = pData[2] + HCI_CMD_HDR_LEN;

  /* dump event for protocol analysis */
  HCI_PDUMP_CMD(len, pData);

  /* transmit ACL header and data */
  if (hciDrvWrite(HCI_CMD_TYPE, len, pData) == len)
  {
    /* free buffer */
    WsfMsgFree(pData);
  }
}


/*************************************************************************************************/
/*!
 *  \fn     hciSerialRxIncoming
 *
 *  \brief  Receive function.  Gets called by external code when bytes are received.
 *
 *  \param  pBuf   Pointer to buffer of incoming bytes.
 *  \param  len    Number of bytes in incoming buffer.
 *
 *  \return None.
 */
/*************************************************************************************************/
void hciTrSerialRxIncoming(uint8_t *pBuf, uint8_t len)
{
  static uint8_t    stateRx = HCI_RX_STATE_IDLE;
  static uint8_t    pktIndRx;
  static uint16_t   iRx;
  static uint8_t    hdrRx[HCI_HDR_LEN_MAX];
  static uint8_t    *pPktRx;
  static uint8_t    *pDataRx;

  uint8_t dataByte;

  /* loop until all bytes of incoming buffer are handled */
  while (len--)
  {
    /* read single byte from incoming buffer and advance to next byte */
    dataByte = *pBuf++;

    /* --- Idle State --- */
    if (stateRx == HCI_RX_STATE_IDLE)
    {
      /* save the packet type */
      pktIndRx = dataByte;
      iRx      = 0;
      stateRx  = HCI_RX_STATE_HEADER;
      g_bHCIReceivingPacket = TRUE;
    }

    /* --- Header State --- */
    else if (stateRx == HCI_RX_STATE_HEADER)
    {
      uint8_t  hdrLen = 0;
      uint16_t dataLen = 0;

      /* copy current byte into the temp header buffer */
      hdrRx[iRx++] = dataByte;

      /* determine header length based on packet type */
      if (pktIndRx == HCI_EVT_TYPE)
      {
        hdrLen = HCI_EVT_HDR_LEN;
      }
      else if (pktIndRx == HCI_ACL_TYPE)
      {
        hdrLen = HCI_ACL_HDR_LEN;
      }
      else
      {
        /* invalid packet type */
        WSF_ASSERT(0);
      }

      /* see if entire header has been read */
      if (iRx == hdrLen)
      {
        /* extract data length from header */
        if (pktIndRx == HCI_EVT_TYPE)
        {
          dataLen = hdrRx[1];
        }
        else if (pktIndRx == HCI_ACL_TYPE)
        {
          BYTES_TO_UINT16(dataLen, &hdrRx[2]);
        }

        /* allocate data buffer to hold entire packet */
        if (pktIndRx == HCI_ACL_TYPE)
        {
          pPktRx = (uint8_t*)WsfMsgDataAlloc(hdrLen + dataLen, 0);
        }
        else
        {
          pPktRx = (uint8_t*)WsfMsgAlloc(hdrLen + dataLen);
        }

        if (pPktRx != NULL)
        {
          pDataRx = pPktRx;

          /* copy header into data packet (note: memcpy is not so portable) */
          {
            uint8_t  i;
            for (i = 0; i < hdrLen; i++)
            {
              *pDataRx++ = hdrRx[i];
            }
          }

          /* save number of bytes left to read */
          iRx = dataLen;
          if (iRx == 0)
          {
            stateRx = HCI_RX_STATE_COMPLETE;
          }
          else
          {
            stateRx = HCI_RX_STATE_DATA;
          }
        }
        else
        {
          WSF_ASSERT(0); /* allocate falied */
        }

      }
    }

    /* --- Data State --- */
    else if (stateRx == HCI_RX_STATE_DATA)
    {
      /* write incoming byte to allocated buffer */
      *pDataRx++ = dataByte;

      /* determine if entire packet has been read */
      iRx--;
      if (iRx == 0)
      {
        stateRx = HCI_RX_STATE_COMPLETE;
      }
    }

    /* --- Complete State --- */
    /* ( Note Well!  There is no else-if construct by design. ) */
    if (stateRx == HCI_RX_STATE_COMPLETE)
    {
      g_bHCIReceivingPacket = FALSE;

      /* deliver data */
      if (pPktRx != NULL)
      {
        hciCoreRecv(pktIndRx, pPktRx);
      }

      /* reset state machine */
      stateRx = HCI_RX_STATE_IDLE;
    }
  }
}

//*****************************************************************************
//
//! @brief Check to see if the state machine has received part of a packet.
//!
//! This function checks the HCI packet-receive state machine to see if it is
//! in the middle of receiving a packet. This information can be useful in
//! determining whether the serial interface should remain enabled.
//!
//! @return TRUE if there is a packet in progress.
//
//*****************************************************************************
bool_t
hciTrReceivingPacket(void)
{
    return g_bHCIReceivingPacket;
}
