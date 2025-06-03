#include "rtthread.h"
#include "bf0_hal.h"
#include "mem_section.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"

#define DBG_TAG "drv_lcd"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LCD_PWR_EN	 IOEXP_CH03

#define FB_WIDTH  LCD_HOR_RES_MAX
#define FB_HEIGHT LCD_VER_RES_MAX

#if 0//def BSP_USING_PSRAM
    #define FB_COLOR_FORMAT RTGRAPHIC_PIXEL_FORMAT_RGB888
    #define FB_PIXEL_BYTES  3
#else
    #define FB_COLOR_FORMAT RTGRAPHIC_PIXEL_FORMAT_RGB565
    #define FB_PIXEL_BYTES  2
#endif
#define FB_TOTAL_BYTES (FB_WIDTH * FB_HEIGHT *FB_PIXEL_BYTES)
/*
    Define framebuffer for flushing LCD, using double framebuffer if LCD is ramless.
*/
//L2_NON_RET_BSS_SECT_BEGIN(frambuf)
///L2_NON_RET_BSS_SECT(frambuf, ALIGN(64) static uint8_t framebuffer1[FB_TOTAL_BYTES]);

//  #ifdef BSP_USING_RAMLESS_LCD
//      L2_NON_RET_BSS_SECT(frambuf, ALIGN(64) static uint8_t framebuffer2[FB_TOTAL_BYTES]);
//  #endif /* BSP_USING_RAMLESS_LCD */
//L2_NON_RET_BSS_SECT_END

static uint8_t framebuffer1[FB_TOTAL_BYTES];


static rt_device_t lcd_device = NULL;
static void lcd_refresh(uint8_t id);

static struct rt_semaphore lcd_sema;
static rt_err_t lcd_flush_done(rt_device_t dev, void *buffer)
{
    rt_sem_release(&lcd_sema);
    return RT_EOK;
}

void lcd_init(void)
{
	//1. pin configuration
	/*HAL_PIN_Set(PAD_PA02, LCDC1_JDI_B2, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA03, LCDC1_JDI_B1, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA04, LCDC1_JDI_G1, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA05, LCDC1_JDI_R1, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA06, LCDC1_JDI_HST, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA07, LCDC1_JDI_ENB, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA08, LCDC1_JDI_VST, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA39, LCDC1_JDI_VCK, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA40, LCDC1_JDI_XRST, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA41, LCDC1_JDI_HCK, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA42, LCDC1_JDI_R2, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA43, LCDC1_JDI_G2, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA24, GPIO_A24, PIN_NOPULL, 1);
    //HAL_PIN_Set(PAD_PA25, GPIO_A25, PIN_NOPULL, 1);*/

	//2. enable power
	ioexp_pin_set(LCD_PWR_EN, IOEXP_LOW);

	//3. init lcd ctrl
	lcd_device =  rt_device_find("lcd");
    if (lcd_device)
    {
        LOG_D("found lcd instance\n");
    }
    /*
    Open LCD Device and get LCD infomation
	*/
    if (rt_device_open(lcd_device, RT_DEVICE_OFLAG_RDWR) == RT_EOK)
    {
        struct rt_device_graphic_info info;
        if (rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_INFO, &info) == RT_EOK)
        {
            rt_kprintf("Lcd info w:%d, h%d, bits_per_pixel %d, draw_align:%d\r\n",
                       info.width, info.height, info.bits_per_pixel, info.draw_align);
        }
    }
    else
    {
        rt_kprintf("Lcd open error!\n");
        return;
    }
	rt_sem_init(&lcd_sema, "lcdsem", 0, RT_IPC_FLAG_FIFO);

	//display test content.
	lcd_refresh(0);
	HAL_Delay_us(100 * 1000);
	lcd_refresh(0);
}

static uint8_t get_pixel_size(uint16_t color_format)
{
    uint8_t pixel_size;

    if (RTGRAPHIC_PIXEL_FORMAT_RGB565 == color_format)
    {
        pixel_size = 2;
    }
    else if (RTGRAPHIC_PIXEL_FORMAT_RGB888 == color_format)
    {
        pixel_size = 3;
    }
    else
    {
        pixel_size = 0;
    }

    return pixel_size;
}


static uint32_t make_color(uint16_t cf, uint32_t rgb888)
{
    uint8_t r, g, b;

    r = (rgb888 >> 16) & 0xFF;
    g = (rgb888 >> 8) & 0xFF;
    b = (rgb888 >> 0) & 0xFF;

    switch (cf)
    {
    case RTGRAPHIC_PIXEL_FORMAT_RGB565:
        return ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3));
    case RTGRAPHIC_PIXEL_FORMAT_RGB666:
        return ((((r) & 0xFC) << 10) | (((g) & 0xFC) << 4) | (((b) & 0xFC) >> 2));
    case RTGRAPHIC_PIXEL_FORMAT_RGB888:
        return ((((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF));

    default:
        return 0;
    }
}


