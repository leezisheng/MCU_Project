/**
  ******************************************************************************
  * File Name          : ADC_Operation.c
  *
  * Description        : This file contains basic operations on the ADC, including 
  *						 initialization, reading data, returning data, and so on
  *
  *						 ADC sampling is triggered by software
  *						 ADC software startup trigger program is started in TIM2 (2000Hz)timer interrupt. 
  *						 Specific parameters of TIM2 are as follows:
  *						 APB1 100MHz = 100*1000000 PSC:1000-1 ARR:50-1 
  * parameter          :
  * 					 ADC1:Preenmption Priority 0
							 mode: IN1  -- PA1
							 mode: IN3  -- PA3
							 mode: IN5  -- PA5
							 mode: IN6  -- PA6
							 mode: Vref -- Vref
					     
						 ADC sampling rate £º
							 Sampling period = conversion time + Read time
							 Conversion time = Sampling time£¨15 clock cycles£© +12 clock cycles (STM32F4)
							 Conversion frequency = ADC clock frequency/£¨Conversion time£©= 25 Mhz /27 cycle = 925Khz
						 
							 ADCs_Common_Settings:
							 Mode Independent mode
							 
							 ADC_Settings:
							 Clock Prescaler PCLK2 divided by 4 = 25MHz
							 Resolution 12 bits (15 ADC Clock cycles)
							 Data Alignment Right alignment
							 Scan Conversion Mode Enabled
							 Continuous Conversion Mode Disabled
							 Discontinuous Conversion Mode Enabled *
							 Number Of Discontinuous Conversions 1
							 DMA Continuous Requests Enabled *
							 End Of Conversion Selection EOC flag at the end of single channel conversion
							 
							 ADC_Regular_ConversionMode:
							 Number Of Conversion 2 *
							 External Trigger Conversion Source Regular Conversion launched by software
							 External Trigger Conversion Edge None
							 
							 Rank 1
							 Channel Channel 1
							 Sampling Time 15 Cycles
							 
							 Rank 2 *
							 Channel Channel 3 
							 Sampling Time 15 Cycles
							 
							 Rank 3
							 Channel Channel 5
							 Sampling Time 15 Cycles
							 
							 Rank 4 *
							 Channel Channel 6 
							 Sampling Time 15 Cycles
							 
							 Rank 5 *
							 Channel Vref 
							 Sampling Time 15 Cycles
						 
						 
						 DMA2: Preenmption Priority 2
						 
							DMA request 	Stream 			Direction 				Priority
							ADC1 			DMA2_Stream0 	Peripheral To Memory 	High
							
							Mode: Circular *
							
						 Use fifo: Disable
						 Peripheral Increment: Disable
						 Memory Increment: Enable *
						 Peripheral Data Width: Half Word
						 Memory Data Width: Half Word
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "ADC_Operation.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"

/* External function declaration----------------------------------------------*/

/* Private macro definitions--------------------------------------------------*/

/* Global variable------------------------------------------------------------*/

/* External variable: handle of ADC1 */
extern ADC_HandleTypeDef hadc1;
/* External variable: handle of TIM2 */
extern TIM_HandleTypeDef htim2;

/* Variable containing ADC conversions results */
static __IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];
/* Variables for ADC conversions results computation to physical values */
static uint16_t   uhADCChannel_1_ToDAC_mVolt 	= 0;
static uint16_t   uhADCChannel_3_ToDAC_mVolt 	= 0;
static uint16_t   uhADCChannel_5_ToDAC_mVolt 	= 0;
static uint16_t   uhADCChannel_6_ToDAC_mVolt 	= 0;
static uint16_t   uhADCChannel_Vref_ToDAC_mVolt = 0;
/* Variable to report ADC sequencer status */
uint8_t ubSequenceCompleted = RESET;     /* Set when all ranks of the sequence have been converted */

/* Static function definition-------------------------------------------------*/

/* The function uses the reference voltage to calibrate the voltage value collected by the ADC channel */
static uint16_t ADC_Calibration(uint16_t V_Channel , uint16_t V_Ref);

/* Function definition--------------------------------------------------------*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
  /* Report to main program that ADC sequencer has reached its end */
  ubSequenceCompleted = SET;
}

