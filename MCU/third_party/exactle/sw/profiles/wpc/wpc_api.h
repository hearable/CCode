/*************************************************************************************************/
/*!
 *  \file   wpc_api.h
 *
 *  \brief  ARM Ltd. proprietary profile client.
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
#ifndef WPC_API_H
#define WPC_API_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! ARM Ltd. proprietary service P1 enumeration of handle indexes of characteristics to be discovered */
enum
{
  WPC_P1_DAT_HDL_IDX,           /*! Proprietary data */
  WPC_P1_NA_CCC_HDL_IDX,        /*! Proprietary data client characteristic configuration descriptor */
  WPC_P1_HDL_LIST_LEN           /*! Handle list length */
};  

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \fn     WpcP1Discover
 *        
 *  \brief  Perform service and characteristic discovery for ARM Ltd. proprietary service P1. 
 *          Parameter pHdlList must point to an array of length WPC_P1_HDL_LIST_LEN. 
 *          If discovery is successful the handles of discovered characteristics and
 *          descriptors will be set in pHdlList.
 *
 *  \param  connId    Connection identifier.
 *  \param  pHdlList  Characteristic handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WpcP1Discover(dmConnId_t connId, uint16_t *pHdlList);
                     
#ifdef __cplusplus
};
#endif

#endif /* WPC_API_H */
