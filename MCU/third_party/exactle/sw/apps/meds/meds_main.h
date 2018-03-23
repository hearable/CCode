/*************************************************************************************************/
/*!
 *  \file   meds_main.h
 *        
 *  \brief  Health/medical sensor sample application interface file.
 *
 *          $Date: 2016-12-28 16:12:14 -0600 (Wed, 28 Dec 2016) $
 *          $Revision: 10805 $
 *  
 *  Copyright (c) 2012-2017 ARM Ltd., all rights reserved.
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

#ifndef MEDS_MAIN_H
#define MEDS_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! WSF message event starting value */
#define MEDS_MSG_START             0xA0

/*! WSF message event enumeration */
enum
{
  MEDS_TIMER_IND = MEDS_MSG_START,    /*! Timer expired */
};

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! profile interface callback functions */
typedef void (*medsStartCback_t)(void);
typedef void (*medsProcMsgCback_t)(wsfMsgHdr_t *pMsg);
typedef void (*medsBtnCback_t)(dmConnId_t connId, uint8_t btn);

/*! profile interface structure */
typedef struct
{
  medsStartCback_t      start;
  medsProcMsgCback_t    procMsg;
  medsBtnCback_t        btn;
} medsIf_t;

/*! application control block */
typedef struct
{
  medsIf_t          *pIf;                           /*! Profile interface */
  wsfHandlerId_t    handlerId;                      /*! WSF hander ID */
} medsCb_t;

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! application control block */
extern medsCb_t medsCb;

/*! profile interface pointers */
extern medsIf_t medsBlpIf;          /* blood pressure profile */
extern medsIf_t medsWspIf;          /* weight scale profile */
extern medsIf_t medsHtpIf;          /* health thermometer profile */
extern medsIf_t medsPlxIf;          /* pulse oximeter profile */
extern medsIf_t medsGlpIf;          /* glucose profile */

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void medsCccCback(attsCccEvt_t *pEvt);

#ifdef __cplusplus
};
#endif

#endif /* MEDS_MAIN_H */

