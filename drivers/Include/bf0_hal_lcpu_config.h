/**
  ******************************************************************************
  * @file   bf0_hal_lcpu_config.h
  * @author Sifli software development team
  * @brief Header file of configure parameters to LCPU
  * @{
  ******************************************************************************
*/
/**
 * @attention
 * Copyright (c) 2021 - 2021,  Sifli Technology
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


#ifndef __BF0_HAL_LCPU_CONFIG_H
#define __BF0_HAL_LCPU_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "bf0_hal_def.h"
#include "lcpu_config_type.h"

/** @addtogroup BF0_HAL_Driver
  * @{
  */


/** @defgroup LPCONFIG LCPU configure
  * @brief LCOU configure to share parameter with HCPU
  * @{
  */


/**
  * @addtogroup  LPCONFIG_exported_constants
  * @{
*/

#define HAL_LCPU_CONFIG_SN_MAX_NUM 128


/**
  * @brief  Structure definition of @ref HAL_LCPU_CONFIG_ADC_CALIBRATION for LCPU_CONFIG_V1.
*/
typedef struct
{
    uint16_t vol10;     /*!< Reg value for low voltage. */
    uint16_t vol25;      /*!< Reg value for high voltage. */
#ifndef SF32LB55X
    uint16_t low_mv;     /*!< voltage for low with mv . */
    uint16_t high_mv;    /*!< voltage for high with mv. */
#endif
} HAL_LCPU_CONFIG_ADC_T;


/**
  * @brief  Structure definition of @ref HAL_LCPU_CONFIG_ADC_CALIBRATION for LCPU_CONFIG_V2.
*/
typedef struct
{
    uint16_t vol10;     /*!< Reg value for low voltage. */
    uint16_t vol25;      /*!< Reg value for high voltage. */
    uint16_t low_mv;     /*!< voltage for low with mv . */
    uint16_t high_mv;    /*!< voltage for high with mv. */
} HAL_LCPU_CONFIG_ADC_V2_T;


/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_SDADC_CALIBRATION for LCPU_CONFIG_V1.
*/
typedef struct
{
    uint32_t vol_mv;
    uint32_t value;
} HAL_LCPU_CONFIG_SDMADC_T;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_SDADC_CALIBRATION for LCPU_CONFIG_V2.
*/
typedef struct
{
    uint32_t vol_mv;
    uint32_t value;
} HAL_LCPU_CONFIG_SDMADC_V2_T;


/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_SN for LCPU_CONFIG_V1.
*/
typedef struct
{
    uint8_t sn[HAL_LCPU_CONFIG_SN_MAX_NUM];
} HAL_LCPU_CONFIG_SN_T;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_SN for LCPU_CONFIG_V2.
*/
typedef struct
{
    uint16_t sn_len;
    uint8_t sn[HAL_LCPU_CONFIG_SN_MAX_NUM];
} HAL_LCPU_CONFIG_SN_V2_T;



/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BATTERY_CALIBRATION for LCPU_CONFIG_V1.  ax+b   a*10000 for integer
*/
typedef struct
{
    int magic;
    uint32_t a;
    int32_t b;
} HAL_LCPU_CONFIG_BATTERY_T;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BATTERY_CALIBRATION for LCPU_CONFIG_V2.  ax+b   a*10000 for integer
*/
typedef struct
{
    int magic;
    uint32_t a;
    int32_t b;
} HAL_LCPU_CONFIG_BATTERY_V2_T;



#define HAL_LCPU_CONFIG_EM_BUF_MAX_NUM 40

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BT_EM_BUF.
*/
typedef struct
{
    uint8_t is_valid;
    uint16_t em_buf[HAL_LCPU_CONFIG_EM_BUF_MAX_NUM];
} hal_lcpu_bluetooth_em_config_t;


/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BT_ACT_CFG.
*/
typedef struct
{
    // max valid bit is 7
    uint32_t bit_valid;
    uint8_t bt_max_acl;
    uint8_t bt_max_sco;
    uint8_t ble_max_act;
    uint8_t ble_max_ral;
    uint8_t ble_max_iso;
    uint8_t ble_rx_desc;
    uint8_t bt_rx_desc;
    uint8_t bt_name_len;
} hal_lcpu_bluetooth_act_configt_t;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BT_CONFIG.
*/
typedef struct
{
    // max valid bit is 11
    uint32_t bit_valid;
    uint32_t max_sleep_time;
    // bit 0: BLE, 1: BT
    uint8_t controller_enable_bit;
    uint8_t lld_prog_delay;
    uint8_t lld_prog_delay_min;
    uint8_t default_sleep_mode;
    uint8_t default_sleep_enabled;
    uint8_t default_xtal_enabled;
    uint8_t default_rc_cycle;
    uint8_t default_swprofiling_cfg;
    // 0: boot to rom; 1: boot to ram; 2. debug boot
    uint8_t boot_mode;
    // 1: fpga, others asic
    uint8_t is_fpga;
    uint8_t en_inq_filter;
    uint8_t support_3m;
    uint8_t sco_cfg;
} hal_lcpu_bluetooth_rom_config_t;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BT_KE_BUF.
*/
typedef struct
{
    //  max valid bit is 5, buf and size should be aligned.
    uint32_t bit_valid;
    uint8_t *env_buf;
    uint8_t *msg_buf;
    uint8_t *nt_buf;
    uint8_t *log_buf;
    uint8_t *nvds_buf;
    uint8_t *aud_buf;
    uint16_t env_buf_size;
    uint16_t msg_buf_size;
    uint16_t nt_buf_size;
    uint16_t log_buf_size;
    uint16_t nvds_buf_size;
    uint16_t aud_buf_size;
    int8_t max_nb_of_hci_completed;
} hal_lcpu_ble_mem_config_t;

/**
  * @brief  Structure definition of #HAL_LCPU_CONFIG_BT_ACTMOVE_CONFIG.
*/
typedef struct
{
    //  max valid bit is 5, buf and size should be aligned.
    uint8_t bit_valid;
    uint8_t act_mov;
    uint8_t sco_pingpong;

} hal_lcpu_bluetooth_actmove_config_t;

/**
  * @} LPCONFIG_exported_constants
*/


/**
  * @defgroup LPCONFIG_exported_functions LCPU configure Exported functions
  * @ingroup LPCONFIG
  * @{
  *
 */



/**
* @brief  Set configure parameter, only support in HCPU
* @param type Parameter type
* @param value Parameter values
* @param length Parameter length
* @retval HAL_OK if successful, otherwise error
*/
HAL_StatusTypeDef HAL_LCPU_CONFIG_set(HAL_LCPU_CONFIG_TYPE_T type, void *value, uint16_t length);

/**
* @brief  Get configure parameter
* @param type Parameter type
* @param value Parameter values
* @param length Parameter length
* @retval HAL_OK if successful, otherwise error
*/
HAL_StatusTypeDef HAL_LCPU_CONFIG_get(HAL_LCPU_CONFIG_TYPE_T type, uint8_t *value, uint16_t *length);

/**
* @brief Force init context without check
* @return void
*/
void HAL_LCPU_CONFIG_InitContext(void);



/**
  *@} LPCONFIG_exported_functions
*/


/**
  *@} LPCONFIG
  */


/**
  *@} BF0_HAL_Driver
  */



#ifdef __cplusplus
}
#endif


#endif // __BF0_HAL_LCPU_CONFIG_H

/**
  *@}
  */

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/

