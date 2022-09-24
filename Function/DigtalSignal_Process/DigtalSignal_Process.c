/**
  ******************************************************************************
  * File Name          : DigtalSignal_Process.c
  * Description        : The file includes commonly used digital filtering, 
  *						 frequency domain transformation and motion control algorithms
  ******************************************************************************
 */
  
/* Includes ------------------------------------------------------------------*/
#include "DigtalSignal_Process.h"

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/


/* Global variable------------------------------------------------------------*/


/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/** 
* @description: Mean filtering function
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @param  {uint16_t*}    Temp_Data_Buf      : Cache array of ADC voltage acquisition results
* @param  {uint16_t}     New_Data           : ADC voltage acquisition results
* @return {uint16_t}                        : Mean filtering result
* @author: leeqingshui 
*/
uint16_t Data_Mean_Filter(Mean_Filter* p_MeanFilterStruct,uint16_t Temp_Data_Buf[])
 {
    p_MeanFilterStruct->sum = 0;
	 
	for(int count = 0;count < MEAN_FILTER_NUM;count++)
	{
		p_MeanFilterStruct->Data_Buf[count] = Temp_Data_Buf[count];
		p_MeanFilterStruct->sum = p_MeanFilterStruct->sum + p_MeanFilterStruct->Data_Buf[count];
	}
	
	p_MeanFilterStruct->result = (uint16_t)(p_MeanFilterStruct->sum / MEAN_FILTER_NUM);

	return p_MeanFilterStruct->result;
 }
 
/** 
* @description: Mean filtering Reset function 
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void Mean_Filter_Rest(Mean_Filter* p_MeanFilterStruct)
 {
	 for(int count = 0;count < MEAN_FILTER_NUM;count++)
	 {
		 p_MeanFilterStruct->Data_Buf[count] = 0;
	 }
	 
	 p_MeanFilterStruct->result = 0;
	 
	 p_MeanFilterStruct->sum = 0;
 }

/** 
* @description: Initialize the Kalman filter
* @param  {Kalman_Filter*} p_Kalman_Filter : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void KalmanFilter_Init(Kalman_Filter* p_Kalman_Filter)
{
	p_Kalman_Filter->error     = 10;
	p_Kalman_Filter->last_data = 0;
	p_Kalman_Filter->now_data  = 0;
	p_Kalman_Filter->q         = 0.01;
	p_Kalman_Filter->r         = 0.01;
	p_Kalman_Filter->kGain     = 0;
}

/** 
* @description: Data were filtered by Kalman filter 
* @param  {float} InData                   : Kalman filter input value
* @param  {Kalman_Filter*} p_Kalman_Filter : Filter structure pointer
* @return {float}                          : Kalman filter prediction value
* @author: leeqingshui 
*/
float KalmanFilter_Calculate(Kalman_Filter* p_Kalman_Filter , float InData)
{
	p_Kalman_Filter->now_data = InData;
	p_Kalman_Filter->error = p_Kalman_Filter->error + p_Kalman_Filter->q;
	
	/* Compute the Kalman gain */
	p_Kalman_Filter->kGain = (p_Kalman_Filter->error)/(p_Kalman_Filter->error + p_Kalman_Filter->r);
	// Calculate the filter estimate
	p_Kalman_Filter->now_data = p_Kalman_Filter->last_data + ((p_Kalman_Filter->kGain)*(p_Kalman_Filter->now_data - p_Kalman_Filter->last_data));
	// Update the measurement variance
	p_Kalman_Filter->error = (1-p_Kalman_Filter->kGain)*(p_Kalman_Filter->error);
	
	p_Kalman_Filter->last_data = p_Kalman_Filter->now_data;
	
	return (p_Kalman_Filter->now_data);
}


