#include  "main.h"

#define	RCC_CLOCKTYPE_ALL	(RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2)

I2C_HandleTypeDef   i2c_handle;
RTC_HandleTypeDef   rtc_handle;

void SystemClockConfig(void);

void I2C_Init(void);
void RTC_Init(void);

void TimeDateConfig(void);
void AlarmConfig(void);
  
//Display APIs
void Display_Initialization(void);
void Time_Decode(void);
void Display_Time_Write(void);

void I2C_ReInit(void);


uint8_t Display_Addr = (0x70 >> 1);   // Display address
uint8_t transmit_buffer[] = {0x04,0,0x02,0,0x06,0,0x08,0};
uint8_t init_tx_buffer[] = {0x21,0x81,0xEF};
uint8_t *pdata = init_tx_buffer;

//volatile uint8_t digit_bcd[] = {0,0,0,0};

const uint8_t numtable[] = {
  0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F  /* 9 */
};

int main()
{
  HAL_Init();
//  RTC_Init();
  I2C_Init();
  
  SystemClockConfig();
  
  HAL_Delay(50);
  
  if(HAL_I2C_Master_Transmit_IT(&i2c_handle,0xE0,pdata,1) != HAL_OK)
  {
    while(1);
  }
  pdata++;
  
  HAL_Delay(10);
  
  if(HAL_I2C_IsDeviceReady(&i2c_handle,0xE0,3,100) != HAL_OK)
  {
    while(1);
  }
  
  HAL_Delay(10);
  
  if(HAL_I2C_Master_Transmit_IT(&i2c_handle,0xE0,pdata++,1) != HAL_OK)
  {
    while(1);
  }
  
  HAL_Delay(10);
  
  while(1);
}

void SystemClockConfig(void){
  
	RCC_OscInitTypeDef	osc_init;
	RCC_ClkInitTypeDef	clk_init;

	// variable for flash latency config
	uint32_t fLatency = 0;

	//Oscillator setup to use in SYSCLK
	osc_init.OscillatorType		= RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState			= RCC_HSE_BYPASS;
	osc_init.PLL.PLLState		= RCC_PLL_ON;
	osc_init.PLL.PLLSource		= RCC_PLLSOURCE_HSE;

	//clock source  and setup
	clk_init.ClockType			= RCC_CLOCKTYPE_ALL;
	clk_init.SYSCLKSource		= RCC_SYSCLKSOURCE_PLLCLK;

  osc_init.PLL.PLLM		= 4;
  osc_init.PLL.PLLN		= 60;
  osc_init.PLL.PLLP		= RCC_PLLP_DIV2;

  clk_init.AHBCLKDivider	= RCC_SYSCLK_DIV1;
  clk_init.APB1CLKDivider	= RCC_HCLK_DIV4;
  clk_init.APB2CLKDivider	= RCC_HCLK_DIV1;

	fLatency = FLASH_ACR_LATENCY_5WS;


  if(HAL_RCC_OscConfig(&osc_init) != HAL_OK){
		while(1);
	}

	//clk config
	if(HAL_RCC_ClockConfig(&clk_init,fLatency) != HAL_OK){
		while(1);
	}

	//systick config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void I2C_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();

  
  i2c_handle.Instance = I2C1;
  i2c_handle.Init.ClockSpeed = 100000;   
  i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;  //50% Duty Cycle for Standard Mode
  i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; //7bit address plus ack
  i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; // Single address for device
  i2c_handle.Init.OwnAddress1 = 0x49; //Address set to ASCII letter "I"
  i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
  
  if(HAL_I2C_Init(&i2c_handle) != HAL_OK)
  {
    while(1);
  }
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_I2C_ENABLE(&i2c_handle);  
  __HAL_I2C_ENABLE_IT(&i2c_handle,I2C_IT_EVT); 
  
}

void RTC_Init(void)
{
  rtc_handle.Instance = RTC;
  rtc_handle.Init.HourFormat = RTC_HOURFORMAT_12;
  rtc_handle.Init.AsynchPrediv = 0x7F;
  rtc_handle.Init.SynchPrediv = 0xFF;
  rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
  rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
  rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&rtc_handle) != HAL_OK)
  {
    while(1);
  }
}

void TimeDateConfig(void)
{
  RTC_TimeTypeDef   timeinit;
  
  timeinit.TimeFormat = RTC_HOURFORMAT12_PM;
  timeinit.Hours = 9;
  timeinit.Minutes = 0;
  timeinit.Seconds = 0;
  
  if(HAL_RTC_SetTime(&rtc_handle,&timeinit,RTC_FORMAT_BCD) != HAL_OK)
  {
    while(1);
  }
}

void AlarmConfig(void)
{
  RTC_AlarmTypeDef    alarminit;
  
  alarminit.Alarm = RTC_ALARM_A;
  alarminit.AlarmMask = RTC_ALARMMASK_ALL;
  alarminit.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  alarminit.AlarmTime.SubSeconds = 0;
  
  if(HAL_RTC_SetAlarm_IT(&rtc_handle,&alarminit,RTC_FORMAT_BCD) != HAL_OK)
  {
    while(1);
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  Display_Time_Write();
}


//Display APIs
void Display_Initialization(void)
{
  if(HAL_I2C_Master_Transmit_IT(&i2c_handle,4,init_tx_buffer,sizeof(init_tx_buffer)) != HAL_OK)
  {
    while(1);
  }
}

void Time_Decode(void)
{
  RTC_TimeTypeDef   time_read;
  RTC_DateTypeDef   date_read;
  
  if(HAL_RTC_GetTime(&rtc_handle,&time_read,RTC_FORMAT_BCD) != HAL_OK)
  {
    while(1);
  }
  
  if(HAL_RTC_GetDate(&rtc_handle,&date_read,RTC_FORMAT_BCD) != HAL_OK)
  {
    while(1);
  }
  //digit_bcd[0] = ((time_read.Hours & 0xF0) >> 4);   //Hours high digit
  //digit_bcd[1] = time_read.Hours & 0xF;             //Hours low digit
  //digit_bcd[2] = ((time_read.Minutes & 0xF0) >> 4); //Minutes high digit
  //digit_bcd[3] = time_read.Minutes & 0xF;           //Minutes low digit
  
  transmit_buffer[1] = numtable[((time_read.Hours & 0xF0) >> 4)];
  transmit_buffer[3] = numtable[time_read.Hours & 0xF];
  transmit_buffer[5] = numtable[((time_read.Minutes & 0xF0) >> 4)];
  transmit_buffer[7] = numtable[time_read.Minutes & 0xF];

}

void Display_Time_Write(void)
{
  Time_Decode();
  HAL_I2C_Master_Transmit_IT(&i2c_handle,4,transmit_buffer,sizeof(transmit_buffer));
}

void I2C_ReInit(void)
{
    if(HAL_I2C_DeInit(&i2c_handle) != HAL_OK)
  {
    while(1);
  }
  
  if(HAL_I2C_Init(&i2c_handle) != HAL_OK)
  {
    while(1);
  }
  __HAL_I2C_ENABLE(&i2c_handle);
  __HAL_I2C_ENABLE_IT(&i2c_handle, I2C_IT_EVT);
}









