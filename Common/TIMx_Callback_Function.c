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
/* Test the count variable for the 10Hz interrupt in timer 3 */
static uint32_t tim3_count = 0;
/* Handle of the timer associated with ADC timing acquisition, with a frequency of 2000Hz */
extern TIM_HandleTypeDef htim2;
/* Test the count variable for the 2000Hz interrupt in timer 2 */
static uint32_t tim2_count = 0;

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* 
		Timer 3 is interrupted periodically(10Hz), 
		and the receive clearance function of serial port 6 is invoked 
	*/
	if (htim == (&htim3))
	{
		UART6_Reset();
		
		tim3_count++;
		if(tim3_count == 10)
		{
			tim3_count = 0;
		}
	}
	/* 
		Timer 2 is interrupted periodically(2000Hz), 
		and Implementation of ADC timing multi - channel sampling conversion
	*/
	if(htim == (&htim2))
	{
		tim2_count++;
	    if(tim2_count == 2000)
		{
			tim2_count = 0;
		}
	}
}