static void fill_color(uint8_t *buf, uint32_t width, uint32_t height,
                       uint16_t cf, uint32_t ARGB8888)
{
    uint8_t pixel_size;

    if (RTGRAPHIC_PIXEL_FORMAT_RGB565 == cf)
    {
        pixel_size = 2;
    }
    else if (RTGRAPHIC_PIXEL_FORMAT_RGB888 == cf)
    {
        pixel_size = 3;
    }
    else
    {
        RT_ASSERT(0);
    }

    uint32_t i, j, k, c;
    c = make_color(cf, ARGB8888);
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            for (k = 0; k < pixel_size; k++)
            {
                *buf++ = (c >> (k << 3)) & 0xFF;
            }
        }
    }
}

/*
        |<-------------  buffer width --------------------->|
---      0x00000001   0x00000002  0x00000003  ....  0x000000FF
^       0x00000001   0x00000002  0x00000003  ....  0x000000FF
|       0x00000001   0x00000002  0x00000003  ....  0x000000FF
buffer   0x00000001   0x00000002  0x00000003  ....  0x000000FF
height   0x00000001   0x00000002  0x00000003  ....  0x000000FF
 |      0x00000001   0x00000002  0x00000003  ....  0x000000FF
 |      0x00000001   0x00000002  0x00000003  ....  0x000000FF
 v      0x00000001   0x00000002  0x00000003  ....  0x000000FF
----     0x00000001   0x00000002  0x00000003  ....  0x000000FF

*/
static void fill_hor_gradient_color(uint8_t *buf, uint32_t width, uint32_t height,
                                    uint16_t cf, uint32_t color)
{
    uint8_t pixel_size;
    uint8_t *buf_head = buf;

    pixel_size = get_pixel_size(cf);

    uint32_t i, j, k;
    for (i = 0; i < height; i++)
    {
        if (0 == i) //fill first line
        {
            for (j = 0; j < width; j++)
            {
                uint8_t r, g, b;
                uint32_t grad_color;

                r = ((color >> 16) & 0xFF) * j / width;
                g = ((color >> 8) & 0xFF) * j / width;
                b = ((color >> 0) & 0xFF) * j / width;

                grad_color = make_color(cf, (r << 16 | g << 8 | b));
                switch (cf)
                {
                case RTGRAPHIC_PIXEL_FORMAT_RGB565:
                    grad_color = grad_color & 0xFFDF; //Convert to RGB555 to make gradient more smooth.
                    *(buf ++) = (uint8_t)((grad_color >> 0) & 0xFF);
                    *(buf ++) = (uint8_t)((grad_color >> 8) & 0xFF);
                    break;

                case RTGRAPHIC_PIXEL_FORMAT_RGB888:
                    *(buf ++) = (uint8_t)((grad_color >> 0) & 0xFF);
                    *(buf ++) = (uint8_t)((grad_color >> 8) & 0xFF);
                    *(buf ++) = (uint8_t)((grad_color >> 16) & 0xFF);
                    break;
                default:
                    RT_ASSERT(0);
                    break;
                }
            }
        }
        else//copy to left lines
        {
            memcpy(buf, buf - width * pixel_size, width * pixel_size);
            buf += width * pixel_size;
        }
    }
}


/*
        |<-------------  buffer width --------------------->|
---        0x00000000      0x00000000      0x00000000       0x00000000
^          0x00010101      0x00010101      0x00010101       0x00010101
|          0x00020202      0x00020202      0x00020202       0x00020202
buffer     0x00030303      0x00030303      0x00030303       0x00030303
height           ....         ....         ....         ....
 |         0x00FFFFFF      0x00FFFFFF      0x00FFFFFF      0x00FFFFFF
 |         0x00000001      0x00000001      0x00000001      0x00000001
 |         0x00020202      0x00020202      0x00020202      0x00020202
 |         0x00030303      0x00030303      0x00030303      0x00030303
 |             ....      ....           ....              ....
 |         0x00FFFFFF      0x00FFFFFF      0x00FFFFFF      0x00FFFFFF
 v              ...          ...           ...                ...
----

*/
static void fill_ver_gradient_color(uint8_t *buf, uint32_t width, uint32_t height,
                                    uint16_t cf)
{
    uint8_t pixel_size;
    uint8_t *buf_head = buf;

    pixel_size = get_pixel_size(cf);

    uint32_t i, j;
    for (i = 0; i < height; i++)
    {
        uint8_t r, g, b;
        uint32_t grad_color;

        r = (uint8_t) i & 0xFF;
        g = r;
        b = r;

        grad_color = make_color(cf, (r << 16 | g << 8 | b));

        for (j = 0; j < width; j++)
        {
            switch (cf)
            {
            case RTGRAPHIC_PIXEL_FORMAT_RGB565:
                *(buf ++) = (uint8_t)((grad_color >> 0) & 0xFF);
                *(buf ++) = (uint8_t)((grad_color >> 8) & 0xFF);
                break;

            case RTGRAPHIC_PIXEL_FORMAT_RGB888:
                *(buf ++) = (uint8_t)((grad_color >> 0) & 0xFF);
                *(buf ++) = (uint8_t)((grad_color >> 8) & 0xFF);
                *(buf ++) = (uint8_t)((grad_color >> 16) & 0xFF);
                break;
            default:
                RT_ASSERT(0);
                break;
            }
        }
    }
}


