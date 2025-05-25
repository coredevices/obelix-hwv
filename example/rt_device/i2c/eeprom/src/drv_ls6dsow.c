#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"
#include "lsm6dsox_reg.h"

#define DBG_TAG "drv_ls6dsow"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define IMU_POWER_PIN	IOEXP_CH07

static stmdev_ctx_t* lsm6dsow_ctx = NULL;

static int32_t stmdev_write(void * handler, uint8_t reg, uint8_t* data, uint16_t len)
{
	return i2c_write_boost(LSM6DSOW_I2C_ID, LSM6DSOX_I2C_ADD_L, reg, data, len);
}

static int32_t stmdev_read(void *handler, uint8_t reg, uint8_t* data, uint16_t len)
{
	return i2c_read_boost(LSM6DSOW_I2C_ID, LSM6DSOX_I2C_ADD_L, reg, data, len);
}


void ls6dsow_init(void)
{
	//1. init ctx handler
	lsm6dsow_ctx = malloc(sizeof(stmdev_ctx_t));
	memset(lsm6dsow_ctx, 0, sizeof(lsm6dsow_ctx));
	lsm6dsow_ctx->handle = NULL;
	lsm6dsow_ctx->read_reg = stmdev_read;
	lsm6dsow_ctx->write_reg = stmdev_write;

	//2. power on device
	ioexp_pin_set(IMU_POWER_PIN, 0);
	HAL_Delay_us(200 * 1000);

	//3. read device ID
	lsm6dsox_id_t id = {0};
	int32_t ret = lsm6dsox_id_get(lsm6dsow_ctx, NULL, &id);
	LOG_D("read imu device ID:0x%x\n", id.ui);
	if (id.ui == LSM6DSOX_ID) {
		LOG_D("**** TESTING COMMS WITH LSM6DSOW: PASS ********\n");
	} else {
		LOG_D("**** TESTING COMMS WITH LSM6DSOW: FAILED ********\n");
	}
}

void ls6dsow_start(void)
{

}


static int lsm6dsow_mode(int argc, char *argv[])
{
    if (argc < 2) {
		LOG_D("usage %s #get; #set mode\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "get") == 0) {
		lsm6dsox_md_t mode;
		lsm6dsox_mode_get(lsm6dsow_ctx, NULL, &mode);
		LOG_D("lsm6dsox odr:%d\n", mode.ui.xl.odr);
	}
	if (strcmp(argv[1], "set") == 0) {

	}
	
    return 0;
}
MSH_CMD_EXPORT(lsm6dsow_mode, "lsm6dsow_mode get/set")


