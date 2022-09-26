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

/* Get the median filter the motion data value */
t_FuncRet Get_MotionData_MeanFilter_Value(uint16_t* p_angle_x , 
								   uint16_t* p_angle_y ,
						           uint16_t* p_angle_z ,
						           uint16_t* p_gyro_x  ,
								   uint16_t* p_gyro_y  ,
						           uint16_t* p_gyro_z );

#ifdef __cplusplus
}
#endif
#endif /*__GYROSCOPEDATA_PROCESS_H*/
 
 
  