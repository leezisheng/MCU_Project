/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* 
	This file contains basic operations on the ADC, including 
	initialization, reading data, returning data, and so on
*/
#include "ADC_Operation.h"

/*
	Header files for functions related to serial port 2 redirection, 
	serial port 6 send string, analog redirection, enable serial port receive interrupt, 
	and serial port 1 operations
*/
#include "USART_Printf.h"

/*
	Contains a header file for a function that uses serial port 6 to 
	send instructions to control the bus steering gear
*/
#include "ServoMotor_Control.h"
/*
	The functions include reading and writing gyroscope data through serial port 1, 
	enabling serial port 1 to receive interrupt, and parsing gyroscope data
*/
#include "USART_Gyroscope.h"

/*
	Mean filtering of channel 1, 3, 4, 5 and reference channel of ADC1
*/
#include "ADC_Function.h"

/*
	The file includes commonly used digital filtering, 
    frequency domain transformation and motion control algorithms
*/
#include "DigtalSignal_Process.h"

/*
	This file includes the declaration of the function for 
    mean filtering the motion data collected by the gyroscope
*/
#include "GyroscopeData_Process.h"

/*
    This file defines the structure and functions for sending data to the upmachine
*/
#include "SendData_Function.h"

/*
    This file includes the ARM digital signal processing related firmware library
*/
#include "arm_math.h"
#include "arm_const_structs.h"
/*
    The virtual serial port sends related header files
*/
#include "usbd_cdc_if.h"

/*
    This file includes definition of serial port panel control function 
    and related data structure definition
*/
#include "HMI_Control.h"

/*
    This file includes serial port panel curve update function, serial port panel data display function, 
    and related structure declaration
*/
#include "HMI_Function.h"

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

/* A variable that stores the return value of a function for easy debugging */
static t_FuncRet ret = Operation_Success;
/* A variable used to indicate whether hardware initialization was successful */
static volatile bool HardwareComplete_Flag = (bool)FALSE;

/* 
Note: global variables are used here only to see the value of the variable. 
They are not used when the program is running 
*/
#ifdef CODE_TEST
/* 
	The following functions are only used for software development and debugging, 
	and are not required for official launch
*/

/* Adc-related static global variables */
static uint16_t Sensor1_V_Data          = 3300;
static uint16_t Sensor2_V_Data          = 2900;
static uint16_t Sensor3_V_Data          = 1500;
static uint16_t Sensor4_V_Data          = 1000;
static uint16_t Vref                    = 800;
volatile static uint32_t sample_count   = 400;

static float Sensor1_V_Data_F           = 0;
static float Sensor2_V_Data_F           = 0;
static float Sensor3_V_Data_F           = 0;
static float Sensor4_V_Data_F           = 0;
static float Vref_F                     = 0;

/* Servo Motor Postion(Angle) */
volatile static int32_t Angle           = 0;

/* Gyroscope related data */
/* 
	Gyroscope calibration count variable: when count is 10, acceleration and Z-axis Angle calibration is started 
*/
volatile static float angle_x           = 90.71;
volatile static float angle_y           = 43.75;
volatile static float angle_z           = 0;

volatile static float acc_x 	        = 12.75;
volatile static float acc_y 	        = 33.33;
volatile static float acc_z 	        = 0;

volatile static float gyro_x            = 0;
volatile static float gyro_y            = 0;
volatile static float gyro_z            = 0;

/* Manipulator control related variables */

volatile static uint8_t temp_id		    = 0;
volatile static int32_t angle 	        = 0;
volatile static int16_t position        = 0;
volatile static int32_t wait_time 	    = 0;

#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* Peripheral initialization function */ 
t_FuncRet Hardware_Init(void);

