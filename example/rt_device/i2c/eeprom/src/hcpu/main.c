#include "rtthread.h"
#include "bf0_hal.h"
#include "bf0_hal_aon.h"
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
#include "app_ble.h"
//#include "drv_pm.h"

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
	#if 0
	HAL_LPAON_DISABLE_PAD();
	HAL_LPAON_DISABLE_AON_PAD();
	HAL_LPAON_DISABLE_VLP();
	/* force lpsys to enter sleep */
	hwp_lpsys_aon->PMR = (3UL << LPSYS_AON_PMR_MODE_Pos) | (1 << LPSYS_AON_PMR_CPUWAIT_Pos) | (1 << LPSYS_AON_PMR_FORCE_SLEEP_Pos);
	#endif
	HAL_RCC_EnableModule(RCC_MOD_GPIO1); // GPIO clock enable
	//hwp_rtc->BKP0R |= 0x5020;
	i2c_init(1);
	i2c_init(2);
	i2c_init(3);
	i2c_init(4);
	i2c_init(5);
	ioexp_init();
	aw86225_init();
	//lcd_init();
	eta4662_init();
	battery_init();
	key_init();
	//audio_init();
	//ls6dsow_init();
	//mmc5603_init();
	//pm_init();
	app_ble_init();

    while (1)
    {
    	static uint8_t cnt = 0;
		cnt++;
		if (cnt%2) {
			rt_pm_request(PM_SLEEP_MODE_IDLE);
			rt_thread_mdelay(30 * 1000);
		} else {
			rt_pm_release(PM_SLEEP_MODE_IDLE);
			rt_thread_mdelay(150 * 1000);
		}
        
		rt_kprintf("loop.\n");
    }

    return RT_EOK;
}

static int psram_rw(int argc, char *argv[]) 
{
	//psram
	uint8_t* ptr = (uint8_t*)0x60000000;
	strcpy(ptr, "******hello psram*******");
	LOG_D("test psram:%s\n", ptr);
}
MSH_CMD_EXPORT(psram_rw, "psram read write test")


static int shutdown(int argc, char *argv[])
{
    if (argc != 1) {
		LOG_D("usage %s \n", argv[0]);
		return 0;
	}

	LOG_D("enter hibernate mode\n");
	rt_hw_interrupt_disable();
	
	HAL_PMU_EnterHibernate();
    return 0;
}
MSH_CMD_EXPORT(shutdown, "Hibernate")


