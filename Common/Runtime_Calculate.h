/**
  ******************************************************************************
  * File Name          : Runtime_Calculate.h
  * Description        : This file mainly includes the function running measurement function and its macro definition: 
  *                      the function running time is obtained mainly by measuring the GPIO turning time
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RUNTIME_CALCULATE_H
#define __RUNTIME_CALCULATE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "numtype.h"

/* Common macro definitions---------------------------------------------------*/

/* GPIO ports and pins to use */
#define RUNTIME_TEST_GPIO_Pin           GPIO_PIN_0
#define RUNTIME_TEST_GPIO_GPIO_Port     GPIOB

/* Extern Variable------------------------------------------------------------*/

/* Data structure declaration-------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* Init Used GPIO ports and pins */
t_FuncRet Runtime_Calculate_Init_Hardware(void); 

/* Measure function start (measured by measuring the GPIO port turn time) - Pull up the GPIO port level */
t_FuncRet Runtime_Calculate_Start_Hardware(void); 

/* Measure function finish (measured by measuring the GPIO port turn time) - Pull down the GPIO port level */
t_FuncRet Runtime_Calculate_Finish_Hardware(void); 


#ifdef __cplusplus
}
#endif
#endif /* __RUNTIME_CALCULATE_H */
