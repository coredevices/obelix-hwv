#include "bsp_board.h"


void BSP_GPIO_Set(int pin, int val, int is_porta)
{
    GPIO_TypeDef *gpio = (is_porta) ? hwp_gpio1 : hwp_gpio2;
    GPIO_InitTypeDef GPIO_InitStruct;

    // set sensor pin to output mode
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);

    // set sensor pin to high == power on sensor board
    HAL_GPIO_WritePin(gpio, pin, (GPIO_PinState)val);
}

void BSP_GPIO_SetInt(int pin, int val, int is_porta)
{
    GPIO_TypeDef *gpio = (is_porta) ? hwp_gpio1 : hwp_gpio2;
    GPIO_InitTypeDef GPIO_InitStruct;

    // set sensor pin to output mode
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);

    // set sensor pin to high == power on sensor board
    HAL_GPIO_WritePin(gpio, pin, (GPIO_PinState)val);
}


#define MPI2_POWER_PIN  (11)

HAL_RAM_RET_CODE_SECT(BSP_PowerDownCustom, void BSP_PowerDownCustom(int coreid, bool is_deep_sleep))
{
    //BSP_GPIO_Set(MPI2_POWER_PIN, 0, 1);
#ifdef BF0_HCPU
	extern int rt_psram_enter_low_power(char *name);
	rt_psram_enter_low_power("psram1");

    FLASH_HandleTypeDef *flash_handle;
	extern void *rt_flash_get_handle_by_addr(uint32_t addr);
	flash_handle =(FLASH_HandleTypeDef *)rt_flash_get_handle_by_addr(MPI2_MEM_BASE);
	HAL_FLASH_DEEP_PWRDOWN(flash_handle);
	HAL_Delay_us(3);
#endif
}

HAL_RAM_RET_CODE_SECT(BSP_PowerUpCustom, void BSP_PowerUpCustom(bool is_deep_sleep))
{
#ifdef BF0_HCPU
	extern int rt_psram_exit_low_power(char *name);
	rt_psram_exit_low_power("psram1");

    //BSP_GPIO_Set(MPI2_POWER_PIN, 1, 1);
    FLASH_HandleTypeDef *flash_handle;
	extern void *rt_flash_get_handle_by_addr(uint32_t addr);
	flash_handle =(FLASH_HandleTypeDef *)rt_flash_get_handle_by_addr(MPI2_MEM_BASE);
	HAL_FLASH_RELEASE_DPD(flash_handle);
	HAL_Delay_us(25);
#endif
}


void BSP_Power_Up(bool is_deep_sleep)
{	
    BSP_PowerUpCustom(is_deep_sleep);

	//CHG 
	HAL_PIN_Set(PAD_PA26, GPIO_A26, PIN_PULLUP, 1);


}



void BSP_IO_Power_Down(int coreid, bool is_deep_sleep)
{
#if 0
	//UART1
	HAL_PIN_Set(PAD_PA18, GPIO_A18, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(18, 0, 1);
	HAL_PIN_Set(PAD_PA19, GPIO_A19, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(19, 0, 1);
#endif
	//pa ctrl
	HAL_PIN_Set(PAD_PA00, GPIO_A0, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(0, 0, 1);

	//Motor rst
	HAL_PIN_Set(PAD_PA01, GPIO_A1, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(1, 0, 1);

	//JDI
	HAL_PIN_Set(PAD_PA02, GPIO_A2, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(2, 0, 1);
	HAL_PIN_Set(PAD_PA03, GPIO_A3, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(3, 0, 1);
	HAL_PIN_Set(PAD_PA04, GPIO_A4, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(4, 0, 1);
	HAL_PIN_Set(PAD_PA05, GPIO_A5, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(5, 0, 1);
	HAL_PIN_Set(PAD_PA06, GPIO_A6, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(6, 0, 1);
	HAL_PIN_Set(PAD_PA07, GPIO_A7, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(7, 0, 1);
	HAL_PIN_Set(PAD_PA08, GPIO_A8, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(8, 0, 1);
	//lcd detect
	HAL_PIN_Set_Analog(PAD_PA09,1);
	//PA24, TODO PWM_60HZ
	
	//唤醒脚需要注意，没有外部上下拉
	HAL_PIN_Set(PAD_PA39, GPIO_A39, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(39, 0, 1);
	HAL_PIN_Set(PAD_PA40, GPIO_A40, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(40, 0, 1);
	HAL_PIN_Set(PAD_PA41, GPIO_A41, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(41, 0, 1);
	HAL_PIN_Set(PAD_PA42, GPIO_A42, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(42, 0, 1);
	HAL_PIN_Set(PAD_PA43, GPIO_A43, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(43, 0, 1);

	//TP
	HAL_PIN_Set(PAD_PA10, GPIO_A10, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(10, 0, 1);
	HAL_PIN_Set(PAD_PA11, GPIO_A11, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(11, 0, 1);
	HAL_PIN_Set(PAD_PA27, GPIO_A27, PIN_PULLDOWN, 1);  //int
	BSP_GPIO_Set(27, 0, 1);

	//Nor Flash 12,13,14,15,16,17

	//Uart 18,19

	//HR 20,21
	HAL_PIN_Set(PAD_PA20, GPIO_A20, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(20, 0, 1);
	HAL_PIN_Set(PAD_PA21, GPIO_A21, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(21, 0, 1);
	HAL_PIN_Set(PAD_PA25, GPIO_A25, PIN_PULLDOWN, 1); //int
	BSP_GPIO_Set(25, 0, 1);

	//MIC
	HAL_PIN_Set(PAD_PA22, GPIO_A22, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(22, 0, 1);
	HAL_PIN_Set(PAD_PA23, GPIO_A23, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(23, 0, 1);

	//CHG INT PA26 Keep status

	//IOE i2c PA28 29 keep status

	//COMM_I2C PA30,31 keep status

	//IMU_I2C
	HAL_PIN_Set(PAD_PA32, GPIO_A32, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(32, 0, 1);
	HAL_PIN_Set(PAD_PA33, GPIO_A33, PIN_PULLDOWN, 1);
	BSP_GPIO_Set(33, 0, 1);
	HAL_PIN_Set(PAD_PA38, GPIO_A38, PIN_PULLDOWN, 1);//int
	BSP_GPIO_Set(38, 0, 1);
	
	//Key PA34,35,36,37 keep status

	//

	//
    BSP_PowerDownCustom(coreid, is_deep_sleep);
}

void BSP_SDIO_Power_Up(void)
{
#ifdef RT_USING_SDIO
    // TODO: Add SDIO power up

#endif

}
void BSP_SDIO_Power_Down(void)
{
#ifdef RT_USING_SDIO
    // TODO: Add SDIO power down
#endif
}


