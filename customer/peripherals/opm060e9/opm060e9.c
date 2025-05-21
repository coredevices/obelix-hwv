/**
  ******************************************************************************
  * @file   opm060e9.c
  * @author Sifli software development team
  * @brief   This file includes the LCD driver for opm060e9 LCD.
  * @attention
  ******************************************************************************
*/
/**
 * @attention
 * Copyright (c) 2019 - 2022,  Sifli Technology
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
#include "rtthread.h"
#include "string.h"
#include "board.h"
#include "drv_lcd.h"

#include "epd_pin_defs.h"
#include "epd_wave_tables.h"
#include "epd_tps.h"
#include "mem_section.h"

#define  DBG_LEVEL            DBG_INFO  //DBG_LOG //

#define LOG_TAG                "opm060e9"
#include "log.h"

#define LCD_ID                  0x85


#define DISPLAY_LINE_CLOCKS   (LCD_HOR_RES_MAX/4)     //每列刷新所需次数，362*4像素
#define DISPLAY_ROWS   LCD_VER_RES_MAX
#define EPD_ARRAY  LCD_HOR_RES_MAX*LCD_VER_RES_MAX/8   // 全屏所需字节量



static LCDC_InitTypeDef lcdc_int_cfg =
{
    .lcd_itf = LCDC_INTF_DBI_8BIT_B,
    .freq = 24 * 1000 * 1000,
    .color_mode = LCDC_PIXEL_FORMAT_RGB332,

    .cfg = {
        .dbi = {
            .syn_mode = HAL_LCDC_SYNC_DISABLE,
            .vsyn_polarity = 0,
            .vsyn_delay_us = 0,
            .hsyn_num = 0,
        },
    },
};

static uint8_t  lcdc_input_idx = 0;
static uint8_t  lcdc_input_buffer[2][DISPLAY_LINE_CLOCKS];
#ifdef MIXED_REFRESH_METHODS
    /*
    Define a 4bpp grey framebuffer on PSRAM to save previous frame
    */
    L2_NON_RET_BSS_SECT_BEGIN(frambuf)
    L2_NON_RET_BSS_SECT(frambuf, ALIGN(4) static uint8_t previous_frame[LCD_HOR_RES_MAX * LCD_VER_RES_MAX / 2];);
    L2_NON_RET_BSS_SECT_END
