/**
  ******************************************************************************
  * File Name          : ADC_Operation.h
  * Description        : This file contains declarations of the basic ADC operation 
  *						 functions and related macro definitions and macro functions
  * 1. Problem 1: only one conversion in ADC-DMA scan mode
  * Note the problem of only one conversion in ADC scan mode, which is a common 
  * problem of STM32F4 series. For solutions, please refer to:
  *
  * https://www.cnblogs.com/SpaceVision/p/14967055.html
  *
  * 2. Probelm 2: ADC measurement is inaccurate and the jump value is large
  * Note that in this case, because the VDDA value is unknown, the ADC channel sample 
  * value must be calibrated using an internal reference voltage,please refer to:
  * 
  * If VDDA is connected to VDD, the fluctuation of power supply will greatly affect the accuracy of ADC
  *
  * https://blog.csdn.net/weixin_43128823/article/details/82803927
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_Opretion_H
#define __ADC_Opretion_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

/* 
	VREFINT_CAL:ADC reference voltage calibration value, solidified in the chip 
	It needs to be read by software
	
	It's in the STM32F411 data sheet on page 120
	
	Memory address:0x1FFF 7A2A - 0x1FFF 7A2B
*/

#define VREF_CAL                        *(__IO uint16_t *)(0x1FFF7A2A) 
/* Value of analog voltage supply Vdda (unit: mV) */
#define VDD_APPLI                      ((uint32_t) 3300)  
/* Maximum value of variable "UserButtonClickCount" */
#define RANGE_12BITS                   ((uint32_t) 4095)   
/* Size of array containing ADC converted values: set to ADC sequencer number of ranks converted, to have a rank in each address */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)    3)  

/**
  * @brief  Computation of voltage (unit: mV) from ADC measurement digital
  *         value on range 12 bits.
  *         Calculation validity conditioned to settings: 
  *          - ADC resolution 12 bits (need to scale value if using a different 
  *            resolution).
  *          - Power supply of analog voltage Vdda 3.3V (need to scale value 
  *            if using a different analog voltage supply value).
  * @param ADC_DATA: Digital value measured by ADC
  * @retval None
  */
#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA)                        \
  ( (ADC_DATA) * VDD_APPLI / RANGE_12BITS)

/* Extern Variable------------------------------------------------------------*/

/* Data structure declaration-------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* Initialize ADC related peripherals: ADC GPIO port and DMA channel */
t_FuncRet ADC_Operation_Init(void);
/* Obtain the voltage values collected by the 2 channels */
t_FuncRet ADC_Get_Data(void);
/* Obtain the voltage of no. 1 EMG sensor */
t_FuncRet ADC_Get_SensorData_1(float* p_Sensor_V_Data);
/* Obtain the voltage of no. 2 EMG sensor */
t_FuncRet ADC_Get_SensorData_2(float* p_Sensor_V_Data);
/* Obtain the voltage of Vref */
t_FuncRet ADC_Get_Vref(float* p_Vref);

#ifdef __cplusplus
}
#endif
#endif /* __ADC_Opretion_H */

