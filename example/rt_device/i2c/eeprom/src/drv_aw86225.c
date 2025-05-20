#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"

#define DBG_TAG "drv_aw86225"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


void aw86225_init(void)
{
	LOG_D("aw86225_init gpio");
	gpio_set(1, 1, 1);
	LOG_D("aw86225_init i2c");
	i2c_init(AW86225_I2C_ID);
	HAL_Delay_us(20 * 1000);

	uint8_t value;
	rt_size_t ret = i2c_read(AW86225_I2C_ID, AW86225_I2C_ADDRESS, 0x64, &value);
	if (ret) {
		LOG_D("aw86225 read device id=0x%x\n", value);
	} else {
		LOG_E("aw86225 read device id error.\n");
	}
}

int aw86225_standby(int argc, char *argv[])
{
    if (argc != 1) {
		LOG_D("usage %s without param\n", argv[0]);
		return 0;
	}

	gpio_set(1, 0, 1);
	LOG_D("aw86225 standby\n");
    return 0;
}
MSH_CMD_EXPORT(aw86225_standby, "aw86225 standby")


