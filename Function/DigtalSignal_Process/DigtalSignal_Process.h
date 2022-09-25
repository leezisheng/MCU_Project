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

/* Common macro definitions---------------------------------------------------*/

/* Mean filter times, the more times, the slower the sensor data transformation */
#define MEAN_FILTER_NUM	3

/* Data structure declaration-------------------------------------------------*/

/* mean filter structure */
typedef struct 
{
	/* Cache three historical data, the size can be modified */
	uint16_t Data_Buf[MEAN_FILTER_NUM];
	
	/* The sum of the data */
	uint32_t sum;

	/* output result */
	uint16_t result; 
	
}Mean_Filter;

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


/* Function declaration-------------------------------------------------------*/

/* Mean filtering function */
uint16_t Data_Mean_Filter(Mean_Filter* p_MeanFilterStruct,uint16_t Temp_Data_Buf[]);
/* Mean filtering Reset function */
void Mean_Filter_Rest(Mean_Filter* p_MeanFilterStruct);
/* Initialize the Kalman filter */
void KalmanFilter_Init(Kalman_Filter* p_Kalman_Filter);
/* Data were filtered by Kalman filter */
float KalmanFilter_Calculate(Kalman_Filter* p_Kalman_Filter , float InData);


#ifdef __cplusplus
}
#endif
#endif /*__DIGITAL_H*/

