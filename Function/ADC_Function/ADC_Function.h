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

/* Data structure declaration-------------------------------------------------*/


/* Extern variables-----------------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* Get the median filter voltage value */
t_FuncRet Get_ADC_MeanFilter_Value(uint16_t* p_Sensor1_V_Data , 
						   uint16_t* p_Sensor2_V_Data ,
						   uint16_t* p_Sensor3_V_Data ,
						   uint16_t* p_Sensor4_V_Data ,
						   uint16_t* p_Vref_V_Data);


#ifdef __cplusplus
}
#endif
#endif /*__ADC_FUNCTION_H*/


