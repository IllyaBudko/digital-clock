#include "main.h"


extern  I2C_HandleTypeDef   i2c_handle;
extern  RTC_HandleTypeDef   rtc_handle;


void SysTick_Handler(void)
{

	//Initialization for SYSTICK to 1ms for HAL functionality
	//If SYSCLK configuration is done later in the program SYSTICK needs to be re-adjusted
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&i2c_handle);
}

void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&rtc_handle);
}





