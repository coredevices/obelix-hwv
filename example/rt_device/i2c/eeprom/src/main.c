#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"

#define DBG_TAG "PebbleTest"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* i2c  */

static struct rt_i2c_bus_device *i2c_bus1 = NULL;
static struct rt_i2c_bus_device *i2c_bus2 = NULL;
static struct rt_i2c_bus_device *i2c_bus3 = NULL;
static struct rt_i2c_bus_device *i2c_bus4 = NULL;


/* i2c */
//Version 5.0

#define EEPROM_I2C_ADDRESS         	0x50  // 7bit device address of EEPROM
#define AW9527_I2C_ADDRESS			0X5B
#define CST816_I2C_ADDRESS			0X15
#define GH3026_I2C_ADDRESS			0X58
#define ETA4662_I2C_ADDRESS			0X07
#define AW86225_I2C_ADDRESS			0X58
#define LS6DSOW_I2C_ADDRESS			0X6A
#define MMC5603_I2C_ADDRESS			0X5B
#define W1160_I2C_ADDRESS			0X5B


typedef enum {
    IOEXP_HIGH,
    IOEXP_LOW,
}IOEXP_STATE_T;

typedef enum {
    IOEXP_CH00 = 0,
    IOEXP_CH01,
    IOEXP_CH02,
    IOEXP_CH03,
    IOEXP_CH04,
    IOEXP_CH05,
    IOEXP_CH06,
    IOEXP_CH07,
    IOEXP_CH10 = 10,
    IOEXP_CH11,
    IOEXP_CH12,
    IOEXP_CH13,
    IOEXP_CH14,
    IOEXP_CH15,
    IOEXP_CH16,
    IOEXP_CH17,
}IOEXP_CHANNEL_T;


