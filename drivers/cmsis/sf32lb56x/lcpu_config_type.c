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


uint8_t LCPU_CONFIG_set(uint8_t *base_addr, uint8_t conifg_type, uint8_t *value, uint16_t length)
{
    uint8_t ret = 1;
    switch (conifg_type)
    {
    case HAL_LCPU_CONFIG_ADC_CALIBRATION_ROM:
    {
        if (length == LCPU_CONFIG_ADC_CALIBRATION_ROM_LENGTH) // 32 bit
        {
            memcpy(base_addr + LCPU_CONFIG_ADC_CALIBRATION_ROM_OFFSET, value, LCPU_CONFIG_ADC_CALIBRATION_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_SDADC_CALIBRATION_ROM:
    {
        if (length == LCPU_CONFIG_SDADC_CALIBRATION_ROM_LENGTH) // 64 bit
        {
            memcpy(base_addr + LCPU_CONFIG_SDADC_CALIBRATION_ROM_OFFSET, value, LCPU_CONFIG_SDADC_CALIBRATION_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_SN_ROM:
    {
        if (length <= LCPU_CONFIG_SN_ROM_LENGTH)
        {
            memcpy(base_addr + LCPU_CONFIG_SN_ROM_OFFSET, value, length);
            memcpy(base_addr + LCPU_CONFIG_SN_LEN_ROM_OFFSET, &length, LCPU_CONFIG_SN_LEN_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_CHIP_REV_ROM:
    {
        if (length == LCPU_CONFIG_CHIP_REV_ROM_LENGTH)
        {
            memcpy(base_addr + LCPU_CONFIG_CHIP_REV_ROM_OFFSET, value, length);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_BATTERY_CALIBRATION_ROM:
    {
        if (length == 8)
        {
            memcpy(base_addr + LCPU_CONFIG_BATTERY_A_ROM_OFFSET, value, length);
            ret = 0;
        }
        else if (length == 12) //maybe 12 bytes
        {
            memcpy(base_addr + LCPU_CONFIG_BATTERY_A_ROM_OFFSET, value + 4, length - 4);
            ret = 0;
        }
        break;
    }
    default:
        break;
    }

    return ret;
}



uint8_t LCPU_CONFIG_get(uint8_t *base_addr, uint8_t config_type, uint8_t *value, uint16_t *length)
{
    uint8_t ret = 1;
    switch (config_type)
    {
    case HAL_LCPU_CONFIG_ADC_CALIBRATION_ROM:
    {
        if (*length == LCPU_CONFIG_ADC_CALIBRATION_ROM_LENGTH) // 32 bit
        {
            memcpy((void *)value, base_addr + LCPU_CONFIG_ADC_CALIBRATION_ROM_OFFSET, LCPU_CONFIG_ADC_CALIBRATION_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_SDADC_CALIBRATION_ROM:
    {
        if (*length == LCPU_CONFIG_SDADC_CALIBRATION_ROM_LENGTH) // 64 bit
        {
            memcpy((void *)value, base_addr + LCPU_CONFIG_SDADC_CALIBRATION_ROM_OFFSET, LCPU_CONFIG_SDADC_CALIBRATION_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_SN_ROM:
    {
        uint16_t sn_len;
        memcpy(&sn_len, base_addr + LCPU_CONFIG_SN_LEN_ROM_OFFSET, LCPU_CONFIG_SN_LEN_ROM_LENGTH);
        if (*length >= sn_len)
        {
            memcpy(value, base_addr + LCPU_CONFIG_SN_ROM_OFFSET, sn_len);
            *length = sn_len;
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_CHIP_REV_ROM:
    {
        if (*length == LCPU_CONFIG_CHIP_REV_ROM_LENGTH)
        {
            memcpy(value, base_addr + LCPU_CONFIG_CHIP_REV_ROM_OFFSET, LCPU_CONFIG_CHIP_REV_ROM_LENGTH);
            ret = 0;
        }
        break;
    }
    case HAL_LCPU_CONFIG_BATTERY_CALIBRATION_ROM:
    {
        if (*length == 8)
        {
            memcpy(value, base_addr + LCPU_CONFIG_BATTERY_A_ROM_OFFSET, 8);
            ret = 0;
        }
        else if (*length == 12)
        {
            *((uint32_t *)value) = 0xe8091ad7;
            memcpy(value + 4, base_addr + LCPU_CONFIG_BATTERY_A_ROM_OFFSET, 8);
            ret = 0;
        }
        break;
    }
    default:
        break;
    }
    return ret;

}


uint16_t LCPU_CONFIG_get_total_size(void)
{
    uint16_t size = LCPU_CONFIG_ROM_SIZE;
    return size;
}


/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
