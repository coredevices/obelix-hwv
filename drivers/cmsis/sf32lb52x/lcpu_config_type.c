/**
  ******************************************************************************
  * @file   lcpu_config_type.c
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

#include <stdlib.h>
#include <string.h>

#include "bf0_hal.h"
#include "lcpu_config_type.h"

typedef struct
{
    uint8_t type;
    uint16_t offset;
    uint16_t len;
} lcpu_rom_type_array_t;


const lcpu_rom_type_array_t rom_array[9] =
{
    {HAL_LCPU_CONFIG_XTAL_ENABLED, LCPU_CONFIG_IS_XTAL_ENABLE_ROM_OFFSET, LCPU_CONFIG_IS_XTAL_ENABLE_ROM_LENGTH},
    {HAL_LCPU_CONFIG_LPCYCLE_CURR, LCPU_CONFIG_LPCYCLE_CURR_ROM_OFFSET, LCPU_CONFIG_LPCYCLE_CURR_ROM_LENGTH},
    {HAL_LCPU_CONFIG_LPCYCLE_AVE, LCPU_CONFIG_LPCYCLE_AVE_ROM_OFFSET, LCPU_CONFIG_LPCYCLE_AVE_ROM_LENGTH},
    {HAL_LCPU_CONFIG_WDT_TIME, LCPU_CONFIG_WDT_TIME_ROM_OFFSET, LCPU_CONFIG_WDT_TIME_ROM_LENGTH},
    {HAL_LCPU_CONFIG_WDT_STATUS, LCPU_CONFIG_WDT_STATUS_ROM_OFFSET, LCPU_CONFIG_WDT_STATUS_ROM_LENGTH},
    {HAL_LCPU_CONFIG_WDT_CLK_FEQ, LCPU_CONFIG_WDT_CLK_ROM_OFFSET, LCPU_CONFIG_WDT_CLK_ROM_LENGTH},
    {HAL_LCPU_CONFIG_BT_TX_PWR, LCPU_CONFIG_BT_TXPWR_ROM_OFFSET, LCPU_CONFIG_BT_TXPWR_ROM_LENGTH},
    {HAL_LCPU_CONFIG_BT_RC_CAL_IN_L, LCPU_CONFIG_IS_RCCAL_IN_L_ROM_OFFSET, LCPU_CONFIG_IS_RCCAL_IN_L_ROM_LENGTH},
    {HAL_LCPU_CONFIG_SOFT_CVSD, LCPU_CONFIG_IS_SOFT_CVSD_ROM_OFFSET, LCPU_CONFIG_IS_SOFT_CVSD_ROM_LENGTH},
};


const lcpu_rom_type_array_t rom_array_a4[6] =
{
    {HAL_LCPU_CONFIG_BT_EM_BUF, LCPU_CONFIG_EM_BUF_ROM_OFFSET, LCPU_CONFIG_EM_BUF_ROM_LENGTH},
    {HAL_LCPU_CONFIG_BT_ACT_CFG, LCPU_CONFIG_BT_ACT_CONFIG_ROM_OFFSET, LCPU_CONFIG_BT_ACT_CONFIG_ROM_LENGTH},
    {HAL_LCPU_CONFIG_BT_CONFIG, LCPU_CONFIG_BT_ROM_CONFIG_ROM_OFFSET, LCPU_CONFIG_BT_ROM_CONFIG_ROM_LENGTH},
    {HAL_LCPU_CONFIG_BT_KE_BUF, LCPU_CONFIG_KE_MEM_CONFIG_ROM_OFFSET, LCPU_CONFIG_KE_MEM_CONFIG_ROM_LENGTH},
    {HAL_LCPU_CONFIG_SEC_ADDR, LCPU_CONFIG_SEC_ADDR_ROM_OFFSET, LCPU_CONFIG_SEC_ADDR_ROM_LENGTH},
    {HAL_LCPU_CONFIG_HCPU_TX_QUEUE, LCPU_CONFIG_HCPU_IPC_ADDR_OFFSET, LCPU_CONFIG_HCPU_IPC_ADDR_LENGTH},
};


static lcpu_rom_type_array_t *lcpu_config_find_type(lcpu_rom_type_array_t *array, uint8_t array_len, uint8_t config_type, uint16_t length)
{
    for (uint32_t i = 0; i < array_len; i++)
    {
        if (array[i].type == config_type &&
                array[i].len == length)
        {
            return &array[i];
        }
    }
    return NULL;
}

static lcpu_rom_type_array_t *lcpu_config_find_array(uint8_t config_type, uint16_t length)
{
    lcpu_rom_type_array_t *array = lcpu_config_find_type((lcpu_rom_type_array_t *)&rom_array[0], sizeof(rom_array) / sizeof(lcpu_rom_type_array_t), config_type, length);
    if (!array)
    {
        if (__HAL_SYSCFG_GET_REVID() >= HAL_CHIP_REV_ID_A4)
        {
            array = lcpu_config_find_type((lcpu_rom_type_array_t *)&rom_array_a4[0], sizeof(rom_array_a4) / sizeof(lcpu_rom_type_array_t), config_type, length);
        }
    }

    return array;
}

uint8_t LCPU_CONFIG_set(uint8_t *base_addr, uint8_t config_type, uint8_t *value, uint16_t length)
{
    uint8_t ret = 1;
    lcpu_rom_type_array_t *array = lcpu_config_find_array(config_type, length);
    if (array)
    {
        memcpy(base_addr + array->offset, value, array->len);
        ret = 0;
    }

    return ret;
}


uint8_t LCPU_CONFIG_get(uint8_t *base_addr, uint8_t config_type, uint8_t *value, uint16_t *length)
{
    uint8_t ret = 1;
    lcpu_rom_type_array_t *array = lcpu_config_find_array(config_type, *length);
    if (array)
    {
        memcpy(value, base_addr + array->offset, array->len);
        ret = 0;
    }

    return ret;

}


uint16_t LCPU_CONFIG_get_total_size(void)
{
    uint16_t size = LCPU_CONFIG_ROM_SIZE;
    if (__HAL_SYSCFG_GET_REVID() >= HAL_CHIP_REV_ID_A4)
        size = LCPU_CONFIG_ROM_A4_SIZE;

    return size;
}



/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
