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

/* Gyroscope calibration */
t_FuncRet Gyroscope_Calibration(void);
/* The parameters of the gyroscope data are analyzed */
t_FuncRet CopeSerial2Data(unsigned char ucData);
/* Serial port send command function */
t_FuncRet Send_Command(uint8_t data[3]);

/* Return the X-axis acceleration */
float Get_Xaxis_Acc(void);
/* Return the Y-axis acceleration */
float Get_Yaxis_Acc(void);
/* Return the Z-axis acceleration */
float Get_Zaxis_Acc(void);

/* Return the X-axis angel */
float Get_Xaxis_Angle(void);
/* Return the Y-axis angel */
float Get_Yaxis_Angle(void);
/* Return the Z-axis angel */
float Get_Zaxis_Angle(void);

/* Return the X-axis Angle_Acc */
float Get_Xaxis_Angle_Acc(void);
/* Return the Y-axis Angle_Acc */
float Get_Yaxis_Angle_Acc(void);
/* Return the Z-axis Angle_Acc */
float Get_Zaxis_Angle_Acc(void);

#ifdef __cplusplus
}
#endif
#endif /*__GYROSCOPE_H*/



