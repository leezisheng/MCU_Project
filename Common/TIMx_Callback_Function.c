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
#include "SendData_Function.h"
#include "tim.h"
#include "GyroscopeData_Process.h"
#include "ADC_Function.h"

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/

/* Whether to use the receive wait delay */
//#define USE_USB_RECV_DELAY                  0
#ifdef USE_USB_RECV_DELAY
    /* Receive wait time parameter */
    #define WAIT_ACK_SIGNAL_DELAY_TIME          255
#endif

/* Global variable------------------------------------------------------------*/

/* Timer 3 is interrupted periodically(Fre = 100Hz), and the receive clearance function of serial port 6 is invoked  */
extern TIM_HandleTypeDef htim3;
/* Handle of the timer associated with ADC timing acquisition, with a frequency of 2000Hz */
extern TIM_HandleTypeDef htim2;
/* Timing reading of gyroscope data with timer 4 interrupt (Fre = 20Hz) */
extern TIM_HandleTypeDef htim4;
/* Ack signal received flag bit */
extern bool AckSignalRecvFlag;
/* USB virtual serial port receiving flag bit */
extern bool USBRecvSuccessFlag;

/* Adc-related static global variables */
static uint16_t Temp_Sensor1_V_Data          = 0;
static uint16_t Temp_Sensor2_V_Data          = 0;
static uint16_t Temp_Sensor3_V_Data          = 0;
static uint16_t Temp_Sensor4_V_Data          = 0;
static uint16_t Temp_Vref                    = 0;

/* Gyroscope related data */
volatile static float32_t Temp_angle_x       = 0;
volatile static float32_t Temp_angle_y       = 0;
volatile static float32_t Temp_angle_z       = 0;
volatile static float32_t Temp_gyro_x        = 0;
volatile static float32_t Temp_gyro_y        = 0;
volatile static float32_t Temp_gyro_z        = 0;

#ifdef USE_USB_RECV_DELAY
    /* The delay variable waiting for a response signal to arrive */
    uint8_t wait_ack_delay_val = WAIT_ACK_SIGNAL_DELAY_TIME
#endif

uint32_t count = 0;

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
            /* First of all, STM32 sends the synchronization sequence signal to the upper computer */
            SendSyncSignalToPC();
            
            /* After receiving the synchronization signal, the upper computer send to PC the ack signal */
            
            /* Whether to use the receive wait delay */
            #ifdef USE_USB_RECV_DELAY
            
            /* Wait for the Ack signal to arrive */
            for(wait_ack_delay_val = WAIT_ACK_SIGNAL_DELAY_TIME;wait_ack_delay_val != 0;wait_ack_delay_val = wait_ack_delay_val-1)
            {
                /* The USB virtual serial port receives data and jumps out of the loop */
                if(USBRecvSuccessFlag != (bool)FALSE) 
                {
                    break;
                }
            }
            /* When the delay is reached, the while loop breaks out and the wait_ack_delay_val variable restores to its original value */
            wait_ack_delay_val = WAIT_ACK_SIGNAL_DELAY_TIME;
            /* Complete data reception, reset flag bit */
            USBRecvSuccessFlag = (bool)FALSE;
            
            #endif
            
            /* The synchronization signal was received successfully */
            if(AckSignalRecvFlag != (bool)FALSE)
            {
                count = count+1;
                Runtime_Calculate_Start_Hardware();
                /* Get the Mean filter voltage value , This is a three-point mean */
                Get_ADC_MeanFilter_Value(&Temp_Sensor1_V_Data, &Temp_Sensor2_V_Data, &Temp_Sensor3_V_Data, &Temp_Sensor4_V_Data, &Temp_Vref);
                /* After sending the ack signal, STM32 sends the data structure */
                SendDataToPC(ADC_TYPE, (void*)&Temp_Sensor1_V_Data, (void*)&Temp_Sensor2_V_Data, (void*)&Temp_Sensor3_V_Data, (void*)&Temp_Sensor4_V_Data);
                /* The successful reception flag bit of the synchronization signal is set to 0 */
                AckSignalRecvFlag = (bool)FALSE;
                Runtime_Calculate_Finish_Hardware();
            }
            /* If no synchronization signal is received or an incorrect synchronization signal is received, skip this step */
            else
            {
                return;
            }
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
           Get_MotionData_MeanFilter_Value((float*)&Temp_angle_x , 
										   (float*)&Temp_angle_y ,
						                   (float*)&Temp_angle_z ,
						                   (float*)&Temp_gyro_x  ,
								           (float*)&Temp_gyro_y  ,
						                   (float*)&Temp_gyro_z );
        }
	}
}



