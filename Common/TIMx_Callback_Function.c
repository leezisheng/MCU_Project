/**
  ******************************************************************************
  * File Name          : TIMx_Callback_Function.c
  * Description        : This file mainly includes the implementation of the timer 
  *						 interrupt callback function and some related local variables
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "USART_Printf.h"
#include "tim.h"

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/


/* Global variable------------------------------------------------------------*/

/* Timer 3 handle pointer */
extern TIM_HandleTypeDef htim3;
/* Handle of the timer associated with ADC timing acquisition, with a frequency of 2000Hz */
extern TIM_HandleTypeDef htim2;
/* Timing reading of gyroscope data with timer 4 interrupt (Fre = 20Hz) */
extern TIM_HandleTypeDef htim4;

/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* 
		Timer 3 is interrupted periodically(Fre = 100Hz), 
		and the receive clearance function of serial port 6 is invoked 
	*/
	if (htim == (&htim3))
	{
        if(IsCompleteHardwareInit() == Operation_Success)
        {
            USART6_RecvDataClear();
        }
	}
    
	/* 
		Timer 2 is interrupted periodically(Fre = 2000Hz), 
		and Implementation of ADC timing multi - channel sampling conversion
	*/
	if(htim == (&htim2))
	{
        if(IsCompleteHardwareInit() == Operation_Success)
        {
//            ADC_KalmanFilter_Value_Test();
        }
	}
    
    /*
        Timer 4 Periodic interrupt (Fre = 20Hz)
        and is used for timed data return of serial gyro JY-60
    */
    if(htim == (&htim4))
	{
        if(IsCompleteHardwareInit() == Operation_Success)
        {
//           USART_Gyroscope_MeanFilter_Test();
        }
	}
}



