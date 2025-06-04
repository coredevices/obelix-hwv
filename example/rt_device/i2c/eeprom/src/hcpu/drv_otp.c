#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "drv_flash.h"
#include "stdio.h"
#include "string.h"
#include "board.h"

#define DBG_TAG "drv_otp"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void opt_init(void)
{
	char* str = "Pebble test flash otp";

	rt_flash_config_write(FACTORY_CFG_ID_USERK3, (uint8_t*)str, strlen(str) + 1);

	uint8_t readbak[64] = {0};
	uint8_t res = rt_flash_config_read(FACTORY_CFG_ID_USERK3, readbak, strlen(str) + 1);
    if ((res == strlen(str) + 1) && strcmp(str, readbak) == 0)
    {
        LOG_D("**** TESTING COMMS WITH OTP: PASS ********\n");
    } else {
		LOG_E("**** TESTING COMMS WITH OTP: FAILED ********\n");
	}
}

static int otp(int argc, char *argv[])
{
    if (argc < 2) {
		LOG_D("usage %s #write/read\n", argv[0]);
		return 0;
	}

	if (strcmp("write", argv[1]) == 0) {
		rt_flash_config_write(FACTORY_CFG_ID_USERK3, (uint8_t*)argv[2], strlen(argv[2]) + 1);
	}
	if (strcmp("read", argv[1]) == 0) {
		uint8_t readbak[64] = {0};
		uint8_t res = rt_flash_config_read(FACTORY_CFG_ID_USERK3, readbak, sizeof(readbak)-1);
		LOG_D("read size:%d, str:%s\n", res, readbak);
	}

	return 0;
}
MSH_CMD_EXPORT(otp, "otp read/write cmd")


