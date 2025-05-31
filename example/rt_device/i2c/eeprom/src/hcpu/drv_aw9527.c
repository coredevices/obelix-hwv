#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "drv_aw9527.h"

#define DBG_TAG "drv_aw9527"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void ioexp_init(void)
{
	LOG_D("ioexp_init gpio");
	gpio_set(44, 0, 1);
	HAL_Delay_us(200);
	//gpio_set(44, 0, 1);
	//HAL_Delay_us(500);
	gpio_set(44, 1, 1);
	HAL_Delay_us(10 *1000);
	LOG_D("ioexp_init i2c");
	//i2c_init(AW9527_I2C_ID);
	LOG_D("iic init done");

	uint8_t value;
	rt_size_t ret = i2c_read(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x10, &value);
	//set work mode, configure P1_7~P1_5 to LED mode
	i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x13, 0x1F);
	if (ret) {
		LOG_D("aw9527 read device id=0x%x\n", value);
		LOG_D("**** TESTING COMMS WITH AW9527: PASS ********\n");
	} else {
		LOG_E("aw9527 read device id error.\n");
		LOG_E("**** TESTING COMMS WITH AW9527: FAILED ********\n");
	}

	//gpio_set(44, 0, 1);
}

/*channel ctrl*/
void ioexp_pin_set(IOEXP_CHANNEL_T channel, IOEXP_STATE_T state) {
    if (channel >= IOEXP_CH10) {
        uint8_t p1_value;
		channel -= IOEXP_CH10;
        i2c_read(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x03, &p1_value);
        if (state == IOEXP_HIGH) {
            p1_value |= (1<<channel);
        } else {
            p1_value &= ~(1<<channel);
        }
        i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x03, p1_value);
    } else {
        uint8_t p0_value;
        i2c_read(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x02, &p0_value);
        if (state == IOEXP_HIGH) {
            p0_value |= (1<<channel);
        } else {
            p0_value &= ~(1<<channel);
        }
        i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x02, p0_value);
    }
}

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
MSH_CMD_EXPORT(ioexp_set, "ioexp_set cmd")

static int ioexp_dump(int argc, char *argv[])
{
    if (argc != 1) {
		LOG_D("usage %s to get all regs\n", argv[0]);
		return 0;
	}

	for (uint8_t i=0; i<=0x07; i++) {
		uint8_t value;
		i2c_read(AW9527_I2C_ID, AW9527_I2C_ADDRESS, i, &value);
		LOG_D("aw9527 reg[0x%x] = 0x%02x\n", i, value);
	}
	for (uint8_t i=0x10; i<=0x13; i++) {
		uint8_t value;
		i2c_read(AW9527_I2C_ID, AW9527_I2C_ADDRESS, i, &value);
		LOG_D("aw9527 reg[0x%x] = 0x%02x\n", i, value);
	}
    return 0;
}
MSH_CMD_EXPORT(ioexp_dump, "ioexp_dump cmd")


void aw9527_set_brightness(uint8_t channel, uint8_t brightness)
{
	if (channel == IOEXP_CH15)
	{
		i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x2d, brightness);
		LOG_D("aw9527 reg[0x%x] = 0x%02x\n", 0x2d, brightness);
	}
	else if (channel == IOEXP_CH16)
	{
		i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x2e, brightness);
		LOG_D("aw9527 reg[0x%x] = 0x%02x\n", 0x2e, brightness);
	}
	else if (channel == IOEXP_CH17)
	{
		i2c_write(AW9527_I2C_ID, AW9527_I2C_ADDRESS, 0x2f, brightness);
		LOG_D("aw9527 reg[0x%x] = 0x%02x\n", 0x2f, brightness);
	}
}
MSH_CMD_EXPORT(aw9527_set_brightness, "aw9527_set_brightness cmd")

