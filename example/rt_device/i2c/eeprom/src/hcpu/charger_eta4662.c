#include "charger_eta4662.h"
#include "drv_platform.h"
#include "drv_eta4662.h"

/////////////////////////////////////////////////////////////////////////////

// @brief 总线初始化
int32_t eta4662_bus_init(uint32_t freq)
{
    return 0;
}

// @brief 总线去初始化
int32_t eta4662_bus_deinit(void)
{
    return 0;
}

// @brief 打开总线驱动
void eta4662_bus_start(void)
{
    //sdrv_i2c_bus_start(m_dev_bus);
}

// @brief 关闭总线驱动
void eta4662_bus_stop(void)
{
    //sdrv_i2c_bus_stop(m_dev_bus);
}

// @brief 总线读数据
static int32_t eta4662_bus_data_read(uint8_t slave_addr, uint8_t reg, uint8_t* p_data, uint16_t data_size)
{
	return i2c_read(ETA4662_I2C_ID, ETA4662_I2C_ADDRESS, reg, p_data);
}

// @brief 总线写数据
static int32_t eta4662_bus_data_write(uint8_t slave_addr, uint8_t reg, const uint8_t* p_data, uint16_t data_size)
{
	return i2c_write(ETA4662_I2C_ID, ETA4662_I2C_ADDRESS, reg, *p_data);
}




/////////////////////////////////////////////////////////////////////////////
uint8_t eta4662_read_reg(uint8_t reg, uint8_t *regvalue)
{
    int32_t ret = eta4662_bus_data_read(eta4662_slave_addr, reg, regvalue, 1);
    return ret < 0 ? 1 : 0;
}

static uint8_t eta4662_write_reg(uint8_t reg, uint8_t val)
{
    eta4662_bus_data_write(eta4662_slave_addr, reg, &val, sizeof(uint8_t));
    return 0;
}

static uint8_t eta4662_set_value(uint8_t reg, uint8_t reg_bit,uint8_t reg_shift, uint8_t val)
{
    uint8_t tmp;
    eta4662_read_reg(reg, &tmp);
    tmp = (tmp &(~(reg_bit<< reg_shift))) |(val << reg_shift);
    eta4662_write_reg(reg,tmp);
    return tmp;
}

static uint8_t eta4662_get_value(uint8_t reg, uint8_t reg_bit, uint8_t reg_shift)
{
    uint8_t data = 0;
    uint8_t ret = 0 ;
    ret = eta4662_read_reg(reg, &data);
    ret = (data & reg_bit) >> reg_shift;
    return ret;
}

void eta4662_vin_dpm(uint16_t val)
{
    uint8_t regval;
    regval=(val-3880)/80;
    eta4662_set_value(eta4662_reg00,vin_dpm,vin_dpm_shift,regval);
}
 
void eta4662_iin_lim(uint16_t val)
{
    uint8_t regval;
    regval=(val-50)/30; 
    eta4662_set_value(eta4662_reg00,iin_limit,iin_limit_shift,regval);
}
void eta4662_trst_dgl(uint8_t val)
{
    uint8_t regval;
    if (val <= 8) {
        regval = 0;
    } else if (val > 8 && val <= 12) {
        regval = 1;
    } else if (val > 12 && val <= 16) {
        regval = 2;
    } else if (val > 16) {
        regval = 3;
    }
    eta4662_set_value(eta4662_reg01,trst_dgl,trst_dgl_shift,regval);
}

void eta4662_trst_dur(uint8_t val)
{
    uint8_t regval;
    if (val <= 2) {
        regval = 0;
    } else {
        regval = 1;
    }
    eta4662_set_value(eta4662_reg01,trst_dur,trst_dur_shift,regval);
}

void eta4662_en_hiz(uint8_t val)
{
    eta4662_set_value(eta4662_reg01,en_hiz,en_hiz_shift,1);
}

void eta4662_charger_enable(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg01,ceb,ceb_shift,0);
    else
        eta4662_set_value(eta4662_reg01,ceb,ceb_shift,1);
}

void eta4662_bat_uvlo(uint16_t val)
{
    uint8_t regval;
    if (val <= 2400)
        regval = 0;
    else if (val >= 3030)
        regval = 7;
    else
        regval = (val - 2400) / 90;
    eta4662_set_value(eta4662_reg01,vbat_uvlo,vbat_uvlo_shift,regval);
}
 
