/**
  ******************************************************************************
  * @file   lcpu_config_type_int.h
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


#ifndef __LCPU_CONFIG_TYPE_INT_H
#define __LCPU_CONFIG_TYPE_INT_H

/*
typedef struct
{
    uint32_t lpcycle_curr;
    uint32_t lpcycle_ave;
    uint32_t wdt_time;
    uint32_t wdt_status;
    uint32_t bt_txpwr;
    uint16_t wdt_clk;
    uint8_t is_xtal_enable;
    uint8_t is_rccal_in_L;
    uint32_t is_soft_cvsd;//u32 for magic
    hal_lcpu_bluetooth_em_config_t em_buf;
    hal_lcpu_bluetooth_act_configt_t bt_act_config;
    hal_lcpu_ble_mem_config_t ke_mem_config;
    hal_lcpu_bluetooth_rom_config_t bt_rom_config;
    uint32_t sec_addr;
    uint32_t hcpu_ipc_addr;

} LCPU_CONFIG_TYPE_T;
*/

#define LCPU_CONFIG_START_ADDR 0x2040FDC0
#define LPCU_CONFIG_MAGIC_NUM 0x45457878

#define LCPU_ASSERT_INFO_ADDR  0x2040FDBC
#define LPCU_ASSERT_OVER_FLAG  0xa5a5a5a5

#define LCPU_CONFIG_ROM_SIZE 0x40
#define LCPU_CONFIG_ROM_A4_SIZE 0xCC


#define LCPU_CONFIG_LPCYCLE_CURR_ROM_OFFSET 4
#define LCPU_CONFIG_LPCYCLE_CURR_ROM_LENGTH 4

#define LCPU_CONFIG_LPCYCLE_AVE_ROM_OFFSET 8
#define LCPU_CONFIG_LPCYCLE_AVE_ROM_LENGTH 4

#define LCPU_CONFIG_WDT_TIME_ROM_OFFSET 12
#define LCPU_CONFIG_WDT_TIME_ROM_LENGTH 4

#define LCPU_CONFIG_WDT_STATUS_ROM_OFFSET 16
#define LCPU_CONFIG_WDT_STATUS_ROM_LENGTH 4

#define LCPU_CONFIG_BT_TXPWR_ROM_OFFSET 20
#define LCPU_CONFIG_BT_TXPWR_ROM_LENGTH 4

#define LCPU_CONFIG_WDT_CLK_ROM_OFFSET 24
#define LCPU_CONFIG_WDT_CLK_ROM_LENGTH 2

#define LCPU_CONFIG_IS_XTAL_ENABLE_ROM_OFFSET 26
#define LCPU_CONFIG_IS_XTAL_ENABLE_ROM_LENGTH 1

#define LCPU_CONFIG_IS_RCCAL_IN_L_ROM_OFFSET 27
#define LCPU_CONFIG_IS_RCCAL_IN_L_ROM_LENGTH 1

#define LCPU_CONFIG_IS_SOFT_CVSD_ROM_OFFSET 28
#define LCPU_CONFIG_IS_SOFT_CVSD_ROM_LENGTH 4

#define LCPU_CONFIG_EM_BUF_ROM_OFFSET 32
#define LCPU_CONFIG_EM_BUF_ROM_LENGTH 82

#define LCPU_CONFIG_BT_ACT_CONFIG_ROM_OFFSET 116
#define LCPU_CONFIG_BT_ACT_CONFIG_ROM_LENGTH 12

#define LCPU_CONFIG_KE_MEM_CONFIG_ROM_OFFSET 128
#define LCPU_CONFIG_KE_MEM_CONFIG_ROM_LENGTH 44

#define LCPU_CONFIG_BT_ROM_CONFIG_ROM_OFFSET 172
#define LCPU_CONFIG_BT_ROM_CONFIG_ROM_LENGTH 24

#define LCPU_CONFIG_SEC_ADDR_ROM_OFFSET 196
#define LCPU_CONFIG_SEC_ADDR_ROM_LENGTH 4

#define LCPU_CONFIG_HCPU_IPC_ADDR_OFFSET 200
#define LCPU_CONFIG_HCPU_IPC_ADDR_LENGTH 4

#endif // __LCPU_CONFIG_TYPE_INT_H

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
