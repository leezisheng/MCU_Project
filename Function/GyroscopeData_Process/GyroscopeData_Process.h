/**
  ******************************************************************************
  * File Name          : GyroscopeData_Process.h
  * Description        : This file includes the declaration of the function for 
  *                      mean filtering the motion data collected by the gyroscope
  ******************************************************************************
 */
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYROSCOPEDATA_PROCESS_H
#define __GYROSCOPEDATA_PROCESS_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

/* Data structure declaration-------------------------------------------------*/

/* Extern variables-----------------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* Get the Mean filter the motion data value */
t_FuncRet Get_MotionData_MeanFilter_Value(float* p_angle_x , 
								   float* p_angle_y ,
						           float* p_angle_z ,
						           float* p_gyro_x  ,
								   float* p_gyro_y  ,
						           float* p_gyro_z );


#ifdef __cplusplus
}
#endif
#endif /*__GYROSCOPEDATA_PROCESS_H*/
 
 
  