void eta4662_reg_reset()
{
    eta4662_set_value(eta4662_reg02,reg_reset,reg_reset_shift,1);
}

void eta4662_wd_reset()
{
    eta4662_set_value(eta4662_reg02,wd_reset,wd_reset_shift,1);
}

void eta4662_ichrg(uint16_t val)
{
    uint8_t regval;
    if (val <= 8) {
        regval = 0;
    } else if (val >= 512) {
        regval = 0x3f;
    } else {
        regval = (val - 8) / 8;
    }
    eta4662_set_value(eta4662_reg02,ichrg,ichrg_shift,regval);
}

void eta4662_idschg(uint16_t val)
{
    uint8_t regval;
    if (val <= 200) {
        regval = 0;
    } else if (val >= 3200) {
        regval = 0xf;
    } else {
        regval = (val - 200) / 200;
    }
    eta4662_set_value(eta4662_reg03,idschg,idschg_shift,regval);
}

void eta4662_iterm(uint8_t val)
{
    uint8_t regval;
    if (val <= 1) {
        regval = 0;
    } else if (val >= 31) {
        regval = 0xf;
    } else {
        regval = (val - 1) / 2;
    }
    eta4662_set_value(eta4662_reg03,iterm,iterm_shift,regval);
}

void eta4662_vbat_reg(uint16_t val)
{
    uint8_t regval;
    if (val <= 3600) {
        regval = 0;
    } else if (val >= 4545) {
        regval = 0x3f;
    } else {
        regval = (val - 3600) / 15;
    }
    eta4662_set_value(eta4662_reg04,vbat_reg1,vbat_reg1_shift,regval);
}

void eta4662_vbat_pre_2v8(uint8_t val)
{
    if (val) {
        eta4662_set_value(eta4662_reg04,vbat_pre,vbat_pre_shift,0);
    } else {
        eta4662_set_value(eta4662_reg04,vbat_pre,vbat_pre_shift,1);
    }
}

void eta4662_vrech_100mv(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg04,vrech,vrech_shift,0);
    else
        eta4662_set_value(eta4662_reg04,vrech,vrech_shift,1);
}

void eta4662_en_wd_dischg(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg05,en_wd_dischg,en_wd_dischg_shift,1);
    else
        eta4662_set_value(eta4662_reg05,en_wd_dischg,en_wd_dischg_shift,0);
}

void eta4662_wd_time(uint8_t val)
{
    uint8_t regval;
    if (val == 0) {
        regval = 0;
    } else if (val < 40) {
        regval = 1;
    } else if (val > 40 && val <= 80) {
        regval = 2;
    } else {
        regval = 3;
    }
    eta4662_set_value(eta4662_reg05, watchdog, watchdog_shift,regval);
}

void eta4662_en_term(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg05,en_term,en_term_shift,1);
    else
        eta4662_set_value(eta4662_reg05,en_term,en_term_shift,1);
}

void eta4662_en_timer(uint8_t val)
{
    if (val) {
        eta4662_set_value(eta4662_reg05,en_timer,en_timer_shift,1);
    } else {
        eta4662_set_value(eta4662_reg05,en_timer,en_timer_shift,0);
    }
}

void eta4662_chg_tmr_h(uint8_t val)
{
    uint8_t regval;
    if (val <= 3) {
        regval = 0;
    } else if (val > 5 && val <= 8) {
        regval = 2;
    } else {
        regval = 1;
    }
    eta4662_set_value(eta4662_reg05,chg_tmr,chg_tmr_shift,regval);
}

void eta4662_en_term_tmr(uint8_t val)
{
    if (val) {
        eta4662_set_value(eta4662_reg05,term_tmr,term_tmr_shift,1);
    } else {
        eta4662_set_value(eta4662_reg05,term_tmr,term_tmr_shift,0);
    }
}

void eta4662_en_ntc(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,en_ntc,en_ntc_shift,1);
    else
        eta4662_set_value(eta4662_reg06,en_ntc,en_ntc_shift,0);
}

void eta4662_tmr2x_en(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,tmr2x_en,tmr2x_en_shift,1);
    else
        eta4662_set_value(eta4662_reg06,tmr2x_en,tmr2x_en_shift,0);
}

