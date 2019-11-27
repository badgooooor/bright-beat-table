/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "ws2812b.h"
#include "time.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
//#include "arm_math.h"
//#include "arm_const_structs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
WSLED wsled;

int mode=0;
uint32_t speed=20;

bool flag_stop = false;
bool flag_music = false;
uint8_t FREQ=0;
uint32_t adc_Val[10];
uint8_t palette_A[6][3] = {
	{0xac,0x46,0x84},
	{0x95,0x47,0x84},
	{0x6d,0x48,0x86},
	{0x50,0x48,0x87},
	{0x3a,0x48,0x87},
	{0x20,0x49,0x88},
};

uint8_t musicSpectrum[6] = {1,2,3,4,5,6};
uint8_t musicColor[3] = {0xff,0x00,0x92};
uint8_t freqType = 5;
uint8_t randomFreq;

uint8_t musicLine[6][6] = {
	{1,3,4,6,5,2},
	{1,2,3,5,6,4},
	{1,1,2,3,5,6},
	{4,6,5,4,2,1},
	{6,4,3,3,2,1},
	{2,4,6,5,3,2},
};

extern DMA_HandleTypeDef hdma_adc1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void initWSLED(void);
uint32_t Wheel(uint8_t WheelPos);

void rainbowGradient();
void strobeGradient();
void wheelAll(void);
void randomColor(void);
void loopPalette(uint8_t color[6][3]);
void loopPalette2(uint8_t color[6][3]);

uint8_t randomNumber(uint8_t lower, uint8_t upper);
void displayHEX(uint32_t adc_input);
void displayVOL(uint32_t input_vol);
uint32_t set_speed(uint32_t input_rotary);

uint32_t spectrum_ANALYZER();
uint32_t set_Freq(double get_ADC[]);
 
void musicPalette(uint8_t spectrum[6], uint8_t color[3]);
void musicWave(uint8_t waveLine[6][6], uint8_t type, uint8_t color[3]);
void musicRow(uint8_t color[3]);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	/* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();
  /* USER CODE END 1 */
  

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	MX_ADC1_Init();
	MX_USART3_UART_Init();
	
//	uint32_t adc_val = 0;

	wsled.led_init();
	
	HAL_TIM_Base_Start(&htim2);
	HAL_ADC_Start_IT(&hadc1);
	
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_ADC_Start(&hadc2);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET); // set default NONC
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14,GPIO_PIN_SET);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
			
		if (flag_stop) {
			wsled.led_init();
			HAL_Delay(200);
			flag_stop = false;
		}
		
		if (flag_music) {
			musicRow(musicColor);
		} else {
			if(mode==0) {
				GPIOC -> BSRR = 0x40810000;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_SET);
				
				strobeGradient();
			} else if(mode==1) {
				GPIOC -> BSRR = 0x40810000;
			
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_SET);

				rainbowGradient(); 
			} else if(mode==2) {
				GPIOC -> BSRR = 0x40810000;
			
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_SET);
			
				wheelAll();
			} else if(mode==3) {
				//
			
				loopPalette2(palette_A);
			} else if(mode==4) {
				//
			
				randomColor();
			}
		}
	
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void initWSLED(void) {
	wsled.led_init();
	wsled.led_set_color_all(0x0F, 0x0F, 0x0F);  
	wsled.led_display();
}

uint32_t Wheel(uint8_t WheelPos) {
  if(WheelPos < 85) {
    //return wsled.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
		return (((uint32_t)(WheelPos * 3) << 16) | ((255 - (uint32_t)WheelPos * 3) <<  8) | 0) << 8;
  } else {
    if(WheelPos < 170) {
     WheelPos -= 85;
     //return wsled.Color(255 - WheelPos * 3, 0, WheelPos * 3);
		 return (((255 - (uint32_t)WheelPos * 3) << 16) | (0 <<  8) | ((uint32_t)WheelPos * 3)) << 8;
    } else {
     WheelPos -= 170;
     //return wsled.Color(0, WheelPos * 3, 255 - WheelPos * 3);
		 return (((uint32_t)0 << 16) | (((uint32_t)WheelPos * 3) <<  8) | (255 - (uint32_t)WheelPos * 3)) << 8;
    }
  }
}

void rainbowGradient(void) {
	uint16_t i, j;
	
	for(j=0; j<256*5; j++) { 
		for(i=0; i< LED_CFG_LEDS_CNT; i++) {
			wsled.led_set_color_rgb(i, Wheel(((i * 255 / LED_CFG_LEDS_CNT) + j) & 255));
		}
		wsled.led_display();
		HAL_Delay(10);
		HAL_Delay(speed/3);
		if (flag_stop) break;
	}
}

void strobeGradient(void) {
	uint16_t i, j, q;
	
	for (j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
		for (q=0; q < 3; q++) {
			for (i=0;i < LED_CFG_LEDS_CNT;i=i+3) {
				wsled.led_set_color_rgb(i+q, Wheel((i+j) % 255));    //turn every third pixel on
			}
			wsled.led_display();
			
			HAL_Delay(50);
			HAL_Delay(speed);

			for (i=0;i < LED_CFG_LEDS_CNT;i=i+3) {
				wsled.led_set_color_rgb(i+q, 0);        //turn every third pixel off
			}
		}
		if (flag_stop) break;
	}
}

