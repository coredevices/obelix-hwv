#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "drv_platform.h"
#include "charger_eta4662.h"

#define DBG_TAG "drv_eta4662"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static void charger_int_callback(void *args);

void eta4662_init(void)
{
	LOG_D("eta4662_init i2c");
	i2c_init(ETA4662_I2C_ID);

	uint8_t value;
	rt_size_t ret = get_eta4662_code(&value);
	LOG_D("chip id:0x%x\n", value);
	//rt_size_t ret = i2c_read(ETA4662_I2C_ID, ETA4662_I2C_ADDRESS, 0x0B, &value);
	if (!ret) {
		LOG_D("eta4662 read device id=0x%x\n", value);
		LOG_D("**** TESTING COMMS WITH ETA4662: PASS ********\n");
	} else {
		LOG_E("eta4662 read device id error.\n");
		LOG_E("**** TESTING COMMS WITH ETA4662: FAILED ********\n");
	}

	//2. initialize the configuration
	//进芯片最低电压
    eta4662_vin_dpm(4600);
    //进芯片最大电流
    eta4662_iin_lim(1000);
    //vbat保护电压
    eta4662_bat_uvlo(2940);
    //进电池电流
    eta4662_ichrg(200);
    //电池放电电流最大值mA
    eta4662_idschg(1600);
    //充电截至电流
    eta4662_iterm(10);
    //使能截至电流功能
    eta4662_en_term(1);
    //充电截至电压
    eta4662_vbat_reg(4350);
    //复充电压 下降值：1：100mV 0：200mV
    eta4662_vrech_100mv(0);
    //预充电压阈值，0：3V；1：2.8V
    eta4662_vbat_pre_2v8(0);
    //看门狗复位时间; 0为关闭; 是能需要喂狗 eta4662_wd_reset
    eta4662_wd_time(0);
    //安全充电计时器；超时停止充电
    eta4662_chg_tmr_h(5);
    //安全计时器使能 1：enalbe; 0:disalbe
    eta4662_en_timer(0);
    //NTC开发；0开闭；1打开
    eta4662_en_ntc(0);
	eta4662_dis_vdd(1);
	//过温保护；0：关 1：开
    eta4662_en_pcb_otp(0);
    //充电状态下vsys；需比电池电压高
    eta4662_vsys_reg(4500);
    //使能充电状态变化中断
    eta4662_chg_stat_int_ctl(0);
    //设置硬复位按键时长
    eta4662_trst_dgl(16);
    //设置赢复位断电时长
    eta4662_trst_dur(2);
    //进入船运模式delay时间
    eta4662_en_ship_dgl(5);
    //使能充电
    eta4662_charger_enable(1);
	//进入开关模式
	//eta4662_switch_mode(1);

	
	//3.todo:reg int pin
	HAL_PIN_Set(PAD_PA26, GPIO_A26, PIN_PULLUP, 1);
	rt_pin_mode(26, PIN_MODE_INPUT);
	rt_pin_attach_irq(26, PIN_IRQ_MODE_RISING_FALLING, charger_int_callback, (void *)(rt_uint32_t)26);
    rt_pin_irq_enable(26, ENABLE);
}

static void charger_int_callback(void *args)
{
    //set your own irq handle
    rt_kprintf("charger Interrupt!\n");
	LOG_D("**** TESTING COMMS WITH ETA4662 INTERRUPT: PASS ********\n");
}



static int eta4662_set(int argc, char *argv[])
{
    if (argc < 2) {
		LOG_D("usage %s #switch mode; #ship_mode;\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "switch_mode") == 0) {
		LOG_D("work in switch mode\n");
		eta4662_switch_mode(1);
	} 
	if (strcmp(argv[1], "ship_mode") == 0){
		LOG_D("enter to ship mode\n");
		eta4662_bfet_dis(1);
	}
    return 0;
}
MSH_CMD_EXPORT(eta4662_set, "set eta4662 mode")


