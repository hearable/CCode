/*************************************************************************************************/
/*!
 *  \file   cfg_stack.c
 *
 *  \brief  Stack configuration.
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

#include "wsf_types.h"
#include "cfg_stack.h"
#include "hci_api.h"
#include "dm_api.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"

/**************************************************************************************************
  HCI
**************************************************************************************************/

/**************************************************************************************************
  DM
**************************************************************************************************/

/* Configuration structure */
const dmCfg_t dmCfg =
{
  0
};

/* Configuration pointer */
dmCfg_t *pDmCfg = (dmCfg_t *) &dmCfg;

/**************************************************************************************************
  L2C
**************************************************************************************************/

/* Configuration structure */
const l2cCfg_t l2cCfg =
{
  30                                /* Request timeout in seconds */
};

/* Configuration pointer */
l2cCfg_t *pL2cCfg = (l2cCfg_t *) &l2cCfg;

/**************************************************************************************************
  ATT
**************************************************************************************************/

/* Configuration structure */
const attCfg_t attCfg =
{
  15,                               /* ATT server service discovery connection idle timeout in seconds */
  ATT_DEFAULT_MTU,                  /* desired ATT MTU */
  ATT_MAX_TRANS_TIMEOUT,            /* transcation timeout in seconds */
  4                                 /* number of queued prepare writes supported by server */
};

/* Configuration pointer */
attCfg_t *pAttCfg = (attCfg_t *) &attCfg;

/**************************************************************************************************
  SMP
**************************************************************************************************/

/* Configuration structure */
const smpCfg_t smpCfg =
{
  3000,                             /* 'Repeated attempts' timeout in msec */
  SMP_IO_NO_IN_NO_OUT,              /* I/O Capability */
  7,                                /* Minimum encryption key length */
  16,                               /* Maximum encryption key length */
  3,                                /* Attempts to trigger 'repeated attempts' timeout */
  0                                 /* Device authentication requirements */
};

/* Configuration pointer */
smpCfg_t *pSmpCfg = (smpCfg_t *) &smpCfg;
