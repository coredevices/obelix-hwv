/**
  ******************************************************************************
  * @file   lcpu_config_type.h
  * @author Sifli software development team
  ******************************************************************************
*/
/**
 * @attention
 * Copyright (c) 2025 - 2025,  Sifli Technology
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Sifli integrated circuit
 *    in a product or a software update for such product, must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Sifli nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Sifli integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY SIFLI TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SIFLI TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#ifndef __LCPU_CONFIG_TYPE_H
#define __LCPU_CONFIG_TYPE_H

#include "lcpu_config_type_int.h"

/**
  * @brief  ENUM definition of configration type.
*/


typedef enum
{
    HAL_LCPU_CONFIG_XTAL_ENABLED = 0,      /*!< ADC calibration value. */
    HAL_LCPU_CONFIG_LPCYCLE_CURR = 1,             /*!< Chip revision. */
    HAL_LCPU_CONFIG_LPCYCLE_AVE = 2,  /*!< Battery calibration value. */
    HAL_LCPU_CONFIG_WDT_TIME = 3,          /*!< SDADC calibration value. */
    HAL_LCPU_CONFIG_WDT_STATUS = 4,  /*!< Battery calibration value. */
    HAL_LCPU_CONFIG_WDT_CLK_FEQ = 5,
    HAL_LCPU_CONFIG_BT_TX_PWR = 6,  /*!< Battery calibration value. */
    HAL_LCPU_CONFIG_BT_RC_CAL_IN_L = 7,   /*!< Enable RCCal for BT in LCPU. */
    HAL_LCPU_CONFIG_SOFT_CVSD = 8,        /*!< Enable soft cvsd encode decode for BT in LCPU. */
    // Following type only support after revID >= 4
    HAL_LCPU_CONFIG_BT_EM_BUF = 9,  /*!< BT custormized EM buffer. */
    HAL_LCPU_CONFIG_BT_ACT_CFG = 10,  /*!< BT custormized link related config. */
    HAL_LCPU_CONFIG_BT_CONFIG = 11,  /*!< BT custormized other config. */
    HAL_LCPU_CONFIG_BT_KE_BUF = 12,  /*!< BT custormized KE buffer. */
    HAL_LCPU_CONFIG_SEC_ADDR = 13, /*!< Security protection address. */
    HAL_LCPU_CONFIG_HCPU_TX_QUEUE = 14, /*!< TX buffer of HCPU IPC queue. */
    HAL_LCPU_CONFIG_ADC_CALIBRATION = 15,      /*!< ADC calibration value. */
    HAL_LCPU_CONFIG_SDADC_CALIBRATION = 16,    /*!< SDADC calibration value. */
    HAL_LCPU_CONFIG_SN = 17,                   /*!< mcu serial number. */
    HAL_LCPU_CONFIG_CHIP_REV = 18,             /*!< Chip revision. */
    HAL_LCPU_CONFIG_BATTERY_CALIBRATION = 19,  /*!< Battery calibration value. */
    HAL_LCPU_CONFIG_BT_ACTMOVE_CONFIG = 20,  /*!< BT custormized activity move config. */
    HAL_LCPU_CONFIG_MAX = 0xFE,
} HAL_LCPU_CONFIG_TYPE_T;


uint16_t LCPU_CONFIG_get_total_size(void);
uint8_t LCPU_CONFIG_set(uint8_t *base_addr, uint8_t config_type, uint8_t *value, uint16_t length);
uint8_t LCPU_CONFIG_get(uint8_t *base_addr, uint8_t config_type, uint8_t *value, uint16_t *length);


#endif // __LCPU_CONFIG_TYPE_H

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
