#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"

#define DBG_TAG "drv_platform"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* i2c  */

static struct rt_i2c_bus_device *i2c_bus1 = NULL;
static struct rt_i2c_bus_device *i2c_bus2 = NULL;
static struct rt_i2c_bus_device *i2c_bus3 = NULL;
static struct rt_i2c_bus_device *i2c_bus4 = NULL;

uint8_t i2c_init(uint8_t id)
{
	struct rt_i2c_bus_device * i2c[5] = {NULL};//, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};
		//1. pin mux
#ifdef SF32LB52X
	switch(id) {
	case 1:
		HAL_PIN_Set(PAD_PA11, I2C1_SCL, PIN_NOPULL, 1); // i2c io select
		HAL_PIN_Set(PAD_PA10, I2C1_SDA, PIN_NOPULL, 1);
		i2c[id] = i2c_bus1 = rt_i2c_bus_device_find("i2c1");
		rt_kprintf("i2c_bus1:0x%x\n", i2c_bus1);
		break;
	case 2:
		HAL_PIN_Set(PAD_PA29, I2C2_SCL, PIN_NOPULL, 1); // i2c io select
		HAL_PIN_Set(PAD_PA28, I2C2_SDA, PIN_NOPULL, 1);
		i2c[id] = i2c_bus2 = rt_i2c_bus_device_find("i2c2");
		rt_kprintf("i2c_bus2:0x%x\n", i2c_bus2);
		break;
	case 3:
		HAL_PIN_Set(PAD_PA21, I2C3_SCL, PIN_NOPULL, 1); // i2c io select
		HAL_PIN_Set(PAD_PA20, I2C3_SDA, PIN_NOPULL, 1);
		i2c[id] = i2c_bus3 = rt_i2c_bus_device_find("i2c3");
		rt_kprintf("i2c_bus3:0x%x\n", i2c_bus3);
		break;
	case 4:
		HAL_PIN_Set(PAD_PA31, I2C4_SCL, PIN_NOPULL, 1); // i2c io select
		HAL_PIN_Set(PAD_PA30, I2C4_SDA, PIN_NOPULL, 1);
		i2c[id] = i2c_bus4 = rt_i2c_bus_device_find("i2c4");
		rt_kprintf("i2c_bus4:0x%x\n", i2c_bus4);
		break;
#elif defined(SF32LB58X)
		HAL_PIN_Set(PAD_PB28, I2C6_SCL, PIN_PULLUP, 0); // i2c io select
		HAL_PIN_Set(PAD_PB29, I2C6_SDA, PIN_PULLUP, 0);
#endif
	}

	rt_kprintf("i2c[%d]=0x%x\n", id, i2c[id]);
	if (i2c[id])
	{
#ifdef SF32LB52X
		rt_kprintf("Find i2c bus device I2C%d\n", id);
#elif defined(SF32LB58X)
		rt_kprintf("Find i2c bus device I2C6\n");
#endif
		// open rt_device i2c2
		rt_device_open((rt_device_t)i2c[id], RT_DEVICE_FLAG_RDWR);
		//rt_i2c_open(i2c_bus, RT_DEVICE_FLAG_RDWR);
		struct rt_i2c_configuration configuration =
		{
			.mode = 0,
			.addr = 0,
			.timeout = 500, //Waiting for timeout period (ms)
			.max_hz = 400000, //I2C rate (hz)
		};
		// config I2C parameter
		rt_i2c_configure(i2c[id], &configuration);
	}
	else
	{
#ifdef SF32LB52X
		LOG_E("Can not found i2c bus I2C%d, init fail\n", id);
#elif defined(SF32LB58X)
		LOG_E("Can not found i2c bus I2C6, init fail\n");
#endif
		return -1;
	}
	return 0;
}

rt_size_t i2c_read(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t* value)
{
	struct rt_i2c_bus_device * i2c[] = {NULL, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};

	HAL_StatusTypeDef ret = HAL_ERROR;
    uint8_t buf[2];

	if (i2c[i2c_id]) {
	    ret = rt_i2c_mem_read(i2c[i2c_id], device_addr, reg, 8, value, 1);  // ret: return data size
	    //LOG_D("i2c[%d] read device:0x%x reg:0x%x,pdata:0x%x,ret:%d\n", i2c_id, device_addr, reg, *value, ret);
	}
	
	return ret;
}

rt_size_t i2c_write(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t value)
{
	struct rt_i2c_bus_device * i2c[] = {NULL, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};

	rt_size_t ret = 0;
    uint8_t buf[2];

	if (i2c[i2c_id]) {
    	ret = rt_i2c_mem_write(i2c[i2c_id], device_addr, reg, 8, &value, 1);  // ret: return data size
    	//LOG_D("i2c[%d] write device:0x%x reg:0x%x,pdata:0x%x,ret:%d\n", i2c_id, device_addr, reg, value, ret);
	}

	return ret;
}

void gpio_set(int pin, int val, int is_porta)
{
    GPIO_TypeDef *gpio = (is_porta) ? hwp_gpio1 : hwp_gpio2;
    GPIO_InitTypeDef GPIO_InitStruct;

    // set sensor pin to output mode
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_PIN_Set(PAD_PA00 + pin, GPIO_A0 + pin, PIN_PULLUP, 1); 
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);

    HAL_GPIO_WritePin(gpio, pin, (GPIO_PinState)val);
}