static void fill_color_bar(uint8_t *buf, uint32_t width, uint32_t height, uint16_t cf)
{
    uint32_t i = 0;
    uint32_t line_height = height / 8;
    uint32_t offset = get_pixel_size(cf) * width * line_height;

    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0xFFFFFF);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0x777777);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0xFF0000);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0x00FF00);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0x0000FF);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0x00FFFF);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0xFF00FF);
    i++;
    fill_hor_gradient_color(buf + offset * i, width, line_height, cf, 0xFFFF00);
    i++;
}



static void lcd_refresh(uint8_t id)
{
    RT_ASSERT(get_pixel_size(FB_COLOR_FORMAT) == FB_PIXEL_BYTES);

    uint8_t *p_framebuffer = (uint8_t *)&framebuffer1[0];


    uint16_t framebuffer_color_format = FB_COLOR_FORMAT;

    /*Fill framebuffer*/
    rt_kprintf("Fill framebuffer addr=0x%x, w=%d, h=%d, size=%d(Bytes)\n", p_framebuffer, FB_WIDTH, FB_HEIGHT, FB_TOTAL_BYTES);
    if (0 == id) fill_color_bar((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format);  //Fill 8 gradient color bar
    if (1 == id) fill_ver_gradient_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format);  //Fill vertical grey color bar
    if (2 == id) fill_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format, 0xFF0000); //Fill RED color
    if (3 == id) fill_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format, 0x00FF00); //Fill GREEN color
    if (4 == id) fill_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format, 0x0000FF); //Fill BLUE color
    if (5 == id) fill_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format, 0xFFFFFF); //Fill WHITE color
    if (6 == id) fill_color((uint8_t *)p_framebuffer, FB_WIDTH, FB_HEIGHT, framebuffer_color_format, 0x000000); //Fill BLACK color

	rt_kprintf("mpu chache clean\n");
    mpu_dcache_clean((uint32_t *)p_framebuffer, sizeof(framebuffer1));

	rt_device_open(lcd_device, RT_DEVICE_OFLAG_RDWR);

    /*Flush framebuffer to LCD*/
	rt_kprintf("RTGRAPHIC_CTRL_SET_BUF_FORMAT.\n");
    rt_device_control(lcd_device, RTGRAPHIC_CTRL_SET_BUF_FORMAT, &framebuffer_color_format);
	rt_kprintf("set_window\n");
    rt_graphix_ops(lcd_device)->set_window(0, 0, FB_WIDTH - 1, FB_HEIGHT - 1);
	rt_kprintf("rt_device_set_tx_complete\n");
    rt_device_set_tx_complete(lcd_device, lcd_flush_done);
	rt_kprintf("draw_rect_async\n");
    rt_graphix_ops(lcd_device)->draw_rect_async((const char *)p_framebuffer, 0, 0, FB_WIDTH - 1, FB_HEIGHT - 1);
	rt_kprintf("wait sema\n");

	/*Waitting for Flushing LCD done*/
    rt_sem_take(&lcd_sema, RT_WAITING_FOREVER);
	rt_device_close(lcd_device);
}


static int lcd_test(int argc, char *argv[])
{
    if (argc < 2) {
		LOG_D("usage %s #id\n", argv[0]);
		return 0;
	}

	uint8_t id = atoi(argv[1]);
	if (id == 99) {
		LOG_D("run 20fps refresh\n");
		for (uint8_t i=0; i<200; i++) {
			lcd_refresh(i%7);
			rt_thread_mdelay(80);
		}	
	}
	lcd_refresh(id);
	LOG_D("lcd refresh color id:%d = %d\n", id);
    return 0;
}
MSH_CMD_EXPORT(lcd_test, "lcd test")


