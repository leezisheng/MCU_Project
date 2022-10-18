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

/* An input array for testing */
volatile static float32_t TempBuff[FFT_LENGTH*2] = {0};
/* A processed array for testing */
volatile static float32_t ProcessedTempBuff[FFT_LENGTH*2] = {0};

/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/** 
* @description: Mean filtering function
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @param  {float*}    Temp_Data_Buf      : Cache array acquisition results
* @return {float}                        : Mean filtering result
* @author: leeqingshui 
*/
float Data_Mean_Filter_F(Mean_Filter_F* p_MeanFilterStruct,float Temp_Data_Buf[])
{
    p_MeanFilterStruct->sum = 0;
	 
	for(int count = 0;count < MEAN_FILTER_NUM;count++)
	{
		p_MeanFilterStruct->Data_Buf[count] = Temp_Data_Buf[count];
		p_MeanFilterStruct->sum = p_MeanFilterStruct->sum + p_MeanFilterStruct->Data_Buf[count];
	}
	
	p_MeanFilterStruct->result = (float)(p_MeanFilterStruct->sum / MEAN_FILTER_NUM);

	return p_MeanFilterStruct->result;
}
 
/** 
* @description: Mean filtering Reset function 
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @return {void}                       
* @author: leeqingshui 
*/
void Mean_Filter_Rest_F(Mean_Filter_F* p_MeanFilterStruct)
{
	 for(int count = 0;count < MEAN_FILTER_NUM;count++)
	 {
		 p_MeanFilterStruct->Data_Buf[count] = 0;
	 }
	 
	 p_MeanFilterStruct->result = 0;
	 
	 p_MeanFilterStruct->sum = 0;
}

/** 
* @description: Mean filtering function
* @param  {Mean_Filter*} p_MeanFilterStruct : Filter structure pointer
* @param  {uint16_t*}    Temp_Data_Buf      : Cache array acquisition results
* @return {float}                           : Mean filtering result
* @author: leeqingshui 
*/
uint16_t Data_Mean_Filter_U16(Mean_Filter_U16* p_MeanFilterStruct,uint16_t Temp_Data_Buf[])
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
void Mean_Filter_Rest_U16(Mean_Filter_U16* p_MeanFilterStruct)
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
	p_Kalman_Filter->error     = 0.01;
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


/** 
* @description: Get the absolute value of an array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Abs(float32_t* p_SrcBuff,float32_t* p_DstpBuff,uint32_t Buff_Size)
{	
	#if(_DSP_ABS_USED == 1)
		arm_abs_f32(p_SrcBuff,p_DstpBuff,Buff_Size);
	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			if((*(p_SrcBuff + i))<0)
			{
				*(p_DstpBuff + i) = -(*(p_SrcBuff + i));
			}
		}
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: Get the offset additioned value of an array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t}  offset     : Data offset
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Offeset(float32_t* p_SrcBuff, float32_t offset,float32_t* p_DstpBuff,uint32_t Buff_Size)
{
	#if(_DSP_OFFSET_USED == 1)
		arm_offset_f32(p_SrcBuff, offset, p_DstpBuff, Buff_Size);	

	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			*(p_DstpBuff + i) = *(p_SrcBuff + i) + offset;
		}
	
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: Computes the maximum value of the data array. This function returns the maximum value and its position in the array
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @param  {float32_t*} p_Result   : the maximum value of the data array
* @param  {uint32_t*}  p_Index    : The maximum element is indexed in the array
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Max(float32_t* p_SrcBuff, uint32_t Buff_Size, float32_t* p_Result, uint32_t* p_Index)
{
	#if(_DSP_MAX_USED == 1)
		arm_max_f32 (p_SrcBuff, Buff_Size, p_Result, p_Index);
	
	#else
		// Assume that the first element in the array is the maximum value
		float32_t temp_max = *p_SrcBuff;
		uint32_t  temp_index = 0;
		
		// Go through the group, getting each element, ready to compare
		for(uint32_t i = 1; i < Buff_Size; i++)
		{
			// If something larger than max occurs during the comparison, let max record the larger value
			if((*(p_SrcBuff + i)) > temp_max)
			{
					temp_max = *(p_SrcBuff + i);
					temp_index = i;
			}
		}
		
		// The loop ends and the result is retrieved
		*p_Result = temp_max;
		*p_Index  = temp_index;
		
	#endif
}

/* !! 
	This function causes Hardfault for an unknown reason and does not perform properly !!
	Notice here that if you use sizeof on an array to find its length, it will cause Hardfault
*/
/** 
* @description: The target array is multiplied by the proportionality constant
* @param  {float32_t*} p_SrcBuff  : A pointer to the array to be processed
* @param  {float32_t}  ratio      : the proportionality constant
* @param  {float32_t*} p_DstpBuff : A pointer to the processed array
* @param  {uint32_t}   Buff_Size  : Size of Data Buffer
* @return {void}                         
* @author: leeqingshui 
*/
void Get_DataBuff_Scale(float32_t* p_SrcBuff, float32_t ratio,float32_t* p_DstpBuff,uint32_t Buff_Size)
{
	#if(_DSP_SCALE_USED == 1)
		arm_scale_f32(p_SrcBuff, ratio, p_DstpBuff, Buff_Size);	
	
	#else
		for(uint32_t i = 0;i<Buff_Size;i++)
		{
			*(p_DstpBuff + i) = *(p_SrcBuff + i)*ratio;
		}
	
	#endif
}


/* Functions for testing digital signals */
void DigtalSignal_Process_Test(void)
{
	static float32_t Result = 0;
	static uint32_t  Index  = 0;
	
	/* Generate a sinusoidal signal */
	for(int i=0;i<2*FFT_LENGTH;i++)
	{
		TempBuff[i]=(float32_t)(-100+i*1);	
	}
	
	Get_DataBuff_Offeset((float32_t*)TempBuff, (float32_t)50,(float32_t*)ProcessedTempBuff,2*FFT_LENGTH);
}
