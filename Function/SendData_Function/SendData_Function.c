/**
  ******************************************************************************
  * File Name          : SendData_Function.c
  * Description        : This file defines the structure and functions for sending data to the upmachine
  ******************************************************************************
 */
  
/* Includes ------------------------------------------------------------------*/
#include "SendData_Function.h"
#include "USART_Printf.h"

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/

/* Serial port send macro definition */
#define DataWrite  USART2_SendBuf_Polling

/* Global variable------------------------------------------------------------*/


/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/** 
* @description                : A function that sends data to PC 
* @param   {uint8_t} DataType : Incoming data type
*                               ADC_TYPE (0) -Datax The data type is float
*                               GYROSCOPE_TYPE (1) -Datax The data type is uint16
* @param   {void*}   Datax    : Data that needs to be sent and whose data type is uncertain
* @return  {t_FuncRet}        : if success , return (t_FuncRet)Operation_Success
* @example :
*           SendDataToPC(ADC_TYPE, (void*)&Sensor1_V_Data_F, (void*)&Sensor2_V_Data_F, (void*)&Sensor3_V_Data_F, (void*)&Sensor4_V_Data_F);
*           SendDataToPC(GYROSCOPE_TYPE, (void*)&angle_x, (void*)&angle_y, (void*)&acc_x, (void*)&acc_y);
* @author: leeqingshui 
*/
t_FuncRet SendDataToPC(uint8_t DataType, void* Data0, void* Data1, void* Data2, void* Data3)
{
    t_FuncRet ret = Operation_Success;
    
    /* Determines whether the data type is correct */
    if(IS_TRUE_DATATYPE(DataType))
    {
        /* Initializes the data sending structure */
        SendDataToPCFrame SendDataStruct;
        
        uint16_t u16_temp_data0, u16_temp_data1, u16_temp_data2, u16_temp_data3;
        
        SendDataStruct.HeaderFrame1 = (uint8_t)FRAME_HEADER;
        SendDataStruct.HeaderFrame2 = (uint8_t)FRAME_HEADER;
        SendDataStruct.DataType     = (uint8_t)DataType;
        SendDataStruct.Stop         = (uint8_t)FRAME_STOP;
        
        /* The gyroscope output Angle and angular velocity data of the data type float32_t */
        if(DataType == GYROSCOPE_TYPE)
        {
            /* Exclude cases where the data is negative */
            if(*((float32_t*)Data0)<0)
            {
                *((float32_t*)Data0) = 0;
            }
            
            if(*((float32_t*)Data1)<0)
            {
                *((float32_t*)Data1) = 0;
            }
            
            if(*((float32_t*)Data2)<0)
            {
                *((float32_t*)Data2) = 0;
            }
            
            if(*((float32_t*)Data3)<0)
            {
                *((float32_t*)Data3) = 0;
            }
            
            /* The data is expanded 100 times */
            u16_temp_data0 = (uint16_t)( (*((float32_t*)Data0))*100 );
            u16_temp_data1 = (uint16_t)( (*((float32_t*)Data1))*100 );
            u16_temp_data2 = (uint16_t)( (*((float32_t*)Data2))*100 );
            u16_temp_data3 = (uint16_t)( (*((float32_t*)Data3))*100 );
            
            SendDataStruct.Data0_H = GET_HIGH_BYTE(u16_temp_data0);
            SendDataStruct.Data0_L = GET_LOW_BYTE(u16_temp_data0);
            SendDataStruct.Data1_H = GET_HIGH_BYTE(u16_temp_data1);
            SendDataStruct.Data1_L = GET_LOW_BYTE(u16_temp_data1);
            SendDataStruct.Data2_H = GET_HIGH_BYTE(u16_temp_data2);
            SendDataStruct.Data2_L = GET_LOW_BYTE(u16_temp_data2);
            SendDataStruct.Data3_H = GET_HIGH_BYTE(u16_temp_data3);
            SendDataStruct.Data3_L = GET_LOW_BYTE(u16_temp_data3);
        }
        else if(DataType == ADC_TYPE)
        {
            u16_temp_data0 = (uint16_t)(*((uint16_t*)Data0));
            u16_temp_data1 = (uint16_t)(*((uint16_t*)Data1));
            u16_temp_data2 = (uint16_t)(*((uint16_t*)Data2));
            u16_temp_data3 = (uint16_t)(*((uint16_t*)Data3));
            
            SendDataStruct.Data0_H = GET_HIGH_BYTE(u16_temp_data0);
            SendDataStruct.Data0_L = GET_LOW_BYTE(u16_temp_data0);
            SendDataStruct.Data1_H = GET_HIGH_BYTE(u16_temp_data1);
            SendDataStruct.Data1_L = GET_LOW_BYTE(u16_temp_data1);
            SendDataStruct.Data2_H = GET_HIGH_BYTE(u16_temp_data2);
            SendDataStruct.Data2_L = GET_LOW_BYTE(u16_temp_data2);
            SendDataStruct.Data3_H = GET_HIGH_BYTE(u16_temp_data3);
            SendDataStruct.Data3_L = GET_LOW_BYTE(u16_temp_data3);
        }
        
        /* Data transmission */
        DataWrite((uint8_t*)&SendDataStruct,sizeof(SendDataStruct));
    }
    else
    {
        /* The data type is incorrect */
        ret = Operation_Fail;
    }

    return ret;
}


