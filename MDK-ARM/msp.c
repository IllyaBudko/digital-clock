



#include  "main.h"

void HAL_MspInit(void)
{
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  
  SCB->SHCSR |= 0x7 << 16;      //Usage fault, Memory fault and Bus fault system exceptions
  
  HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
  HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
  HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  i2c_init;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();

  i2c_init.Mode  = GPIO_MODE_AF_OD;
  i2c_init.Alternate = GPIO_AF4_I2C1;
  i2c_init.Pull  = GPIO_NOPULL;
  i2c_init.Speed = GPIO_SPEED_FREQ_HIGH;
  
  i2c_init.Pin   = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOB,&i2c_init);
  
  i2c_init.Pin   = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB,&i2c_init);
  
  HAL_NVIC_SetPriority(I2C1_EV_IRQn,0,0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  /*	RTC doesnt need to have APB domain clock enabled,
	because it is part of the backup domain,
	and still needs to run if the APB clock is off
  */
  
  RCC_OscInitTypeDef        rtc_osc_init;
  RCC_PeriphCLKInitTypeDef  rtc_clk_config;
  
  rtc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  rtc_osc_init.LSEState       = RCC_LSE_ON;
  rtc_osc_init.PLL.PLLState   = RCC_PLL_NONE;
  
  if(HAL_RCC_OscConfig(&rtc_osc_init) != HAL_OK)
  {
    while(1);
  }
  
  rtc_clk_config.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  rtc_clk_config.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
  
  if(HAL_RCCEx_PeriphCLKConfig(&rtc_clk_config) != HAL_OK)
  {
    while(1);
  }
  
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn,1,0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

  __HAL_RCC_RTC_ENABLE();
}











