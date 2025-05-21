#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"

#define DBG_TAG "drv_eta4662"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define MMC5603_PWR_PIN    IOEXP_CH06


void mmc5603_init(void)
{
	ioexp_pin_set(MMC5603_PWR_PIN, IOEXP_LOW);
	LOG_D("eta4662_init i2c");
	i2c_init(MMC5603_I2C_ID);
	HAL_Delay_us(20 * 1000);

	uint8_t value;
	rt_size_t ret = i2c_read(MMC5603_I2C_ID, MMC5603_I2C_ADDRESS, 0x39, &value);
	if (ret) {
		LOG_D("mmc5603 read device id=0x%x\n", value);
	} else {
		LOG_E("mmc5603 read device id error.\n");
	}

	//todo:reg int pin
}

void mmc5603_power_down(void) 
{
	i2c_write(MMC5603_I2C_ID, MMC5603_I2C_ADDRESS, 0x1D, 0x00);
	HAL_Delay_us(20 * 1000);
	//ioexp_pin_set(MMC5603_PWR_PIN, IOEXP_HIGH);
	LOG_D("mmc5603 enter low power mode");
}

int mmc5603_pd(int argc, char *argv[])
{
    if (argc != 1) {
		LOG_D("usage %s without param\n", argv[0]);
		return 0;
	}

	mmc5603_power_down();
	return 0;
}
MSH_CMD_EXPORT(mmc5603_pd, "mmc5603 enter low power mode")

