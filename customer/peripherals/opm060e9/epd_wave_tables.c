#include <rtthread.h>
#include "epd_wave_tables.h"
#include "mem_section.h"

#define FRAME_END_LEN       32      //每次刷新所需帧数
/// The image drawing mode.
typedef enum
{

    EPD_DRAW_MODE_INVALID = 0,

    /// Go from any grayscale value to another with a flashing update.
    EPD_DRAW_MODE_GC = 0x1,

    /// Direct Update
    EPD_DRAW_MODE_DU = 0x2,

    /// Draw on a white background
    EPD_DRAW_PREVIOUSLY_WHITE = 0x200,

    /// Draw on a black background
    EPD_DRAW_PREVIOUSLY_BLACK = 0x400,
} EpdDrawMode;
void epd_set_state(EpdDrawMode s);

#ifdef MIXED_REFRESH_METHODS  //混合刷新模式（包括局部刷新和全局刷新），使用新的wave table

#define PART_DISP_TIMES       10        //局刷PART_DISP_TIMES-1次后全刷一次
int reflesh_times = 0;

const unsigned char wave_end[4][FRAME_END_LEN] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, //GC3->GC0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //GC3->GC1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //GC3->GC2
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, //GC3->GC3
};


#else

//反显刷图片 改善边缘扩散专用波形
static const uint8_t wave_end[4][FRAME_END_LEN] =
{
    0, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, //GC3->GC0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //GC3->GC1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //GC3->GC2
    0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, //GC3->GC3
};

#endif

//1像素1bit表示，转为 1像素用2bit表示
const uint8_t OneBitToTwoBit[16] =
{
    0x00,   //0000B
    0x03,   //0001B
    0x0c,   //0010B
    0x0f,   //0011B
    0x30,   //0100B
    0x33,   //0101B
    0x3c,   //0110B
    0x3f,   //0111B

    0xc0,   //1000B
    0xc3,   //1001B
    0xcc,   //1010B
    0xcf,   //1011B
    0xf0,   //1100B
    0xf3,   //1101B
    0xfc,   //1110B
    0xff,   //1111B
};



static uint8_t wave_end_table[256][FRAME_END_LEN] = {0};
static EpdDrawMode epd_cur_state = EPD_DRAW_MODE_GC;



static uint8_t wave_table_2bpp_pixels(uint8_t num, int frame)
{
    uint8_t tmp;

    tmp = 0;
    tmp = wave_end[(num >> 6) & 0x3][frame];

    tmp = tmp << 2;
    tmp |= wave_end[(num >> 4) & 0x3][frame];

    tmp = tmp << 2;
    tmp |= wave_end[(num >> 2) & 0x3][frame];

    tmp = tmp << 2;
    tmp |= wave_end[(num) & 0x3][frame];

    return tmp;
}

unsigned char compare_bits(unsigned char num_byte)
{

    unsigned char result = 0;

    for (int i = 0; i < FRAME_END_LEN; i++)
    {
        unsigned char old_bit = (num_byte >> (7 - i)) & 0x01;
        unsigned char new_bit = (num_byte >> (3 - i)) & 0x01;

        unsigned char bits;
        if (old_bit == new_bit)
        {
            bits = 0x01; // 相同输出01
        }
        else
        {
            if (new_bit == 1)
            {
                bits = 0x03; // 旧0新1
            }
            else
            {
                bits = 0x00; // 旧1新0
            }
        }

        result |= bits << (6 - 2 * i);
    }

    return result;
}


void epd_wave_table(void)
{
    int frame, num;

    //wave_end_table
    for (frame = 0; frame < FRAME_END_LEN; frame++)
    {
        for (num = 0; num < 256; num++)
        {
#ifdef MIXED_REFRESH_METHODS
            uint8_t compare_data = compare_bits(num);
#else
            uint8_t compare_data = num;
#endif /* MIXED_REFRESH_METHODS */
            wave_end_table[num][frame] = wave_table_2bpp_pixels(compare_data, frame);
        }
    }
}

void epd_set_state(EpdDrawMode s)
{
    epd_cur_state = s;
}



uint32_t epd_wave_table_start_flush(void)
{
#ifdef MIXED_REFRESH_METHODS
    if (reflesh_times++ % PART_DISP_TIMES == 0)
    {
        epd_set_state(EPD_DRAW_MODE_GC);
        return FRAME_END_LEN;
    }
    else
    {
        epd_set_state(EPD_DRAW_MODE_DU);
        return 13;
    }
#else
    return FRAME_END_LEN;
#endif /* MIXED_REFRESH_METHODS */
}



L1_RET_CODE_SECT(epd_codes,  void epd_wave_table_convert_i1o2(uint8_t *output_pic_a2,
                 const uint8_t *input_old_pic_a1,
                 const uint8_t *input_new_pic_a1,
                 uint32_t input_bytes, uint8_t frame))
{
    uint8_t idx, ret, oldret;

#ifdef MIXED_REFRESH_METHODS

    if (EPD_DRAW_MODE_GC == epd_cur_state)
    {
        for (int j = 0, i = 0; i < input_bytes; i++)
        {
            ret = input_new_pic_a1[i];
            oldret = input_old_pic_a1[i];

            idx = (~ret & 0xf0) | ((ret >> 4) & 0x0f);
            output_pic_a2[j++] = wave_end_table[idx][frame];

            idx = (~(ret << 4) & 0xf0) | (ret & 0x0f);
            output_pic_a2[j++] = wave_end_table[idx][frame];

        }
    }
    else
    {
        for (int j = 0, i = 0; i < input_bytes; i++)
        {
            ret = input_new_pic_a1[i];
            oldret = input_old_pic_a1[i];

            idx = ((oldret) & 0xF0) | ((ret >> 4) & 0x0F);
            output_pic_a2[j++] = wave_end_table[idx][frame];

            idx = ((oldret << 4) & 0xF0) | (ret & 0x0F);
            output_pic_a2[j++] = wave_end_table[idx][frame];

        }
    }

#else

    for (int j = 0, i = 0; i < input_bytes; i++)
    {
        ret = input_new_pic_a1[i];
        oldret = input_old_pic_a1[i];

        idx = OneBitToTwoBit[(ret >> 4)];
        output_pic_a2[j++] = wave_end_table[idx][frame];

        idx = OneBitToTwoBit[(ret & 0x0f)];
        output_pic_a2[j++] = wave_end_table[idx][frame];
    }


#endif /* MIXED_REFRESH_METHODS */



}