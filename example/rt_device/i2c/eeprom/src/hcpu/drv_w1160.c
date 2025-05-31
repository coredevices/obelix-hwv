#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"

#define DBG_TAG "drv_w1160"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void w1160_init(void)
{
	uint8_t value;
	rt_size_t ret = i2c_read(W1160_I2C_ID, W1160_I2C_ADDRESS, 0x3e, &value);
	if (ret) {
		LOG_D("W1160 read device id=0x%x\n", value);
		LOG_D("**** TESTING COMMS WITH W1160: PASS ********\n");
	} else {
		LOG_E("W1160 read device id error.\n");
		LOG_E("**** TESTING COMMS WITH W1160: FAILED ********\n");
	}
}

