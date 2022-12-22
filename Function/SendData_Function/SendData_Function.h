/**
  ******************************************************************************
  * File Name          : SendData_Function.c
  * Description        : This file defines the structure and functions for sending data to the upmachine
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SENDDATA_FUNCTION_
#define _SENDDATA_FUNCTION_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

/* Data type macro definition */
#define ADC_TYPE                            0
#define GYROSCOPE_TYPE                      1

/* Format frame macro definition */
#define FRAME_HEADER                        0x55
#define FRAME_STOP                          0x78

/* The macro gets the lower octet of A */
#define GET_LOW_BYTE(DATA) 					((uint8_t)(DATA))
/* The macro gets the higher eight digits of A */
#define GET_HIGH_BYTE(DATA) 				((uint8_t)((DATA) >> 8))
/* The macro function synthesizes the high eight bits into 16 bits */
#define BYTE_TO_HW(DATA_A , DATA_B) 		((((uint16_t)(DATA_A)) << 8) | (uint8_t)(DATA_B))
/* Macro function to determine whether the data type is correct */
#define IS_TRUE_DATATYPE(PERIPH)            ((PERIPH == ADC_TYPE)||(PERIPH == GYROSCOPE_TYPE))

/* Data structure declaration-------------------------------------------------*/

/* 
    The upper computer sends data structures
    Communication protocol format£º
    | frame header | | frame header | DataType | DATAx_H | DATAx_L | Stop |
    x = 0~3
*/
typedef struct
{
    /* 
        frame header :
        Sending two 0x55 consecutively indicates data arrival 
    */
    uint8_t HeaderFrame1;
    uint8_t HeaderFrame2;
    
    /*
        DataType:
        Indicate whether the data is ADC data of SEMG sensor or Angle and angular velocity data of gyroscope
    */
    uint8_t DataType;
    
    /*
        DATAx
        DATAx_H : The high eight bits of data
        DATAx_L : The low eight bits of data
    */
    uint8_t Data0_H;
    uint8_t Data0_L;
    
    uint8_t Data1_H;
    uint8_t Data1_L;
    
    uint8_t Data2_H;
    uint8_t Data2_L;
    
    uint8_t Data3_H;
    uint8_t Data3_L;
    
    /*
        Stop
        Sending 0x78 indicates the end of sending a data frame
    */
    uint8_t Stop;
}SendDataToPCFrame;

/* Extern Variable------------------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* A function that sends data to PC */
t_FuncRet SendDataToPC(uint8_t DataType, void* Data0, void* Data1, void* Data2, void* Data3);

#ifdef __cplusplus
}
#endif
#endif /*_SENDDATA_FUNCTION_*/


