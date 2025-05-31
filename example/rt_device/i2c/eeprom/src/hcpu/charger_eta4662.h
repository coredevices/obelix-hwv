#pragma once

#include <stdint.h>
#include <stddef.h>


#define eta4662_slave_addr          (0xe>>1)
#define i2c_speed                   (400000)

#define eta4662_reg00               0x0
#define eta4662_reg01               0x1
#define eta4662_reg02               0x2
#define eta4662_reg03               0x3
#define eta4662_reg04               0x4
#define eta4662_reg05               0x5
#define eta4662_reg06               0x6
#define eta4662_reg07               0x7
#define eta4662_reg08               0x8
#define eta4662_reg09               0x9
#define eta4662_reg0a               0x0a
#define eta4662_reg0b               0x0b

//reg00
#define vin_dpm                     (0xf)
#define vin_dpm_shift               4
#define iin_limit                   (0xf)
#define iin_limit_shift             0

//reg01
#define trst_dgl                    (0x03)
#define trst_dgl_shift              6
#define trst_dur                    (0x01)
#define trst_dur_shift              5
#define en_hiz                      (0x1)
#define en_hiz_shift                4
#define ceb                         (0x01)
#define ceb_shift                   3
#define vbat_uvlo                   (0x07)
#define vbat_uvlo_shift             0

//reg02
#define reg_reset                   (0x01)
#define reg_reset_shift             7
#define wd_reset                    (0x01)
#define wd_reset_shift              6
#define ichrg                       (0x3f)
#define ichrg_shift                 0

//reg03
#define idschg                      (0x0f)
#define idschg_shift                4
#define iterm                       (0x0f)
#define iterm_shift                 0

//reg04
#define vbat_reg1                    (0x3f)
#define vbat_reg1_shift              2
#define vbat_pre                    (0x01)
#define vbat_pre_shift              1
#define vrech                       (0x01)
#define vrech_shift                 0

//reg05
#define en_wd_dischg                (0x01)
#define en_wd_dischg_shift          7
#define watchdog                    (0x03)
#define watchdog_shift              5
#define en_term                     (0x01)
#define en_term_shift               4
#define en_timer                    (0x01)
#define en_timer_shift              3
#define chg_tmr                     (0x03)
#define chg_tmr_shift               1
#define term_tmr                    (0x01)
#define term_tmr_shift              0

//reg06
#define en_ntc                      (0x01)
#define en_ntc_shift                7
#define tmr2x_en                    (0x01)
#define tmr2x_en_shift              6
#define bfet_dis                    (0x01)
#define bfet_dis_shift              5
#define pg_int_ctl                  (0x01)
#define pg_int_ctl_shift            4
#define eoc_int_ctl                 (0x01)
#define eoc_int_ctl_shift           3
#define chg_stat_int_ctl            (0x01)
#define chg_stat_int_ctl_shift      2
#define ntc_int_ctl                 (0x01)
#define ntc_int_ctl_shift           2
#define battovp_int_ctl             (0x01)
#define battovp_int_ctl_shift       0

//reg07
#define enb_pcb_otp                 (0x01)
#define enb_pch_otp_shift           7
#define dis_vindpm                  (0x01)
#define dis_vindpm_shift            6
#define tj_reg                      (0x03)
#define tj_reg_shift                4
#define vsys_reg                    (0x0f)
#define vsys_reg_shift              0

//REG08
#define wtd_fault                   (0x01)
#define wtd_fault_shift             7
#define no_in_ilim                  (0x01)
#define no_in_ilim_shift            6
#define ilim_add200ma               (0x01)
#define ilim_add200ma_shift         5
#define chg_stat                    (0x03)              //0: 未充电 1：预充 2：充电 3：结束
#define chg_stat_shift              3                   //充电状态
#define ppm_stat                    (0x01)
#define ppm_stat_shift              2                   //输入电压过低或负载电流过大
#define pg_stat                     (0x01)              //0：未接 1：接入
#define pg_stat_shift               1                   //vin状态
#define therm_stat                  (0x01)              //0：正常
#define therm_stat_shift            0                   //芯片过温

//reg09
#define en_ship_dgl                 (0x03)
#define en_ship_dgl_shift           6
#define vin_fault                   (0x01)
#define vin_fault_shift             5
#define them_sd                     (0x01)
#define them_sd_shift               4
#define bat_fault                   (0x01)
#define bat_fault_shift             3
#define stmr_fault                  (0x01)
#define stmr_fault_shift            2
#define ntc_hot                     (0x01)
#define ntc_hot_shift               1
#define ntc_cold                    (0x01)
#define ntc_cold_shift              0

//reg0a
#define addr1                        (0x07)
#define addr_shift                  5
#define cold_reset                  (0x01)
#define cold_reset_shift            4
#define switch_mode                 (0x01)
#define switch_mode_shift           3
#define dis_vdd                     (0x01)
#define dis_vdd_shift               2
#define dis_vinovp                  (0x01)
#define dis_vinovp_shift            1
#define cc_fine                     (0x01)
#define cc_fine_shift               0

//reg0b
#define eta_code                    (0xff)
#define eta_code_shift              0

uint8_t eta4662_read_reg(uint8_t reg, uint8_t *regvalue);
void eta4662_vin_dpm(uint16_t val); 
void eta4662_iin_lim(uint16_t val);
void eta4662_trst_dgl(uint8_t val);
void eta4662_trst_dur(uint8_t val);
void eta4662_en_hiz(uint8_t val);
void eta4662_charger_enable(uint8_t val);
void eta4662_bat_uvlo(uint16_t val);
void eta4662_reg_reset();
void eta4662_wd_reset();
void eta4662_ichrg(uint16_t val);
void eta4662_idschg(uint16_t val);
void eta4662_iterm(uint8_t val);
void eta4662_vbat_reg(uint16_t val);
void eta4662_vbat_pre_2v8(uint8_t val);
void eta4662_vrech_100mv(uint8_t val);
void eta4662_en_wd_dischg(uint8_t val);
void eta4662_wd_time(uint8_t val);
void eta4662_en_term(uint8_t val);
void eta4662_en_timer(uint8_t val);
void eta4662_chg_tmr_h(uint8_t val);
void eta4662_en_term_tmr(uint8_t val);
void eta4662_en_ntc(uint8_t val);
void eta4662_tmr2x_en(uint8_t val);
void eta4662_bfet_dis(uint8_t val);
void eta4662_pg_int_ctl(uint8_t val);
void eta4662_eoc_int_ctl(uint8_t val);
void eta4662_chg_stat_int_ctl(uint8_t val);
void eta4662_ntc_int_ctl(uint8_t val);
void eta4662_battovp_int_ctl(uint8_t val);
void eta4662_en_pcb_otp(uint8_t val);
void eta4662_dis_vindpm(uint8_t val);
void eta4662_tj_reg(uint8_t val);
void eta4662_vsys_reg(uint16_t val);
void eta4662_en_ship_dgl(uint8_t val);
void eta4662_cold_reset(uint8_t val);
void eta4662_switch_mode(uint8_t val);
void eta4662_dis_vdd(uint8_t val);
void eta4662_dis_vinovp(uint8_t val);
void eta4662_cc_fine(uint8_t val);
uint8_t get_eta4662_code(uint8_t *val);




