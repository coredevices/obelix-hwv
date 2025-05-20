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
	ioexp_init();
	aw86225_init();
	eta4662_init();
		
    while (1)
    {
        rt_thread_mdelay(5000);
        //rt_kprintf("__main loop__\r\n");
    }

    return RT_EOK;
}