void wheelAll(void) {
	uint16_t i;
	
	for (i=0; i < 256; i++) {
		wsled.led_set_color_all_rgb(Wheel(i % 255));
		wsled.led_display();
		HAL_Delay(200);
		HAL_Delay(speed/3);
		if (flag_stop) break;
	}
}

uint8_t randomNumber(uint8_t lower, uint8_t upper) {
	return (rand() % (upper - lower + 1)) + lower;
}

void randomColor(void) {
	uint16_t i;
	uint8_t color_r, color_g, color_b;
	
	for (i=0;i < LED_CFG_LEDS_CNT; i++) {
		color_r = randomNumber(0, 255);
		color_g = randomNumber(0, 255);
		color_b = randomNumber(0, 255);
		
		wsled.led_set_color(i, color_r, color_g, color_b);
		if (flag_stop) break;
	}
	wsled.led_display();
	HAL_Delay(200 + (speed));
	
}

void loopPalette(uint8_t color[6][3]) {
	uint16_t i, j;
	uint8_t palettePos;
	
	for (j=0;j < 6; j++) {
		for (i=0;i < LED_CFG_LEDS_CNT; i++) {
			if ((i / 6) % 2 == 1) {
				palettePos = (i+j)%6;
			} else {
				palettePos = 5-(i+j)%6;
			}
			wsled.led_set_color(i, color[palettePos][0], color[palettePos][1], color[palettePos][2]);
		}
		wsled.led_display();
		HAL_Delay(200 + (speed));
		if (flag_stop) break;
	}
}

void loopPalette2(uint8_t color[6][3]) {
	uint16_t i, j;
	uint8_t palettePos;
	
	for (j=0;j < 6;j++) {
		for (i=0;i < LED_CFG_LEDS_CNT;i++) {
			palettePos = ((i / 6)+j) % 6;
			wsled.led_set_color(i, color[palettePos][0], color[palettePos][1], color[palettePos][2]);
		}
		wsled.led_display();
		HAL_Delay(150 + (speed));
		if (flag_stop) break;
	}
}

void musicPalette(uint8_t spectrum[6], uint8_t color[3]) {
	uint8_t tempSpectrum[6] = {
		spectrum[0], spectrum[1], spectrum[2], spectrum[3], spectrum[4], spectrum[5]
	};
	
	uint16_t i;
  uint8_t row;
	bool isOddRow;
	
  for(i=0;i<LED_CFG_LEDS_CNT;i++) {
    row = i / 6;
		isOddRow = (row % 2 == 0);
		
		if (isOddRow) {
			if (tempSpectrum[row] > 0) {
				wsled.led_set_color(i, color[0], color[1], color[2]);
				tempSpectrum[row]--;
			} else {
				wsled.led_set_color(i, 0xFF, 0xFF, 0xFF);
			}
		} else {
			if (tempSpectrum[row] < 6) {
				wsled.led_set_color(i, 0xFF, 0xFF, 0xFF);
				tempSpectrum[row]++;
			} else {
				wsled.led_set_color(i, color[0], color[1], color[2]);
			}
		}
		if (flag_stop) break;
    wsled.led_display();
  }
	HAL_Delay(150);
}

void musicRow(uint8_t color[3]) {
	uint16_t i,j;
	
	randomFreq = FREQ;
	
	for(j=0;j<6;j++) {
		for (i=0;i < LED_CFG_LEDS_CNT;i++) {
			if (i < 6 * (randomFreq) && randomFreq != 0) {
				wsled.led_set_color(i, color[0], color[1], color[2]);
			} else {
				wsled.led_set_color(i, 0x0f, 0x0f, 0x0f);
			}
		}
		wsled.led_display();
		HAL_Delay(50);
		HAL_Delay(speed);
		if (flag_stop) break;
	}
}

void displayVOL(uint32_t input_vol)
{
	char vin[] = "Vin = ";
	double volin = input_vol/pow(2,12.0f)*3.3 ;
	char newline[] = "\n\r";
	char Volin[sizeof(unsigned int)*8+1] ;
	sprintf(Volin,"%.2f",volin);
	
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) vin,strlen(vin),1000);	
		
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) Volin,strlen(Volin),1000);
	
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	
}

void displayHEX(uint32_t adc_input)
{
	char hex[sizeof(unsigned int)*8+1];
	char ox[] = "0x";
	char o = '0';
	
	sprintf(hex, "%x ", adc_input);
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) ox,strlen(ox),1000);
	for(int i =1;i<=8-strlen(hex);i++)
	{	
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) &o,1,1000); 
	}
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
	HAL_UART_Transmit(&huart3,(uint8_t*) hex,strlen(hex),1000);	
	
}

