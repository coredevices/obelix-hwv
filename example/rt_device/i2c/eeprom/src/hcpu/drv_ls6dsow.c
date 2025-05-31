#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "drv_gpio.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"
#include "lsm6dsox_reg.h"
#include "bf0_pm.h"

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


static void imu_int_calback(void* param)
{
	LOG_D("imu int.\n");
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

	lsm6dsox_all_on_int1_set(lsm6dsow_ctx, 1);

#if 1
	HAL_PIN_Set(PAD_PA38, GPIO_A38, PIN_NOPULL, 1);
	rt_pin_mode(38, PIN_MODE_INPUT);
	rt_pin_attach_irq(38, PIN_IRQ_MODE_RISING_FALLING, imu_int_calback, (void *)(rt_uint32_t)38);
    rt_pin_irq_enable(38, ENABLE);
	// 唤醒
	GPIO_TypeDef *gpio = GET_GPIO_INSTANCE(38);
	uint16_t gpio_pin = GET_GPIOx_PIN(38);
	int8_t wakeup_pin = HAL_HPAON_QueryWakeupPin(gpio, gpio_pin);
	ASSERT(wakeup_pin >= 0);
	pm_enable_pin_wakeup(wakeup_pin, AON_PIN_MODE_DOUBLE_EDGE);
#endif
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
		if (strcmp(argv[2], "tap") == 0) {
			LOG_D("tap detection mode\n");
			lsm6dsox_tap_mode_set(lsm6dsow_ctx, LSM6DSOX_BOTH_SINGLE_DOUBLE);
		}
	}
	
    return 0;
}
MSH_CMD_EXPORT(lsm6dsow_mode, "lsm6dsow_mode get/set")


