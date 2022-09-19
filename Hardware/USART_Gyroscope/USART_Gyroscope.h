/**
  ******************************************************************************
  * File Name          : key.h
  * Description        : This file includes the declaration and macro definition 
  *                      of the serial gyroscope read and write operation functions
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYROSCOPE_H
#define __GYROSCOPE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/


/* Data structure declaration-------------------------------------------------*/

/* 
	+Gyroscope data cache structure : Acceleration, angular acceleration, Angle+ 
*/

/* Structure of acceleration */
typedef struct 
{
	short Acc_DataBuf[3];
	short T_Data;
}Struct_Acc;
/* Angular acceleration structure */
typedef struct 
{
	short Angle_Acc_DataBuf[3];
	short T_Data;
}Struct_Gyro;
/* Angular structure */
typedef struct
{
	short Angle_DataBuf[3];
	short T_Data;
}Struct_Angle;

/* Function declaration-------------------------------------------------------*/

/* The parameters of the gyroscope data are analyzed */
void CopeSerial2Data(unsigned char ucData);
/* Serial port send command function */
t_FuncRet Send_Command(uint8_t data[3]);
/* Data conversion */
void Get_TrueData(void);


#ifdef __cplusplus
}
#endif
#endif /*__GYROSCOPE_H*/