/** 
* @description: Initialize ADC related peripherals: ADC GPIO port and DMA channel
* @param  {void} 
* @return {uint8_t } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet  ADC_Operation_Init(void)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	/* 
	Note that there should be a calibration procedure before turning on the ADC, 
	but the F4 chip does not support ADC calibration
	*/
	
	/* Start timer 2 first and then DMA transfer */
	
	/* 
		Timer 2 is interrupted periodically(2000Hz), 
		and Implementation of ADC timing multi - channel sampling conversion
	*/
	
	/* Clear the IT flag bit */
	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE ); 

	/* Enable timer 3 Interrupt */
	if(HAL_TIM_Base_Start_IT(&htim2)!=HAL_OK)
	{
		return ret= (t_FuncRet)Operation_Fail;
	}
	
	/* Start the conversion process and enable interrupt */
    if (HAL_ADC_Start_DMA(&hadc1,
						  (uint32_t *)aADCxConvertedValues,
                          ADCCONVERTEDVALUES_BUFFER_SIZE
                         ) != HAL_OK)
	{
		return ret= (t_FuncRet)Operation_Fail;
	}
	
	/* The flag bit reset of ADC1 collection is complete */
	ubSequenceCompleted = RESET;
	
	return ret;
}

/** 
* @description: Obtain the voltage values collected by the 2 channels
* @param  {void} 
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_Data(void)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	HAL_ADC_Start(&hadc1);
	
	/* Wait for conversion completion before conditional check hereafter */
    HAL_ADC_PollForConversion(&hadc1, 1);


    if (ubSequenceCompleted == RESET)
    {
      /* ADC acquisition not completed, LED off */	
      HAL_GPIO_WritePin(LD4_GPIO_Port,LD4_Pin,GPIO_PIN_RESET);
	  ret= (t_FuncRet)Operation_Wait;
    }
	else
	{
	
	  HAL_GPIO_WritePin(LD4_GPIO_Port,LD4_Pin,GPIO_PIN_SET);
		
      uhADCChannel_1_ToDAC_mVolt    = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aADCxConvertedValues[0]);
      uhADCChannel_3_ToDAC_mVolt    = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aADCxConvertedValues[1]);
	  uhADCChannel_5_ToDAC_mVolt    = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aADCxConvertedValues[2]);
      uhADCChannel_6_ToDAC_mVolt    = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aADCxConvertedValues[3]);
	  uhADCChannel_Vref_ToDAC_mVolt = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(aADCxConvertedValues[4]);
		
	  ubSequenceCompleted = RESET;
      ret= (t_FuncRet)Operation_Success;
	}
	
	return ret;
}

/** 
* @description: Obtain the voltage of no. 1 EMG sensor 
* @param  {float} Sensor_V_Data : the voltage of no. 1 EMG sensor 
* @return {t_FuncRet}           : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_SensorData_1(uint16_t* p_Sensor_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	*p_Sensor_V_Data =  uhADCChannel_1_ToDAC_mVolt;
	
	return ret;
}

/** 
* @description: Obtain the voltage of no. 2 EMG sensor 
* @param  {float} Sensor_V_Data : the voltage of no. 1 EMG sensor 
* @return {t_FuncRet}           : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_SensorData_2(uint16_t* p_Sensor_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	*p_Sensor_V_Data = uhADCChannel_3_ToDAC_mVolt;
	
	return ret;
}

/** 
* @description: Obtain the voltage of no. 3 EMG sensor 
* @param  {float} Sensor_V_Data : the voltage of no. 1 EMG sensor 
* @return {t_FuncRet}           : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_SensorData_3(uint16_t* p_Sensor_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	*p_Sensor_V_Data =  uhADCChannel_5_ToDAC_mVolt;
	
	return ret;
}

/** 
* @description: Obtain the voltage of no. 4 EMG sensor 
* @param  {float} Sensor_V_Data : the voltage of no. 1 EMG sensor 
* @return {t_FuncRet}           : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_SensorData_4(uint16_t* p_Sensor_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	*p_Sensor_V_Data = uhADCChannel_6_ToDAC_mVolt;
	
	return ret;
}

/** 
* @description: Obtain the voltage of Vref
* @param  {float} Sensor_V_Data : Vref 
* @return {t_FuncRet}           : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ADC_Get_Vref(uint16_t* p_Vref)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	*p_Vref = uhADCChannel_Vref_ToDAC_mVolt;
	
	return ret;
}

/** 
* @description: uses the reference voltage to calibrate the voltage value collected by the ADC channel
*				V_True = (VDD_APPLI*VREF_CAL*V_Channel)/£¨V_Ref*RANGE_12BITS£©
* @param  {uint16_t} V_Channel : You need to measure the channel using an ADC readout
* @param  {uint16_t} V_Ref     : Internal reference voltage channel read value
* @return {uint16_t} V_True
* @author: leeqingshui 
*/
static uint16_t ADC_Calibration(uint16_t V_Channel , uint16_t V_Ref)
{
	uint16_t V_True = 0;
	
	V_True = (VREF_CAL*V_Channel)/(V_Ref);
	
	return V_True;
}


