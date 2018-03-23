/*************************************************************************************************/
/*!
 *  \file   cycling_api.h
 *
 *  \brief  Cycling server sample application.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *
 *  Copyright (c) 2016-2017 ARM Ltd., all rights reserved.
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
#ifndef CYCLING_API_H
#define CYCLING_API_H

#include "wsf_os.h"

#ifdef __cplusplus
extern "C" {
#endif
  
/**************************************************************************************************
  Function Declarations
**************************************************************************************************/
/*************************************************************************************************/
/*!
 *  \fn     CyclingStart
 *        
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CyclingStart(void);

/*************************************************************************************************/
/*!
 *  \fn     CyclingHandlerInit
 *        
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID for App.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CyclingHandlerInit(wsfHandlerId_t handlerId);


/*************************************************************************************************/
/*!
 *  \fn     CyclingHandler
 *        
 *  \brief  WSF event handler for the application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CyclingHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

#ifdef __cplusplus
};
#endif

#endif /* CYCLING_API_H */
