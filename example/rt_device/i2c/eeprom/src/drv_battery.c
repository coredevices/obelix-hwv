#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"

#define DBG_TAG "drv_ls6dsow"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define ADC_DEV_NAME        "bat1"      /* ADC name */
#define ADC_DEV_CHANNEL     7           /* ADC channe7 */
static rt_device_t s_adc_dev;
static rt_adc_cmd_read_arg_t read_arg;

static uint32_t battery_read(void)
{
	rt_err_t r;
    /* set pinmux of channel 0 to analog input */
    //HAL_PIN_Set_Analog(PAD_PA28, 1);
    /* find device */
    s_adc_dev = rt_device_find(ADC_DEV_NAME);
    /* set channel 0*/
    read_arg.channel = ADC_DEV_CHANNEL;
    r = rt_adc_enable((rt_adc_device_t)s_adc_dev, read_arg.channel);
    /* will call funtion: sifli_adc_control, and only read once from adc */
    // r = rt_device_control(s_adc_dev, RT_ADC_CMD_READ, &read_arg.channel);
    // LOG_I("adc channel:%d,value:%d", read_arg.channel, read_arg.value); /* (0.1mV), 20846 is 2084.6mV or 2.0846V */
    /* another way to read adc, will call funntion:sifli_get_adc_value,read 22 times from adc and get the average */
    rt_uint32_t value = rt_adc_read((rt_adc_device_t)s_adc_dev, ADC_DEV_CHANNEL);
    LOG_I("rt_adc_read:%d,value:%d", read_arg.channel, value); /* (0.1mV), 20700 is 2070mV or 2.070V */
    /* disable adc */
    rt_adc_disable((rt_adc_device_t)s_adc_dev, read_arg.channel);

	return value;
}

void battery_init(void)
{
	uint32_t adc = battery_read();
	LOG_D("**** TESTING COMMS WITH BATTERY ADC:%d PASS ********\n", adc);
}


static int battery(int argc, char *argv[])
{
    if (argc != 1) {
		LOG_D("usage %s\n", argv[0]);
		return 0;
	}

	uint32_t adc = battery_read();
	LOG_D("read battery adc value:%d\n", adc);
    return 0;
}
MSH_CMD_EXPORT(battery, "read battery adc value")


