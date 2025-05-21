#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"

#define DBG_TAG "drv_key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static rt_uint32_t pin34;

static void key_pin_int_callback(void* args)
{
	rt_uint32_t pin = (rt_uint32_t)(rt_uint32_t*)args;
	LOG_D("key pressed:%d\n", pin);

	switch (pin) {
	case 34:
		LOG_D("**** TESTING COMMS WITH KEY1: PASS ********\n");
		break;
	case 35:
		LOG_D("**** TESTING COMMS WITH KEY2: PASS ********\n");
		break;
	case 36:
		LOG_D("**** TESTING COMMS WITH KEY3: PASS ********\n");
		break;
	case 37:
		LOG_D("**** TESTING COMMS WITH KEY4: PASS ********\n");
		break;
	}
}

void key_init(void)
{
	HAL_PIN_Set(PAD_PA34, GPIO_A34, PIN_NOPULL, 1);
	HAL_PIN_Set(PAD_PA35, GPIO_A35, PIN_NOPULL, 1);
	HAL_PIN_Set(PAD_PA36, GPIO_A36, PIN_NOPULL, 1);
	HAL_PIN_Set(PAD_PA37, GPIO_A37, PIN_NOPULL, 1);

	rt_pin_mode(34, PIN_MODE_INPUT);
	rt_pin_mode(35, PIN_MODE_INPUT);
	rt_pin_mode(36, PIN_MODE_INPUT);
	rt_pin_mode(37, PIN_MODE_INPUT);

	rt_pin_attach_irq(34, PIN_IRQ_MODE_RISING_FALLING, key_pin_int_callback, (void *)(rt_uint32_t)34);
    rt_pin_irq_enable(34, ENABLE);
	rt_pin_attach_irq(35, PIN_IRQ_MODE_RISING_FALLING, key_pin_int_callback, (void *)(rt_uint32_t)35);
    rt_pin_irq_enable(35, ENABLE);
	rt_pin_attach_irq(36, PIN_IRQ_MODE_RISING_FALLING, key_pin_int_callback, (void *)(rt_uint32_t)36);
    rt_pin_irq_enable(36, ENABLE);
	rt_pin_attach_irq(37, PIN_IRQ_MODE_RISING_FALLING, key_pin_int_callback, (void *)(rt_uint32_t)37);
    rt_pin_irq_enable(37, ENABLE);
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

