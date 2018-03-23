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

#include <string.h>
#include "wsf_types.h"
#include "wsf_msg.h"
#include "bstream.h"
#include "hci_api.h"
#include "hci_core.h"
#include "hci_tr.h"
#include "ll_api.h"

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
  uint16_t len;
  uint8_t  *p;

  /* if fragmenting */
  if (hciCoreTxAclDataFragmented(pContext))
  {
    /* get 16-bit length */
    BYTES_TO_UINT16(len, (pData + 2))
    len += HCI_ACL_HDR_LEN;
    
    /* allocate LL buffer */
    if ((p = WsfMsgDataAlloc(len, HCI_TX_DATA_TAILROOM)) != NULL)
    {
      /* copy data */
      memcpy(p, pData, len);

      /* send to LL  */
      LlSendAclData(p);
      
      /* free HCI buffer */
      hciCoreTxAclComplete(pContext, pData);
    }
  }
  else
  {
    /* send to LL  */
    LlSendAclData(pData);
    
    /* LL will free HCI buffer */
    hciCoreTxAclComplete(pContext, NULL);
  }
}
