#include <stdint.h>
#include <string.h>
#include "bf0_hal.h"
#include "mem_map.h"
#include "register.h"
#include "bf0_hal_patch.h"
#ifdef HAL_LCPU_PATCH_MODULE
static const unsigned int g_lcpu_patch_list[] =
{
    0x50544348, 0x00000010, 0x00057830, 0x63682032,
    0x000062C8, 0xBEA2F3FE,
};

static const unsigned int g_lcpu_patch_bin[] =
{
    0xB872F000, 0xB802F000, 0x00004770, 0x460CB5B0,
    0xF7FB4605, 0x6829F0B1, 0x6869B109, 0x4629E000,
    0x2100600C, 0x6021606C, 0x40B0E8BD, 0xB0A8F7FB,
    0x6809491A, 0x0020F851, 0x8D01B138, 0xF8B0B129,
    0x42810052, 0xF400BF18, 0x4770B8FD, 0x41F0E92D,
    0x460E4615, 0xF7FB4607, 0xF8DFF08F, 0x46048040,
    0x0000F8D8, 0x1027F850, 0xF891B199, 0xB1500057,
    0xB1288D08, 0xF8E6F400, 0x0000F8D8, 0x1027F850,
    0x854E850D, 0x4638E005, 0x462A4631, 0xF4392301,
    0x4620F859, 0x41F0E8BD, 0xB072F7FB, 0x204002FC,
    0x280DB510, 0xF04F4614, 0xBF080200, 0xD1032910,
    0xF7FFB2D8, 0x2201FFBD, 0xBD107022, 0x2400B510,
    0x7014280D, 0xF5B1BF08, 0xD0007F80, 0xE9DDBD10,
    0xB2D81202, 0xB292B2C9, 0x4010E8BD, 0xBFB6F7FF,
    0x70102000, 0x00004770, 0x49094808, 0x48096001,
    0x60014909, 0x490A4809, 0x480A6001, 0xF8C02104,
    0xF44F10C4, 0x63417188, 0xBF004770, 0x20400198,
    0x004050AD, 0x204001B8, 0x004050ED, 0x204001A8,
    0x004050C9, 0x204008A8,
};
void lcpu_patch_install_rev_b()
{
    uint32_t entry[3] = {0x48434150, 0x3, LCPU_PATCH_CODE_START_ADDR + 1};
    memcpy((void *)LCPU_PATCH_BUF_START_ADDR, (void *)&entry, 12);
#ifdef SOC_BF0_HCPU
    memset((void *)(LCPU_PATCH_CODE_START_ADDR_S), 0, LCPU_PATCH_CODE_SIZE);
    memcpy((void *)(LCPU_PATCH_CODE_START_ADDR_S), g_lcpu_patch_bin, sizeof(g_lcpu_patch_bin));
#else
    memset((void *)(LCPU_PATCH_CODE_START_ADDR), 0, LCPU_PATCH_CODE_SIZE);
    memcpy((void *)(LCPU_PATCH_CODE_START_ADDR), g_lcpu_patch_bin, sizeof(g_lcpu_patch_bin));
#endif
    HAL_PATCH_install();
}
uint32_t *HAL_PATCH_GetEntryAddr(void)
{
    uint32_t *entry_addr = (uint32_t *)LCPU_PATCH_RECORD_ADDR;
    uint8_t rev_id = __HAL_SYSCFG_GET_REVID();
    if (rev_id >= HAL_CHIP_REV_ID_A4)
        entry_addr = (uint32_t *)g_lcpu_patch_list;
    return entry_addr;
}
#endif
