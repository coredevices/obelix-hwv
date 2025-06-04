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

typedef union{
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

static stmdev_ctx_t* lsm6dsow_ctx = NULL;

int ls6dsow_start(void);


static int32_t stmdev_write(void * handler, uint8_t reg, uint8_t* data, uint16_t len)
{
	int32_t size =  i2c_write_boost(LSM6DSOW_I2C_ID, LSM6DSOX_I2C_ADD_L, reg, data, len);
	return size?0:1;
}

static int32_t stmdev_read(void *handler, uint8_t reg, uint8_t* data, uint16_t len)
{
	int32_t size = i2c_read_boost(LSM6DSOW_I2C_ID, LSM6DSOX_I2C_ADD_L, reg, data, len);
	return size?0:1;
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

	ls6dsow_start();

#if 1
	HAL_PIN_Set(PAD_PA38, GPIO_A38, PIN_NOPULL, 1);
	rt_pin_mode(38, PIN_MODE_INPUT);
	rt_pin_attach_irq(38, PIN_IRQ_MODE_RISING, imu_int_calback, (void *)(rt_uint32_t)38);
    rt_pin_irq_enable(38, ENABLE);
	// 唤醒
	GPIO_TypeDef *gpio = GET_GPIO_INSTANCE(38);
	uint16_t gpio_pin = GET_GPIOx_PIN(38);
	int8_t wakeup_pin = HAL_HPAON_QueryWakeupPin(gpio, gpio_pin);
	ASSERT(wakeup_pin >= 0);
	pm_enable_pin_wakeup(wakeup_pin, AON_PIN_MODE_POS_EDGE);
#endif
}

int ls6dsow_start(void)
{
	uint8_t rst = 0;
	int ret = -1;

#if 1
	/* Uncomment to configure INT 2 */
	lsm6dsox_pin_int2_route_t int2_route;
	lsm6dsox_pin_int1_route_t int1_route;
	
	/* Restore default configuration */
	lsm6dsox_reset_set(lsm6dsow_ctx, PROPERTY_ENABLE);
	do {
	  lsm6dsox_reset_get(lsm6dsow_ctx, &rst);
	} while (rst);
	
	/* Disable I3C interface */
	lsm6dsox_i3c_disable_set(lsm6dsow_ctx, LSM6DSOX_I3C_DISABLE);
	
	/* full scale: +/- 2g */
	ret = lsm6dsox_xl_full_scale_set(lsm6dsow_ctx, LSM6DSOX_2g);
	if (ret < 0) {
		LOG_E("Failed to set accelerometer data rate");
		return ret;
	}

	/* data rate: 26 Hz */
	ret = lsm6dsox_xl_data_rate_set(lsm6dsow_ctx, LSM6DSOX_XL_ODR_26Hz);
	if (ret < 0) {
		LOG_E("Failed to set accelerometer data rate");
		return ret;
	}

	ret = lsm6dsox_fsm_data_rate_set(lsm6dsow_ctx, LSM6DSOX_ODR_FSM_26Hz);
	if (ret < 0) {
		LOG_E("Failed to set FSM data rate");
		return ret;
	}

	/* low power normal mode (no HP) */
	ret = lsm6dsox_xl_power_mode_set(lsm6dsow_ctx, LSM6DSOX_LOW_NORMAL_POWER_MD);
	if (ret < 0) {
		LOG_E("Failed to set power mode");
		return ret;
	}

	/* tap detection on all axis: X, Y, Z */
	ret = lsm6dsox_tap_detection_on_x_set(lsm6dsow_ctx, PROPERTY_ENABLE);
	if (ret < 0) {
		LOG_E("Failed to enable tap detection on X axis");
		return ret;
	}

	ret = lsm6dsox_tap_detection_on_y_set(lsm6dsow_ctx, PROPERTY_ENABLE);
	if (ret < 0) {
		LOG_E("Failed to enable tap detection on Y axis");
		return ret;
	}

	ret = lsm6dsox_tap_detection_on_z_set(lsm6dsow_ctx, PROPERTY_ENABLE);
	if (ret < 0) {
		LOG_E("Failed to enable tap detection on Z axis");
		return ret;
	}

	/* X,Y,Z threshold: 1 * FS_XL / 2^5 = 1 * 2 / 32 = 62.5 mg */
	ret = lsm6dsox_tap_threshold_x_set(lsm6dsow_ctx, 1);
	if (ret < 0) {
		LOG_E("Failed to set tap threshold on X axis");
		return ret;
	}

	ret = lsm6dsox_tap_threshold_y_set(lsm6dsow_ctx, 1);
	if (ret < 0) {
		LOG_E("Failed to set tap threshold on Y axis");
		return ret;
	}

	ret = lsm6dsox_tap_threshold_z_set(lsm6dsow_ctx, 1);
	if (ret < 0) {
		LOG_E("Failed to set tap threshold on Z axis");
		return ret;
	}

	/* shock time: 2 / ODR_XL = 2 / 26 ~= 77 ms */
	ret = lsm6dsox_tap_shock_set(lsm6dsow_ctx, 0);
	if (ret < 0) {
		LOG_E("Failed to set tap shock duration");
		return ret;
	}

	/* quiet time: 2 / ODR_XL = 2 / 26 ~= 77 ms */
	ret = lsm6dsox_tap_quiet_set(lsm6dsow_ctx, 0);
	if (ret < 0) {
		LOG_E("Failed to set tap quiet duration");
		return ret;
	}

	/* enable single tap only */
	ret = lsm6dsox_tap_mode_set(lsm6dsow_ctx, LSM6DSOX_ONLY_SINGLE);
	if (ret < 0) {
		LOG_E("Failed to set tap mode");
		return ret;
	}

	/* route single rap to INT1 */
	lsm6dsox_pin_int1_route_t route = {.single_tap = 1};
	ret = lsm6dsox_pin_int1_route_set(lsm6dsow_ctx, route);
	if (ret < 0) {
		LOG_E("Failed to route interrupt");
		return ret;
	}
#else
		/* Uncomment to configure INT 2 */
		lsm6dsox_pin_int2_route_t int2_route;
		lsm6dsox_pin_int1_route_t int1_route;
	  /* Restore default configuration */
	  lsm6dsox_reset_set(lsm6dsow_ctx, PROPERTY_ENABLE);
	  do {
	    lsm6dsox_reset_get(lsm6dsow_ctx, &rst);
	  } while (rst);

	  /* Disable I3C interface */
	  lsm6dsox_i3c_disable_set(lsm6dsow_ctx, LSM6DSOX_I3C_DISABLE);

	  /* Enable Block Data Update */
	  lsm6dsox_block_data_update_set(lsm6dsow_ctx, PROPERTY_ENABLE);

	  /* Set Output Data Rate */
	  lsm6dsox_xl_data_rate_set(lsm6dsow_ctx, LSM6DSOX_XL_ODR_12Hz5);
	  lsm6dsox_gy_data_rate_set(lsm6dsow_ctx, LSM6DSOX_GY_ODR_12Hz5);

	  /* Set full scale */
	  lsm6dsox_xl_full_scale_set(lsm6dsow_ctx, LSM6DSOX_2g);
	  lsm6dsox_gy_full_scale_set(lsm6dsow_ctx, LSM6DSOX_2000dps);

	  /* Enable drdy 75 μs pulse: uncomment if interrupt must be pulsed */
	  lsm6dsox_data_ready_mode_set(lsm6dsow_ctx, LSM6DSOX_DRDY_LATCHED);

	  /* Uncomment if interrupt generation on Free Fall INT1 pin */
	  lsm6dsox_pin_int1_route_get(lsm6dsow_ctx, &int1_route);
	  int1_route.single_tap = PROPERTY_ENABLE;
	  lsm6dsox_pin_int1_route_set(lsm6dsow_ctx,int1_route);

	  /* Uncomment if interrupt generation on Free Fall INT2 pin */
	  //lsm6dsox_pin_int2_route_get(&dev_ctx, &int2_route);
	  //int2_route.reg.md2_cfg.int2_ff = PROPERTY_ENABLE;
	  //lsm6dsox_pin_int2_route_set(&dev_ctx, &int2_route);  
#endif
	return 0;
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
	if (strcmp(argv[1], "int") == 0) {
		lsm6dsox_all_sources_t all_source;
	    /* Check if Tap events */
	    lsm6dsox_all_sources_get(lsm6dsow_ctx, &all_source);
		//LOG_D("lsm6dsox int 0x%x\n", (uint32_t)all_source);
	    if (all_source.double_tap)
			LOG_D("lsm6dsox double_tap:%d\n");
		if (all_source.single_tap)
			LOG_D("lsm6dsox single_tap:%d\n");
	}
	if(strcmp(argv[1], "data") == 0) {
		uint8_t reg;

		/* Private variables ---------------------------------------------------------*/
		axis3bit16_t data_raw_acceleration;
		axis3bit16_t data_raw_angular_rate;
		float acceleration_mg[3];
		float angular_rate_mdps[3];

		/* Read output only if new xl value is available */
		lsm6dsox_xl_flag_data_ready_get(lsm6dsow_ctx, &reg);
		LOG_D("xl data avliable:%d\n", reg);
		if (reg) {
			/* Read acceleration field data */
			memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dsox_acceleration_raw_get(lsm6dsow_ctx, data_raw_acceleration.u8bit);
			acceleration_mg[0] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[0]);
			acceleration_mg[1] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[1]);
			acceleration_mg[2] =
			lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[2]);

			LOG_D("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
			acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
		}

		/* Read output only if new gyro value is available */
		lsm6dsox_gy_flag_data_ready_get(lsm6dsow_ctx, &reg);
		LOG_D("gy data avliable:%d\n", reg);
		if (reg) {
		/* Read angular rate field data */
			memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
			lsm6dsox_angular_rate_raw_get(lsm6dsow_ctx, data_raw_angular_rate.u8bit);
			angular_rate_mdps[0] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0]);
			angular_rate_mdps[1] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1]);
			angular_rate_mdps[2] =
			lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2]);

			LOG_D("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
			angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
		}
	}
	
    return 0;
}
MSH_CMD_EXPORT(lsm6dsow_mode, "lsm6dsow_mode get/set")


