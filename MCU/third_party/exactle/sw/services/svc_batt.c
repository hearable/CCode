/*************************************************************************************************/
/*!
 *  \file   svc_batt.c
 *        
 *  \brief  Example Battery service implementation.
 *
 *          $Date: 2017-02-07 19:05:49 -0600 (Tue, 07 Feb 2017) $
 *          $Revision: 11110 $
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
#include "att_api.h"
#include "bstream.h"
#include "svc_batt.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef BATT_SEC_PERMIT_READ
#define BATT_SEC_PERMIT_READ SVC_SEC_PERMIT_READ
#endif

/*! Characteristic write permissions */
#ifndef BATT_SEC_PERMIT_WRITE
#define BATT_SEC_PERMIT_WRITE SVC_SEC_PERMIT_WRITE
#endif

/**************************************************************************************************
 Battery Service group
**************************************************************************************************/

/*!
 * Battery service
 */

/* Battery service declaration */
static const uint8_t battValSvc[] = {UINT16_TO_BYTES(ATT_UUID_BATTERY_SERVICE)};
static const uint16_t battLenSvc = sizeof(battValSvc);

/* Battery level characteristic */ 
static const uint8_t battValLvlCh[] = {ATT_PROP_READ | ATT_PROP_NOTIFY, UINT16_TO_BYTES(BATT_LVL_HDL), UINT16_TO_BYTES(ATT_UUID_BATTERY_LEVEL)};
static const uint16_t battLenLvlCh = sizeof(battValLvlCh);

/* Battery level */
static uint8_t battValLvl[] = {0};
static const uint16_t battLenLvl = sizeof(battValLvl);

/* Battery level client characteristic configuration */
static uint8_t battValLvlChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t battLenLvlChCcc = sizeof(battValLvlChCcc);

/* Attribute list for group */
static const attsAttr_t battList[] =
{
  /* Service declaration */
  {
    attPrimSvcUuid, 
    (uint8_t *) battValSvc,
    (uint16_t *) &battLenSvc, 
    sizeof(battValSvc),
    0,
    ATTS_PERMIT_READ
  },
  /* Characteristic declaration */
  {
    attChUuid,
    (uint8_t *) battValLvlCh,
    (uint16_t *) &battLenLvlCh,
    sizeof(battValLvlCh),
    0,
    ATTS_PERMIT_READ
  },
  /* Characteristic value */
  {
    attBlChUuid,
    battValLvl,
    (uint16_t *) &battLenLvl,
    sizeof(battValLvl),
    ATTS_SET_READ_CBACK,
    BATT_SEC_PERMIT_READ
  },
  /* Characteristic CCC descriptor */
  {
    attCliChCfgUuid,
    battValLvlChCcc,
    (uint16_t *) &battLenLvlChCcc,
    sizeof(battValLvlChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | BATT_SEC_PERMIT_WRITE)
  }
};

/* Battery group structure */
static attsGroup_t svcBattGroup =
{
  NULL,
  (attsAttr_t *) battList,
  NULL,
  NULL,
  BATT_START_HDL,
  BATT_END_HDL
};

/*************************************************************************************************/
/*!
 *  \fn     SvcBattAddGroup
 *        
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBattAddGroup(void)
{
  AttsAddGroup(&svcBattGroup);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcBattRemoveGroup
 *        
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBattRemoveGroup(void)
{
  AttsRemoveGroup(BATT_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \fn     SvcBattCbackRegister
 *        
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBattCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  svcBattGroup.readCback = readCback;
  svcBattGroup.writeCback = writeCback;
}

/*************************************************************************************************/
/*!
 *  \fn     SvcBattAddGroupDyn
 *
 *  \brief  Add the battery service using the dynamic attribute subsystem.
 *
 *  \return None.
 */
/*************************************************************************************************/
//void *SvcBattAddGroupDyn()
//{
//  void *pSHdl;
//  uint8_t initCcc[] = {UINT16_TO_BYTES(0x0000)};
//  uint8_t initBatVal[] = {0};


//  /* Create the service */
//  pSHdl = AttsDynCreateGroup(BATT_START_HDL, BATT_END_HDL);

//  /* Primary service */
//  AttsDynAddAttrConst(pSHdl, attPrimSvcUuid, battValSvc, sizeof(battValSvc), 0, ATTS_PERMIT_READ);

//  /* Battery level characteristic */
//  AttsDynAddAttrConst(pSHdl, attChUuid, battValLvlCh, sizeof(battValLvlCh), 0, ATTS_PERMIT_READ);

//  /* Battery level value */
//  AttsDynAddAttr(pSHdl, attBlChUuid, initBatVal, sizeof(uint8_t), sizeof(uint8_t),
//                      ATTS_SET_READ_CBACK, ATTS_PERMIT_READ);

//  /* Battery level CCC descriptor */
//  AttsDynAddAttr(pSHdl, attCliChCfgUuid, initCcc, sizeof(uint16_t), sizeof(uint16_t),
//                      ATTS_SET_CCC, ATTS_PERMIT_READ | ATTS_PERMIT_WRITE);

//  return pSHdl;
//}
