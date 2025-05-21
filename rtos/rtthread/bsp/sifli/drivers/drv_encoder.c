#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_config.h"
#include "bf0_hal_tim.h"
#include "encoder_config.h"
#include <drivers/rt_drv_encoder.h>
#define LOG_TAG             "drv.encoder"
#include <drv_log.h>

#if defined(BSP_USING_ENCODER1) || defined(_SIFLI_DOXYGEN_)


struct bf0_encoder
{
    struct rt_device_encoder encoder_device;    /*!< Encoder os device */
    GPT_HandleTypeDef tim_handle;       /*!< Encoder low level handle */
    char *name;                         /*!< Encoder device name */
    GPT_Encoder_InitTypeDef encoder_init; /*!< Encoder initialization parameters */
};

static struct bf0_encoder bf0_encoder_obj[] =
{
#ifdef BSP_USING_ENCODER1
    ENCODER1_CONFIG,
#endif
};

static rt_err_t drv_encoder_control(struct rt_device_encoder *device, int cmd, void *arg);
static struct rt_encoder_ops drv_ops =
{
    drv_encoder_control

};


static rt_err_t bf0_hw_encoder_init(struct bf0_encoder *device)
{
    rt_err_t result = RT_EOK;
    GPT_HandleTypeDef *tim = RT_NULL;
    GPT_ClockConfigTypeDef clock_config = {0};


    RT_ASSERT(device != RT_NULL);
    tim = &device->tim_handle;

    tim->Init.Period = 0xFFFF;
    tim->Init.Prescaler = 0;
    tim->Init.CounterMode = GPT_COUNTERMODE_UP;
    tim->Init.RepetitionCounter = 0;

    if (HAL_GPT_Base_Init(tim) != HAL_OK)
    {
        LOG_E("%s time base init failed", device->name);
        result = -RT_ERROR;
    }

    clock_config.ClockSource = GPT_CLOCKSOURCE_INTERNAL;
    if (HAL_GPT_ConfigClockSource(tim, &clock_config) != HAL_OK)
    {
        LOG_E("%s clock config failed", device->name);
        result = -RT_ERROR;

    }

    if (HAL_GPT_Encoder_Init(tim, &device->encoder_init) != HAL_OK)
    {
        LOG_E("%s init failed", device->name);
        result = -RT_ERROR;

    }
    LOG_E("HAL_GPT_Encoder_Init success");
    __HAL_GPT_URS_ENABLE(tim);

    return result;
}

static rt_err_t encoder_enable(GPT_HandleTypeDef *tim, struct rt_encoder_configuration *configuration, rt_bool_t enable)
{

    if (HAL_GPT_Encoder_Start(tim, configuration->channel) != HAL_OK)
    {
        LOG_E("Encoder start failed");
        return -RT_ERROR;
    }
    return RT_EOK;
}

static rt_err_t encoder_disable(GPT_HandleTypeDef *tim, struct rt_encoder_configuration *configuration, rt_bool_t enable)
{
    HAL_GPT_Encoder_Stop(tim, configuration->channel);
    return RT_EOK;
}

static rt_err_t encoder_set_count(GPT_HandleTypeDef *tim, struct rt_encoder_configuration *configuration)
{
    __HAL_GPT_SET_COUNTER(tim, configuration->set_count);
    return RT_EOK;
}

static rt_err_t encoder_get_count(GPT_HandleTypeDef *tim, struct rt_encoder_configuration *configuration)
{
    configuration->get_count = __HAL_GPT_GET_COUNTER(tim);
    return RT_EOK;
}

static rt_err_t encoder_get_speed(GPT_HandleTypeDef *tim, struct rt_encoder_configuration *configuration)
{
    // The speed calculation needs to be implemented according to the specific hardware and application scenarios
    // For example, the change in count per unit time can be calculated by timer interruption or other means
    configuration->speed = 0; // Example value
    return RT_EOK;
}

static rt_err_t drv_encoder_control(struct rt_device_encoder *device, int cmd, void *arg)
{
    struct rt_encoder_configuration *configuration = (struct rt_encoder_configuration *)arg;

    GPT_HandleTypeDef *tim = RT_NULL;
    struct bf0_encoder *encoder = RT_NULL;

    rt_err_t result = RT_EOK;
    RT_ASSERT(device != RT_NULL);

    encoder = (struct bf0_encoder *)device;
    tim = &encoder->tim_handle;

    switch (cmd)
    {
    case PULSE_ENCODER_CMD_ENABLE:
        result = encoder_enable(tim, configuration, RT_TRUE);
        break;
    case PULSE_ENCODER_CMD_DISABLE:
        result = encoder_disable(tim, configuration, RT_FALSE);
        break;
    case PULSE_ENCODER_CMD_GET_COUNT:
        result = encoder_get_count(tim, configuration);
        break;
    case PULSE_ENCODER_CMD_GET_SPEED:
        result = encoder_get_speed(tim, configuration);
        break;
    case PULSE_ENCODER_CMD_SET_COUNT:
        result = encoder_set_count(tim, configuration);
        break;
    default:
        result = -RT_ENOSYS;
        break;
    }

    return result;
}

int  bf0_encoder_init(void)
{

    int result = RT_EOK;
    if (bf0_hw_encoder_init(&bf0_encoder_obj[0]) != RT_EOK)
    {
        LOG_E("%s init failed", bf0_encoder_obj[0].name);
        return -RT_ERROR;
    }

    if (rt_device_pulse_encoder_register(&bf0_encoder_obj[0].encoder_device, bf0_encoder_obj[0].name, &drv_ops, &bf0_encoder_obj[0]) != RT_EOK)
    {
        LOG_D("%s register failed", bf0_encoder_obj[0].name);
        return -RT_ERROR;
    }
    else
    {
        LOG_E("%s register success", bf0_encoder_obj[0].name);

    }
    return result;
}
INIT_DEVICE_EXPORT(bf0_encoder_init);

#endif