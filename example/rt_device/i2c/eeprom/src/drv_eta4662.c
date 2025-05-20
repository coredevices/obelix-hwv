#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"

#define DBG_TAG "drv_eta4662"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void eta4662_init(void)
{
	LOG_D("eta4662_init i2c");
	i2c_init(ETA4662_I2C_ID);

	uint8_t value;
	rt_size_t ret = i2c_read(ETA4662_I2C_ID, ETA4662_I2C_ADDRESS, 0x0B, &value);
	if (ret) {
		LOG_D("eta4662 read device id=0x%x\n", value);
	} else {
		LOG_E("eta4662 read device id error.\n");
	}

	//todo:reg int pin
}

/*
static int ioexp_set(int argc, char *argv[])
{
    if (argc < 2) {
		LOG_D("usage %s ioe_pin level\n", argv[0]);
		return 0;
	}

	uint8_t pin = atoi(argv[1]);
	uint8_t level = atoi(argv[2]);
	ioexp_pin_set(pin, level);
	LOG_D("ioe set pin:%d = %d\n", pin, level);
    return 0;
}
MSH_CMD_EXPORT(ioexp_set, "trigger notification to client")
*/