static uint8_t i2c_init(uint8_t id)
{
	struct rt_i2c_bus_device * i2c[] = {NULL, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};
		//1. pin mux
#ifdef SF32LB52X
	switch(id) {
	case 1:
		HAL_PIN_Set(PAD_PA11, I2C1_SCL, PIN_PULLUP, 1); // i2c io select
		HAL_PIN_Set(PAD_PA10, I2C1_SDA, PIN_PULLUP, 1);
		i2c_bus1 = rt_i2c_bus_device_find("i2c1");
		rt_kprintf("i2c_bus1:0x%x\n", i2c_bus1);
		break;
	case 2:
		HAL_PIN_Set(PAD_PA29, I2C2_SCL, PIN_PULLUP, 1); // i2c io select
		HAL_PIN_Set(PAD_PA28, I2C2_SDA, PIN_PULLUP, 1);
		i2c_bus2 = rt_i2c_bus_device_find("i2c2");
		rt_kprintf("i2c_bus2:0x%x\n", i2c_bus2);
		break;
	case 3:
		HAL_PIN_Set(PAD_PA21, I2C3_SCL, PIN_PULLUP, 1); // i2c io select
		HAL_PIN_Set(PAD_PA20, I2C3_SDA, PIN_PULLUP, 1);
		i2c_bus3 = rt_i2c_bus_device_find("i2c3");
		rt_kprintf("i2c_bus3:0x%x\n", i2c_bus3);
		break;
	case 4:
		HAL_PIN_Set(PAD_PA31, I2C4_SCL, PIN_PULLUP, 1); // i2c io select
		HAL_PIN_Set(PAD_PA30, I2C4_SDA, PIN_PULLUP, 1);
		i2c_bus4 = rt_i2c_bus_device_find("i2c4");
		rt_kprintf("i2c_bus4:0x%x\n", i2c_bus4);
		break;
#elif defined(SF32LB58X)
		HAL_PIN_Set(PAD_PB28, I2C6_SCL, PIN_PULLUP, 0); // i2c io select
		HAL_PIN_Set(PAD_PB29, I2C6_SDA, PIN_PULLUP, 0);
#endif
	}
	
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

static HAL_StatusTypeDef i2c_read(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t* value)
{
	struct rt_i2c_bus_device * i2c[] = {NULL, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};

	HAL_StatusTypeDef ret = HAL_ERROR;
    uint8_t buf[2];

	if (i2c[i2c_id]) {
	    ret = rt_i2c_mem_read(i2c[i2c_id], device_addr, reg, 8, value, 1);  // ret: return data size
	    LOG_D("i2c[%d] read device:0x%x reg:0x%x,pdata:0x%x,ret:%d\n", i2c_id, device_addr, reg, *value, ret);
	}
	
	return ret;
}

static rt_size_t i2c_write(uint8_t i2c_id, uint8_t device_addr, uint8_t reg, uint8_t value)
{
	struct rt_i2c_bus_device * i2c[] = {NULL, i2c_bus1, i2c_bus2, i2c_bus3, i2c_bus4};

	rt_size_t ret = 0;
    uint8_t buf[2];

	if (i2c[i2c_id]) {
    	ret = rt_i2c_mem_write(i2c[i2c_id], device_addr, reg, 8, &value, 1);  // ret: return data size
    	LOG_D("i2c[%d] write device:0x%x reg:0x%x,pdata:0x%x,ret:%d\n", i2c_id, device_addr, reg, value, ret);
	}

	return ret;
}

static void gpio_set(int pin, int val, int is_porta)
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

static void ioexp_init(void)
{
	LOG_D("ioexp_init gpio");
	gpio_set(44, 1, 1);
	LOG_D("ioexp_init i2c");
	i2c_init(2);

	uint8_t value;
	rt_size_t ret = i2c_read(2, AW9527_I2C_ADDRESS, 0x10, &value);
	if (ret) {
		LOG_D("aw9527 read device id=0x%x\n", value);
	} else {
		LOG_E("aw9527 read device id error.\n");
	}
}


/*channel ctrl*/
static void ioexp_pin_set(IOEXP_CHANNEL_T channel, IOEXP_STATE_T state) {
    if (channel >= IOEXP_CH10) {
        uint8_t p1_value;
        i2c_read(2, AW9527_I2C_ADDRESS, 0x03, &p1_value);
        if (state == IOEXP_HIGH) {
            p1_value |= (1<<channel);
        } else {
            p1_value &= ~(1<<channel);
        }
        i2c_write(2, AW9527_I2C_ADDRESS, 0x03, p1_value);
    } else {
        uint8_t p0_value;
        i2c_read(2, AW9527_I2C_ADDRESS, 0x02, &p0_value);
        if (state == IOEXP_HIGH) {
            p0_value |= (1<<channel);
        } else {
            p0_value &= ~(1<<channel);
        }
        i2c_write(2, AW9527_I2C_ADDRESS, 0x02, p0_value);
    }
}


uint8_t g_main_flag = 0;
/**
  * @brief  Main program
  * @param  None
  * @retval 0 if success, otherwise failure number
  */
int main(void)
{
    rt_kprintf("main!!\n");
    //LOG_D("Start i2c eeprom rtt demo!\n"); // Output a start message on console using printf function
    //EEPROM_example();
    //LOG_D("i2c end!\n"); // Output a end message on console using printf function

	LOG_D("Ready for Pebble CT2_DEV_Kit testing.\n");
	HAL_RCC_EnableModule(RCC_MOD_GPIO1); // GPIO clock enable
	ioexp_init();
		
    while (1)
    {
        rt_thread_mdelay(5000);
        //rt_kprintf("__main loop__\r\n");
    }

    return RT_EOK;
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
MSH_CMD_EXPORT(ioexp_set, "trigger notification to client")


#if 0
/***************************************************
EEPROM interfaces
****************************************************/

uint8_t RECEIVED = 0;
uint8_t TEST_ADDR[] =
{
    0x01,
    0x02,
    0x03,
    0x04
};
uint8_t TEST_DATA[] =
{
    0x11,
    0x22,
    0x33,
    0x44
};


/**
 * @brief Initialization work before power on EEPROM and
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return bool:if true,the operation is successful;otherwise is failed
 * @retval
 */
unsigned char EEPROM_init(void)
{

    uint8_t slaveAddr = EEPROM_I2C_ADDRESS; // 7bit address of device
    HAL_StatusTypeDef ret;

    //1. pin mux
#ifdef SF32LB52X
    HAL_PIN_Set(PAD_PA41, I2C2_SCL, PIN_PULLUP, 1); // i2c io select
    HAL_PIN_Set(PAD_PA42, I2C2_SDA, PIN_PULLUP, 1);
#elif defined(SF32LB58X)
    HAL_PIN_Set(PAD_PB28, I2C6_SCL, PIN_PULLUP, 0); // i2c io select
    HAL_PIN_Set(PAD_PB29, I2C6_SDA, PIN_PULLUP, 0);
#endif

    // 2. i2c init
    // find i2c2
#ifdef SF32LB52X
    i2c_bus = rt_i2c_bus_device_find("i2c2");
#elif defined(SF32LB58X)
    i2c_bus = rt_i2c_bus_device_find("i2c6");
#endif

    rt_kprintf("i2c_bus:0x%x\n", i2c_bus);
    if (i2c_bus)
    {
#ifdef SF32LB52X
        rt_kprintf("Find i2c bus device I2C2\n");
#elif defined(SF32LB58X)
        rt_kprintf("Find i2c bus device I2C6\n");
#endif
        // open rt_device i2c2
        rt_device_open((rt_device_t)i2c_bus, RT_DEVICE_FLAG_RDWR);
        //rt_i2c_open(i2c_bus, RT_DEVICE_FLAG_RDWR);
        struct rt_i2c_configuration configuration =
        {
            .mode = 0,
            .addr = 0,
            .timeout = 500, //Waiting for timeout period (ms)
            .max_hz = 400000, //I2C rate (hz)
        };
        // config I2C parameter
        rt_i2c_configure(i2c_bus, &configuration);
    }
    else
    {
#ifdef SF32LB52X
        LOG_E("Can not found i2c bus I2C2, init fail\n");
#elif defined(SF32LB58X)
        LOG_E("Can not found i2c bus I2C6, init fail\n");
#endif
        return -1;
    }
    return 0;
}

void EEPROM_write_data(uint8_t addr, uint8_t data)
{
    HAL_StatusTypeDef ret;
    ret = rt_i2c_mem_write(i2c_bus, EEPROM_I2C_ADDRESS, addr, 8, &data, 1);
    LOG_D("i2c write reg:0x%x,data:0x%x,ret:%d\n", addr, data, ret);
}




void EEPROM_read_data(uint8_t addr, uint8_t *pdata)
{
    HAL_StatusTypeDef ret;
    uint8_t buf[2];

    ret = rt_i2c_mem_read(i2c_bus, EEPROM_I2C_ADDRESS, addr, 8, pdata, 1);  // ret: return data size
    LOG_D("i2c read reg:0x%x,pdata:0x%x,ret:%d\n", addr, *pdata, ret);
}

void delayms(unsigned short int ms)
{
    HAL_Delay(ms);
}


/// @brief read and write eeprom to test
/// @param
void EEPROM_test(void)
{
    unsigned char i = 0;

    for (i = 0; i < 4; i++)
    {
        EEPROM_write_data(TEST_ADDR[i], TEST_DATA[i]);
        delayms(5); //5ms delay for AT240C8SC write time cycle
    }

    for (i = 0; i < 4; i++)
    {
        EEPROM_read_data(TEST_ADDR[i], &RECEIVED);
    }
}




void  EEPROM_example(void)
{
    EEPROM_init();
    EEPROM_test();
}
#endif

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/