#endif
/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
static void LCD_Init(LCDC_HandleTypeDef *hlcdc)
{
    uint8_t   parameter[14];

    /* Initialize LCD low level bus layer ----------------------------------*/
    memcpy(&hlcdc->Init, &lcdc_int_cfg, sizeof(LCDC_InitTypeDef));
    HAL_LCDC_Init(hlcdc);

    //Initialize power supply chip
    oedtps_init();

    hlcdc->Instance->LAYER0_CONFIG = (4   << LCD_IF_LAYER0_CONFIG_FORMAT_Pos) |       //RGB332
                                     (1   << LCD_IF_LAYER0_CONFIG_ALPHA_SEL_Pos) |     // use layer alpha
                                     (255 << LCD_IF_LAYER0_CONFIG_ALPHA_Pos) |         // layer alpha value is 255
                                     (0   << LCD_IF_LAYER0_CONFIG_FILTER_EN_Pos) |     // disable filter
                                     (DISPLAY_LINE_CLOCKS << LCD_IF_LAYER0_CONFIG_WIDTH_Pos) |         // layer line width
                                     (0   << LCD_IF_LAYER0_CONFIG_PREFETCH_EN_Pos) |   // prefetch enable
                                     (1   << LCD_IF_LAYER0_CONFIG_ACTIVE_Pos);         // layer is active

    hlcdc->Instance->LAYER0_TL_POS = (0  << LCD_IF_LAYER0_TL_POS_X0_Pos);
    hlcdc->Instance->LAYER0_BR_POS = ((DISPLAY_LINE_CLOCKS - 1) << LCD_IF_LAYER0_BR_POS_X1_Pos);
    // canvas area
    hlcdc->Instance->CANVAS_TL_POS = (0 << LCD_IF_CANVAS_TL_POS_X0_Pos);
    hlcdc->Instance->CANVAS_BR_POS = ((DISPLAY_LINE_CLOCKS - 1) << LCD_IF_CANVAS_BR_POS_X1_Pos);

    hlcdc->Instance->LCD_WR = 0;

    oedtps_vcom_disable();
    oedtps_source_gate_disable();

    EPD_LE_L_hs();
    EPD_CLK_L_hs();
    EPD_OE_L_hs();
    EPD_SPH_H_hs();
    EPD_STV_H_hs();
    EPD_CPV_L_hs();
    EPD_GMODE_H_hs();

    epd_wave_table();
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval LCD Register Value.
  */
static uint32_t LCD_ReadID(LCDC_HandleTypeDef *hlcdc)
{
    return LCD_ID;
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
static void LCD_DisplayOn(LCDC_HandleTypeDef *hlcdc)
{
    /* Display On */
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
static void LCD_DisplayOff(LCDC_HandleTypeDef *hlcdc)
{
    /* Display Off */
}

static void LCD_SetRegion(LCDC_HandleTypeDef *hlcdc, uint16_t Xpos0, uint16_t Ypos0, uint16_t Xpos1, uint16_t Ypos1)
{

}

static uint32_t wait_lcd_ticks;

L1_RET_CODE_SECT(epd_codes, void epd_load_and_send_pic1(LCDC_HandleTypeDef *hlcdc, const uint8_t *old_pic_a1, const uint8_t *new_pic_a1, uint8_t frame))
{

    uint8_t *p_lcdc_input = (uint8_t *) &lcdc_input_buffer[lcdc_input_idx][0];


    epd_wave_table_convert_i1o2(p_lcdc_input, old_pic_a1, new_pic_a1, DISPLAY_LINE_CLOCKS / 2, frame);

    //Wait previous LCDC done.
    uint32_t start_tick = HAL_DBG_DWT_GetCycles();
    while (hlcdc->Instance->STATUS & LCD_IF_STATUS_LCD_BUSY) {;}
    wait_lcd_ticks += HAL_GetElapsedTick(start_tick, HAL_DBG_DWT_GetCycles());


    EPD_CPV_L_hs();
    EPD_OE_L_hs();
    EPD_LE_H_hs();
    HAL_Delay_us(1);
    EPD_LE_L_hs();
    HAL_Delay_us(1);
    EPD_OE_H_hs();
    EPD_CPV_H_hs();



    hlcdc->Instance->LCD_SINGLE = LCD_IF_LCD_SINGLE_WR_TRIG;
    while (hlcdc->Instance->LCD_SINGLE & LCD_IF_LCD_SINGLE_LCD_BUSY) {;}

    hlcdc->Instance->LAYER0_SRC = (uint32_t)p_lcdc_input;
    hlcdc->Instance->COMMAND = 0x1;

    lcdc_input_idx = !lcdc_input_idx;
}



L1_RET_CODE_SECT(epd_codes, static void LCD_WriteMultiplePixels(LCDC_HandleTypeDef *hlcdc, const uint8_t *RGBCode, uint16_t Xpos0, uint16_t Ypos0, uint16_t Xpos1, uint16_t Ypos1))
{
    unsigned char frame;
    unsigned int line;
    unsigned char *ptrnext;
    //波形帧数量，用于局刷和全刷控制
    unsigned int frame_times = 0;
#ifdef MIXED_REFRESH_METHODS
    uint8_t *old_ptr = (uint8_t *)previous_frame;
#else
    uint8_t *old_ptr = NULL;
#endif /* MIXED_REFRESH_METHODS */

    // __disable_irq();
    uint32_t start_tick = rt_tick_get();
    oedtps_source_gate_enable();
    LCD_DRIVER_DELAY_MS(50);
    oedtps_vcom_enable();
    LCD_DRIVER_DELAY_MS(10);



    LOG_I("LCD_WriteMultiplePixels ColorMode=%d", hlcdc->Layer[HAL_LCDC_LAYER_DEFAULT].data_format);

    frame_times = epd_wave_table_start_flush();
    wait_lcd_ticks = 0;
    EPD_GMODE_H_hs();
    for (frame = 0; frame < frame_times; frame++)
    {
        EPD_STV_H_hs();
        EPD_STV_L_hs();
        HAL_Delay_us(1);
        EPD_CPV_L_hs();    //DCLK跑1个时钟
        HAL_Delay_us(1);
        EPD_CPV_H_hs();
        HAL_Delay_us(1);
        EPD_STV_H_hs();
        HAL_Delay_us(1);
        EPD_CPV_L_hs();    //DCLK跑1个时钟
        HAL_Delay_us(1);
        EPD_CPV_H_hs();
        HAL_Delay_us(1);
        EPD_CPV_L_hs();    //DCLK跑1个时钟
        HAL_Delay_us(1);
        EPD_CPV_H_hs();
        HAL_Delay_us(1);
        EPD_CPV_L_hs();
        EPD_LE_H_hs();
        HAL_Delay_us(1);
        EPD_LE_L_hs();
        HAL_Delay_us(1);
        EPD_OE_H_hs();
        EPD_CPV_H_hs();


        for (line = 0; line < DISPLAY_ROWS; line++)                 //共有DISPLAY_ROWS列数据
        {
            epd_load_and_send_pic1(hlcdc, old_ptr + (line * DISPLAY_LINE_CLOCKS / 2), RGBCode + (line * DISPLAY_LINE_CLOCKS / 2), frame); //(line*DISPLAY_LINE_CLOCKS/2)传完一列数据后传下一列，一列数据有
        }
        epd_load_and_send_pic1(hlcdc, old_ptr + ((line - 1) * DISPLAY_LINE_CLOCKS / 2), RGBCode + ((line - 1) * DISPLAY_LINE_CLOCKS / 2), frame); //最后一行还需GATE CLK,故再传一行没用数据

        //Wait previous LCDC done.
        while (hlcdc->Instance->STATUS & LCD_IF_STATUS_LCD_BUSY) {;}

        EPD_CPV_L_hs();
        HAL_Delay_us(1);
        EPD_OE_L_hs();
    }
    EPD_GMODE_L_hs();
    EPD_LE_L_hs();
    EPD_CLK_L_hs();
    EPD_OE_L_hs();
    EPD_SPH_H_hs();
    EPD_STV_H_hs();
    EPD_CPV_L_hs();

    LCD_DRIVER_DELAY_MS(10);
    oedtps_vcom_disable();
    LCD_DRIVER_DELAY_MS(10);
    oedtps_source_gate_disable();
#ifdef MIXED_REFRESH_METHODS
    extern void *sifli_memcpy(void *dst, const void *src, rt_ubase_t count);
    sifli_memcpy(old_ptr, RGBCode, EPD_ARRAY);
#endif /* MIXED_REFRESH_METHODS */
    LOG_I("Cost time=%d wait_lcd=%d(us)\r\n", rt_tick_get() - start_tick, wait_lcd_ticks / 240);
    // __enable_irq();

    /* Simulate LCDC IRQ handler, call user callback */
    if (hlcdc->XferCpltCallback)
    {
        hlcdc->XferCpltCallback(hlcdc);
    }

    HAL_LCDC_SendLayerDataCpltCbk(hlcdc);
}


static void LCD_SetColorMode(LCDC_HandleTypeDef *hlcdc, uint16_t color_mode)
{

}

static void LCD_SetBrightness(LCDC_HandleTypeDef *hlcdc, uint8_t br)
{
    rt_device_t device = rt_device_find("lcdlight");
    if (device)
    {
        rt_err_t err = rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
        uint8_t val = br;
        rt_device_write(device, 0, &val, 1);
        rt_device_close(device);
    }
    else
    {
        LOG_E("Can't find device lcdlight!");
    }
}


static const LCD_DrvOpsDef LCD_drv =
{
    LCD_Init,
    LCD_ReadID,
    LCD_DisplayOn,
    LCD_DisplayOff,

    LCD_SetRegion,
    NULL,
    LCD_WriteMultiplePixels,

    NULL,

    LCD_SetColorMode,
    LCD_SetBrightness,
    NULL,
    NULL
};

LCD_DRIVER_EXPORT2(opm060e9, LCD_ID, &lcdc_int_cfg, &LCD_drv, 1);
/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
