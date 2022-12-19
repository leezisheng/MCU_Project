/**
  ******************************************************************************
  * File Name          : Runtime_Calculate.c
  * Description        : This file mainly includes the function running measurement function and its macro definition: 
  *                      the function running time is obtained mainly by measuring the GPIO turning time
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Runtime_Calculate.h"

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/


/* Global variable------------------------------------------------------------*/


/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/** 
* @description: Init Used GPIO ports and pins
* @param  {void} 
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Runtime_Calculate_Init_Hardware(void)
{
    __NOP();
    return (t_FuncRet)Operation_Success;
}

/** 
* @description: Measure function start (measured by measuring the GPIO port turn time) - Pull up the GPIO port level
* @param  {void} 
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Runtime_Calculate_Start_Hardware(void)
{
    t_FuncRet ret= (t_FuncRet)Operation_Success;
    
    HAL_GPIO_WritePin(RUNTIME_TEST_GPIO_GPIO_Port,RUNTIME_TEST_GPIO_Pin,GPIO_PIN_SET);
    
    if(HAL_GPIO_ReadPin(RUNTIME_TEST_GPIO_GPIO_Port,RUNTIME_TEST_GPIO_Pin) != GPIO_PIN_SET)
    {
        ret= (t_FuncRet)Operation_Fail;
    }
    
    return ret;
}
    
/** 
* @description: Measure function finish (measured by measuring the GPIO port turn time) - Pull down the GPIO port level
* @param  {void} 
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Runtime_Calculate_Finish_Hardware(void)
{
    t_FuncRet ret= (t_FuncRet)Operation_Success;
    
    HAL_GPIO_WritePin(RUNTIME_TEST_GPIO_GPIO_Port,RUNTIME_TEST_GPIO_Pin,GPIO_PIN_RESET);
    
    if(HAL_GPIO_ReadPin(RUNTIME_TEST_GPIO_GPIO_Port,RUNTIME_TEST_GPIO_Pin) != GPIO_PIN_RESET)
    {
        ret= (t_FuncRet)Operation_Fail;
    }
    
    return ret;
}

