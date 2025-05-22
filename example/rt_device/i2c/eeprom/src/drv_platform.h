#pragma once

#include "rtthread.h"
#include "bf0_hal.h"
#include "board.h"

/* i2c */
//Version 5.0

#define EEPROM_I2C_ADDRESS         	0x50  // 7bit device address of EEPROM
#define AW9527_I2C_ADDRESS			0X5B
#define CST816_I2C_ADDRESS			0X15
#define GH3026_I2C_ADDRESS			0X58
#define ETA4662_I2C_ADDRESS			0X07
#define AW86225_I2C_ADDRESS			0X58
#define LS6DSOW_I2C_ADDRESS			0X6A
#define MMC5603_I2C_ADDRESS			0X30
#define W1160_I2C_ADDRESS			0X5B

#define AW9527_I2C_ID				5
#define AW86225_I2C_ID				4
#define ETA4662_I2C_ID				4
#define MMC5603_I2C_ID				4

uint8_t i2c_init(uint8_t id);
rt_size_t i2c_read(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t* value);
rt_size_t i2c_write(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t value);
void gpio_set(int pin, int val, int is_porta);