uint32_t set_Freq(double get_ADC[]){
	char get_ADC_write[sizeof(unsigned int)*8+1] ;
	char newline[]= "\n\r" ;
	uint32_t format_Freq;
	for(int i = 0 ; i<6;i++){

		get_ADC[i]=get_ADC[i]/pow(2,12.0f)*3.3 ;
		
	}
	for(int i = 0 ; i<6;i++){
	sprintf(get_ADC_write,"%.2f",get_ADC[i]);
	while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *) get_ADC_write,strlen(get_ADC_write),1000);
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
		
	}
	if(get_ADC[0]<0.5&&get_ADC[1]<0.5&&get_ADC[2]<0.5&&get_ADC[3]<0.5&&get_ADC[4]<0.5&&get_ADC[5]>=2.0){
		format_Freq=0;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 0",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if((get_ADC[0]>=1.65&&get_ADC[1]>=1.3&&get_ADC[2]>=2.0&&get_ADC[3]>=2.0)||(get_ADC[0]>=1.4&&get_ADC[1]<=0.8&&get_ADC[2]>=1.4&&get_ADC[3]>=1.4)||(get_ADC[0]>=0.90&&get_ADC[1]<=0.01&&get_ADC[2]>=0.01&&get_ADC[3]>=0.01)){
		format_Freq=1;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 1",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(get_ADC[0]>=1.1&&get_ADC[1]>=2&&get_ADC[2]>=2&&get_ADC[3]>=2){
		format_Freq=2;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 2",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(get_ADC[0]>=0.6&&get_ADC[1]>=2&&get_ADC[2]>=2&&get_ADC[3]>=2){
		format_Freq=3;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 3",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(get_ADC[0]>=0.2&&get_ADC[1]>=0.7&&get_ADC[2]>=2&&get_ADC[3]>=2){
		format_Freq=4;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 4",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(get_ADC[0]>=0.1&&get_ADC[1]>=1.4&&get_ADC[2]>=2&&get_ADC[3]>=2){
		format_Freq=5;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 5",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(get_ADC[4]>=2&&get_ADC[5]>=2){
		format_Freq=6;
		while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Freq = 6",8,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	} 
	return format_Freq;
}

uint32_t spectrum_ANALYZER(){
	int freq=0;
	double adc_NONC[7];	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		
	for(int i = 0; i<7;i++){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
		//HAL_Delay(1);
		uint64_t now = HAL_GetTick();
		while(now + 1> HAL_GetTick());
		adc_NONC[i] = HAL_ADC_GetValue(&hadc2);
		displayHEX(adc_NONC[i]);
		displayVOL(adc_NONC[i]);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
	}
	freq = set_Freq(adc_NONC);
	return freq;
}

uint32_t set_speed(uint32_t input_rotary){
	char newline[]= "\n\r" ;
	uint32_t LED_speed;
	if(input_rotary/pow(2,12.0f)*3.3 <= 1.5){
				LED_speed = 500;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Speed += 500",12,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else if(input_rotary/pow(2,12.0f)*3.3 <= 3.1){
				LED_speed = 250;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Speed += 250",12,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	else{
				LED_speed = 0;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Speed += 0",12,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){};
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
	}
	
	return LED_speed;
	}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {  
		HAL_ADC_Start(&hadc1);
		volatile uint32_t adc_val = 0 ;
		char newline[]= "\n\r" ; 
		
		if(GPIO_Pin==GPIO_PIN_0) {
			HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
			flag_stop = true;
			if(mode==0) {
				mode++;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Mode 1",6,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
				
				musicColor[0] = 0xff;
				musicColor[1] = 0x00;
				musicColor[2] = 0x92;
			} else if(mode==1){
				mode++;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Mode 2",6,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
				
				musicColor[0] = 0xf8;
				musicColor[1] = 0x1e;
				musicColor[2] = 0x07;
			} else if(mode==2){
				mode++;
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t *)"Mode 0",6,1000);
				while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
				HAL_UART_Transmit(&huart3,(uint8_t*) newline,strlen(newline),1000);
					
				musicColor[0] = 0x00;
				musicColor[1] = 0x51;
				musicColor[2] = 0xff;
			} else if(mode==3) {
				mode++;
				
				musicColor[0] = 0xff;
				musicColor[1] = 0xff;
				musicColor[2] = 0xff;
			} else if(mode==4) {
				mode=0;
				
				musicColor[0] = 0x59;
				musicColor[1] = 0xff;
				musicColor[2] = 0xda;
			}
			HAL_Delay(200);
		}
			
		if(GPIO_Pin==GPIO_PIN_1) {
			flag_stop = true;
			mode = 0;
			if (flag_music == true) flag_music = false;
			else if (flag_music == false) flag_music = true;
			
			while(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_TC)==RESET){}
			HAL_UART_Transmit(&huart3,(uint8_t *)"Music0",6,1000);
		
		}
	}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	uint32_t adcVal;
	if(hadc ->Instance==ADC1){
	
		adcVal = HAL_ADC_GetValue(hadc);
		
		speed = set_speed(adcVal);
}
	
}
	

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
