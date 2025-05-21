#include <rtthread.h>
#include <string.h>
#include <stdio.h>
#include <board.h>
#include <bf0_hal_tim.h>
#include <drivers/rt_drv_encoder.h>
#include "drv_io.h"

#define ENCODER_DEVICE_NAME "encoder1"

struct rt_device *encoder_device;

rt_err_t encoder_example_init(void)
{
    rt_err_t result;


#ifdef SF32LB52X
    HAL_PIN_Set(PAD_PA24, GPTIM1_CH1, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA25, GPTIM1_CH2, PIN_PULLUP, 1);
#elif defined SF32LB58X
    HAL_PIN_Set(PAD_PA82, GPTIM1_CH1, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA51, GPTIM1_CH2, PIN_PULLUP, 1);
#endif

    // Find encoder
    encoder_device = rt_device_find(ENCODER_DEVICE_NAME);

    if (encoder_device == RT_NULL)
    {
        rt_kprintf("Failed to find %s device\n", ENCODER_DEVICE_NAME);
        return -RT_ERROR;
    }

    // Starting encoder
    struct rt_encoder_configuration config;
    config.channel = GPT_CHANNEL_ALL;

    result = rt_device_control((struct rt_device *)encoder_device, PULSE_ENCODER_CMD_ENABLE, (void *)&config);//使能

    if (result != RT_EOK)
    {
        rt_kprintf("Failed to enable encoder\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}

void Get_count(void)
{

    rt_err_t result;
    struct rt_encoder_configuration config_count;
    config_count.get_count = 0;
    // Gets the current count value
    result = rt_device_control((struct rt_device *)encoder_device, PULSE_ENCODER_CMD_GET_COUNT, (void *)&config_count);
    if (result != RT_EOK)
    {
        rt_kprintf("Failed to get encoder count\n");
    }
    else
    {
        rt_kprintf("encoder_count:%d\n", config_count.get_count);
    }

}


int main(void)
{

    rt_err_t result;
    result = encoder_example_init();
    if (result != RT_EOK)
    {
        rt_kprintf("Failed encoder_example_init\n");
    }
    else
    {
        rt_kprintf("succeed encoder_example_init\n");
    }
    rt_kprintf("Start Get_count!\n");
    while (1)
    {
        Get_count();
        rt_thread_mdelay(1000);
    }
    return 0;

}