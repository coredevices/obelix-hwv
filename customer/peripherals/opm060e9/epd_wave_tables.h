

#ifndef __EPD_WAVE_TABLES_H__
#define __EPD_WAVE_TABLES_H__

#define MIXED_REFRESH_METHODS



void epd_wave_table(void);//初始化wave table
void epd_wave_table_convert_i1o2(uint8_t *output_pic_a2, const uint8_t *input_old_pic_a1, const uint8_t *input_new_pic_a1, uint32_t input_bytes, uint8_t frame);
uint32_t epd_wave_table_start_flush(void);


#endif