/* Serial port 6 The receiver is cleared periodically */
extern void USART6_RecvDataClear(void);

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

  /* USER CODE END 1 */

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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  
  /* Peripheral initialization function */
  
  ret = Hardware_Init();
  
  #ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
  #endif
  
  if(ret == Operation_Fail)
  {
  	printf("Failed to initialize Hardware Procedure\r\n");
    HardwareComplete_Flag = (bool)FALSE;
	Error_Handler();
  }
  else
  {
    printf("Success to initialize Hardware Procedure\r\n");
    HardwareComplete_Flag = (bool)TRUE;
  }
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 288;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/** 
* @description: Peripheral initialization function
* @param  {void} 
* @return {t_FuncRet} : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Hardware_Init(void)
{
	HAL_Delay(500);
	printf("====The system starts to initialize hardware====\r\n");
	
	/* Initialize ADC related peripherals: ADC GPIO port and DMA channel*/
    /* The interruption of timer 2 was enabled */
    /* 
		Timer 2 is interrupted periodically(Fre = 2000Hz), 
		and Implementation of ADC timing multi - channel sampling conversion
	*/
	HAL_Delay(1000);
	ret= ADC_Operation_Init();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize ADC. Procedure\r\n");
		Error_Handler();
	}
	printf("success to initialize ADC Procedure\r\n");
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
	
	/* Enable serial port 6 Interrupt receiving (control the action of the manipulator through serial port 6) */
	HAL_Delay(1000);
	ret = USART6_Start_IT();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize USART6 IT\r\n");
		Error_Handler();
	}
	printf("success to initialize USART6 IT\r\n");
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
    #endif
	
	/* Enable serial port 1 Interrupt receiving (Read and write gyroscope data through serial port 1) */
	HAL_Delay(1000);
	ret = USART1_Start_IT();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize USART1 IT\r\n");
		Error_Handler();
	}
	printf("success to initialize USART1 IT\r\n");
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
    
    /* Enable serial port 2 Interrupt receiving */
	HAL_Delay(1000);
	ret = USART2_Start_IT();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize USART2 IT\r\n");
		Error_Handler();
	}
	printf("success to initialize USART2 IT\r\n");
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
	
	/* Steering gear control test: Control rotation of No. 0 to 6 steering gear */
    /* The interruption of timer 3 was enabled */
    /* 
		Timer 3 is interrupted periodically(Fre = 100Hz), 
		and the receive clearance function of serial port 6 is invoked 
	*/
	HAL_Delay(1000);
	ret = ServoMotor_Control_Init();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize ServoMotor\r\n");
		Error_Handler();
	}
	printf("success to initialize ServoMotor\r\n");
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
	
	/* Gyroscope initialization: acceleration calibration and Z-axis Angle calibration */
    /* The interruption of timer 4 was enabled */
    /*
        Timer 4 Periodic interrupt (Fre = 20Hz)
        and is used for timed data return of serial gyro JY-60
    */
	HAL_Delay(1000);
	ret = Gyroscope_Calibration();
	if(ret == Operation_Fail)
	{
		printf("Failed to initialize Gyroscope\r\n");
		Error_Handler();
	}
	printf("success to initialize Gyroscope\r\n");
	
	HAL_Delay(1000);
	/* After the device is powered on, the device delays */
	printf("Device power-on delay 500 ms, please wait\r\n");
	HAL_Delay(500);

	return ret;
}

/* A function to indicate whether hardware initialization is complete */
t_FuncRet IsCompleteHardwareInit(void)
{
    t_FuncRet ret = Operation_Wait;
    
    if(HardwareComplete_Flag == (bool)FALSE)
    {
        ret = Operation_Fail;
    }
    else if(HardwareComplete_Flag == (bool)TRUE)
    {
        ret = Operation_Success;
    }
    
    return ret;
}

#ifdef CODE_TEST
/* 
	The following functions are only used for software development and debugging, 
	and are not required for official launch
*/

/* ADC Original voltage output test */
void ADC_Original_Value_Test(void)
{
	/* Enable ADC multi-channel DMA acquisition */
    ret= ADC_Get_Data();
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
	
    /* Obtain the voltage of no. 1 EMG sensor */
	ret= ADC_Get_SensorData_1(&Sensor1_V_Data);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
	
	/* Obtain the voltage of no. 2 EMG sensor */
	ret= ADC_Get_SensorData_2(&Sensor2_V_Data);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
	
	/* Obtain the voltage of no. 3 EMG sensor */
	ret= ADC_Get_SensorData_3(&Sensor3_V_Data);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
	
	/* Obtain the voltage of no. 4 EMG sensor */
	ret= ADC_Get_SensorData_4(&Sensor4_V_Data);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
	
	/* Obtain the voltage of Vref */
	ret= ADC_Get_Vref(&Vref);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
}

/* ADC MeanFilter voltage output test */
void ADC_MeanFilter_Value_Test(void)
{
	ret = Get_ADC_MeanFilter_Value(&Sensor1_V_Data, &Sensor2_V_Data, &Sensor3_V_Data, &Sensor4_V_Data, &Vref);  
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
}

/* ADC KalmanFilter voltage output test */
void ADC_KalmanFilter_Value_Test(void)
{
	ret = Get_ADC_KalmanFilter_Value(&Sensor1_V_Data_F, &Sensor2_V_Data_F, &Sensor3_V_Data_F, &Sensor4_V_Data_F, &Vref_F);
	if(ret == Operation_Fail)
	{
		Error_Handler();
	}
}

/* Serial port gyroscope Original movement data output test */
void USART_Gyroscope_Original_Test(void)
{
	angle_x = Get_Xaxis_Angle();
	angle_y = Get_Yaxis_Angle();
	angle_z = Get_Zaxis_Angle();

	acc_x 	= Get_Xaxis_Acc();
	acc_y   = Get_Yaxis_Acc();
	acc_z   = Get_Zaxis_Acc();

	gyro_x  = Get_Xaxis_Angle_Acc();
	gyro_y  = Get_Yaxis_Angle_Acc();
	gyro_z  = Get_Zaxis_Angle_Acc();
}

/* Serial port gyroscope MeanFilter movement data output test */
void USART_Gyroscope_MeanFilter_Test(void)
{
	ret = Get_MotionData_MeanFilter_Value((float*)&angle_x , 
										  (float*)&angle_y ,
						                  (float*)&angle_z ,
						                  (float*)&gyro_x  ,
								          (float*)&gyro_y  ,
						                  (float*)&gyro_z );
}

#endif
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* when error , At this time, LD3, LD4, LD5, and LD6 blink on the development board */
  while (1)
  {
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
	HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
	HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);

	HAL_Delay(100);	  
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
	while(1);
	
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