void eta4662_bfet_dis(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,bfet_dis,bfet_dis_shift,0);
    else
        eta4662_set_value(eta4662_reg06,bfet_dis,bfet_dis_shift,1);
}

void eta4662_pg_int_ctl(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,pg_int_ctl,pg_int_ctl_shift,1);
    else
        eta4662_set_value(eta4662_reg06,pg_int_ctl,pg_int_ctl_shift,0);
}

void eta4662_eoc_int_ctl(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,eoc_int_ctl,eoc_int_ctl_shift,1);
    else
        eta4662_set_value(eta4662_reg06,eoc_int_ctl,eoc_int_ctl_shift,0);
}

void eta4662_chg_stat_int_ctl(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,chg_stat_int_ctl,chg_stat_int_ctl_shift,1);
    else
        eta4662_set_value(eta4662_reg06,chg_stat_int_ctl,chg_stat_int_ctl_shift,0);
}

void eta4662_ntc_int_ctl(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,ntc_int_ctl,ntc_int_ctl_shift,1);
    else
        eta4662_set_value(eta4662_reg06,ntc_int_ctl,ntc_int_ctl_shift,0);
}

void eta4662_battovp_int_ctl(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg06,battovp_int_ctl,battovp_int_ctl_shift,1);
    else
        eta4662_set_value(eta4662_reg06,battovp_int_ctl,battovp_int_ctl_shift,0);
}

void eta4662_en_pcb_otp(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg07,enb_pcb_otp,enb_pch_otp_shift,0);
    else
        eta4662_set_value(eta4662_reg07,enb_pcb_otp,enb_pch_otp_shift,1);
}

void eta4662_dis_vindpm(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg07,dis_vindpm,dis_vindpm_shift,0);
    else
        eta4662_set_value(eta4662_reg07,dis_vindpm,dis_vindpm_shift,1);
}

void eta4662_tj_reg(uint8_t val)
{
    uint8_t regval;
    if (val <= 60) {
        regval = 0;
    } else if (val > 60 && val <= 80) {
        regval = 1;
    } else if (val > 80 && val <= 100) {
        regval = 2;
    } else {
        regval = 3;
    }
    eta4662_set_value(eta4662_reg07,tj_reg,tj_reg_shift,regval);
}

void eta4662_vsys_reg(uint16_t val)
{
    uint8_t regval;
    if (val <= 4200) {
        regval = 0;
    } else if (val >= 4950) {
        regval = 0xf;
    } else {
        regval = (val - 4200) / 50;
    }
    eta4662_set_value(eta4662_reg07,vsys_reg,vsys_reg_shift,regval);
}

void eta4662_en_ship_dgl(uint8_t val)
{
    uint8_t regval;
    if (val <= 1) {
        regval = 0;
    } else if (val > 1 && val <= 2) {
        regval = 1;
    } else if (val > 2 && val <= 4) {
        regval = 2;
    } else if (val > 4 && val <= 8) {
        regval = 3;
    } else {
        regval = 1;
    }
    eta4662_set_value(eta4662_reg09,en_ship_dgl,en_ship_dgl_shift,regval);
}

void eta4662_cold_reset(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg0a,cold_reset,cold_reset_shift,1);
    else
        eta4662_set_value(eta4662_reg0a,cold_reset,cold_reset_shift,0);
}

void eta4662_switch_mode(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg0a,switch_mode,switch_mode_shift,1);
    else
        eta4662_set_value(eta4662_reg0a,switch_mode,switch_mode_shift,0);
}

void eta4662_dis_vdd(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg0a,dis_vdd,dis_vdd_shift,1);
    else
        eta4662_set_value(eta4662_reg0a,dis_vdd,dis_vdd_shift,0);
}

void eta4662_dis_vinovp(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg0a,dis_vinovp,dis_vinovp_shift,0);
    else
        eta4662_set_value(eta4662_reg0a,dis_vinovp,dis_vinovp_shift,1);
}

void eta4662_cc_fine(uint8_t val)
{
    if (val)
        eta4662_set_value(eta4662_reg0a,cc_fine,cc_fine_shift,0);
    else
        eta4662_set_value(eta4662_reg0a,cc_fine,cc_fine_shift,1);
}

uint8_t get_eta4662_code(uint8_t* val)
{
    return eta4662_read_reg(eta4662_reg0b,val);
}

