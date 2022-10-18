/**
  ******************************************************************************
  * File Name          : DigtalSignal_Process.c
  * Description        : The file includes commonly used digital filtering, 
  *						 frequency domain transformation and motion control algorithms
  ******************************************************************************
 */
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIGITAL_H
#define __DIGITAL_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/*
	Digital signal processing related library
*/
#include "arm_math.h"  
/* Common macro definitions---------------------------------------------------*/

/* Mean filter times, the more times, the slower the sensor data transformation */
#define MEAN_FILTER_NUM	3
/* Number of fast Fourier transform points */
#define FFT_LENGTH 256

/* Whether to use functions from the ARM-DSP library or use inefficient digital processing libraries */
#define _DSP_ABS_USED 			0U
#define _DSP_OFFSET_USED 		0U
#define _DSP_MAX_USED 			1U
#define _DSP_SCALE_USED 		0U

/* Data structure declaration-------------------------------------------------*/

/* mean filter structure */
typedef struct 
{
	/* Cache three historical data, the size can be modified */
	float Data_Buf[MEAN_FILTER_NUM];
	
	/* The sum of the data */
	float sum;

	/* output result */
	float result; 
	
}Mean_Filter_F;

/* mean filter structure */
typedef struct 
{
	/* Cache three historical data, the size can be modified */
	uint16_t Data_Buf[MEAN_FILTER_NUM];
	
	/* The sum of the data */
	uint16_t sum;

	/* output result */
	uint16_t result; 
	
}Mean_Filter_U16;

/* Kalman Filter Structure */
typedef struct 
{
	/* last time Kalman filter predicted */
	float last_data;
	/* this time Kalman filter predicted */
	float now_data;
	/* The deviation at the initial time, it can't be zero, it can be anything */
	float error;
	/* 
	Process noise:
		r is fixed,
		A larger value of Q means more trust in the measurement, while an infinite value of Q means only using the measurement
		The smaller the Q value, the more trust the model predicted value, and the Q value is 0, the more trust the model predicted value
	q Indicates the smoothing degree of the curve after filtering. The smaller q is, the smoother the curve will be
	*/
	float q;
	/*
	Measurement noise:
		With the increase of R, the dynamic response becomes slower and the convergence stability becomes better. 
		On the contrary, the control response speed becomes faster and the convergence stability becomes worse
	The degree of similarity between the filtered curve and the measured curve after R parameter adjustment. 
	The smaller r is, the closer the filtered curve is to the measured curve.
	*/
	float r;
	/* Kalman filter gain */
	float kGain;
}Kalman_Filter;

/* Round the floating point number x to uint16_t */
#define ROUND_TO_UINT16(x)   ((uint16_t)(x)+0.5)>(x)? ((uint16_t)(x)):((uint16_t)(x)+1)

/* Function declaration-------------------------------------------------------*/

/* Mean filtering function */
float Data_Mean_Filter_F(Mean_Filter_F* p_MeanFilterStruct,float Temp_Data_Buf[]);
/* Mean filtering Reset function */
void Mean_Filter_Rest_F(Mean_Filter_F* p_MeanFilterStruct);

/* Mean filtering function */
uint16_t Data_Mean_Filter_U16(Mean_Filter_U16* p_MeanFilterStruct,uint16_t Temp_Data_Buf[]);
/* Mean filtering Reset function */
void Mean_Filter_Rest_U16(Mean_Filter_U16* p_MeanFilterStruct);

/* Initialize the Kalman filter */
void KalmanFilter_Init(Kalman_Filter* p_Kalman_Filter);
/* Data were filtered by Kalman filter */
float KalmanFilter_Calculate(Kalman_Filter* p_Kalman_Filter , float InData);

/* Get the absolute value of an array */
void Get_DataBuff_Abs(float32_t* p_SrcBuff,float32_t* p_DstpBuff,uint32_t Buff_Size);
/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/* Get the offset additioned value of an array */
void Get_DataBuff_Offeset(float32_t* p_SrcBuff, float32_t offset,float32_t* p_DstpBuff,uint32_t Buff_Size);
/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/* The target array is multiplied by the proportionality constant */
void Get_DataBuff_Scale(float32_t* p_SrcBuff, float32_t ratio,float32_t* p_DstpBuff,uint32_t Buff_Size);
/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/* Computes the maximum value of the data array. This function returns the maximum value and its position in the array. */
void Get_DataBuff_Max(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result, uint32_t* p_Index);

/* Functions for testing digital signals */
void DigtalSignal_Process_Test(void);

#ifdef __cplusplus
}
#endif
#endif /*__DIGITAL_H*/


