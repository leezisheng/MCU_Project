/**
  ******************************************************************************
  * File Name          : ADC_Function.h
  * Description        : This file contains the ADC digital filter function 
  *                      declaration and filter structure definition
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_FUNCTION_H
#define __ADC_FUNCTION_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

/* Mean filter times, the more times, the slower the sensor data transformation */
#define MEAN_FILTER_NUM	3

/* Data structure declaration-------------------------------------------------*/

/* Two point mean filter structure */
typedef struct 
{
	/* Cache three historical data, the size can be modified */
	uint16_t Data_Buf[MEAN_FILTER_NUM];
	
	/* The sum of the data */
	uint32_t sum;

	/* output result */
	uint16_t result; 
	
}Mean_Filter;


/* Extern variables-----------------------------------------------------------*/


/* Function declaration-------------------------------------------------------*/

/* Get the median filter voltage value */
t_FuncRet Get_TrueAdcValue(uint16_t* p_Sensor1_V_Data , 
						   uint16_t* p_Sensor2_V_Data ,
						   uint16_t* p_Sensor3_V_Data ,
						   uint16_t* p_Sensor4_V_Data ,
						   uint16_t* p_Vref_V_Data);


#ifdef __cplusplus
}
#endif
#endif /*__ADC_FUNCTION_H*/


