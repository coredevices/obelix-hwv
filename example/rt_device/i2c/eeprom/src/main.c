#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"
#include "drv_aw86225.h"
#include "drv_eta4662.h"
#include "drv_mmc5603.h"
#include "drv_battery.h"
#include "drv_key.h"
#include "drv_audio.h"
#include "drv_ls6dsow.h"
#include "drv_lcd.h"

#define DBG_TAG "PebbleTest"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t g_main_flag = 0;
/**
  * @brief  Main program
  * @param  None
  * @retval 0 if success, otherwise failure number
  */
int main(void)
{
    rt_kprintf("main!!\n");
    //LOG_D("Start i2c eeprom rtt demo!\n"); // Output a start message on console using printf function
    //EEPROM_example();
    //LOG_D("i2c end!\n"); // Output a end message on console using printf function

	LOG_D("Ready for Pebble CT2_DEV_Kit testing.\n");
	HAL_RCC_EnableModule(RCC_MOD_GPIO1); // GPIO clock enable
	hwp_rtc->BKP0R |= 0x5020;
	i2c_init(1);
	i2c_init(2);
	i2c_init(3);
	i2c_init(4);
	i2c_init(5);
	ioexp_init();
	lcd_init();
	aw86225_init();
	eta4662_init();
	battery_init();
	key_init();
	audio_init();
	ls6dsow_init();
	mmc5603_init();
		
    while (1)
    {
        rt_thread_mdelay(5000);
        //rt_kprintf("__main loop__\r\n");
    }

    return RT_EOK;
}

