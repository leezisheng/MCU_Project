/**
  ******************************************************************************
  * File Name          : ADC_Function.c
  * Description        : This file contains the ADC digital filter function 
  *                      definitions and ADC data variables
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "ADC_Function.h"
#include "ADC_Operation.h"
#include "DigtalSignal_Process.h"

/* External function declaration----------------------------------------------*/

/* Obtain the voltage values collected by the 2 channels */
extern t_FuncRet ADC_Get_Data(void);
/* Obtain the voltage of no. 1 EMG sensor */
extern t_FuncRet ADC_Get_SensorData_1(uint16_t* p_Sensor_V_Data);
/* Obtain the voltage of no. 2 EMG sensor */
extern t_FuncRet ADC_Get_SensorData_2(uint16_t* p_Sensor_V_Data);
/* Obtain the voltage of no. 3 EMG sensor */
extern t_FuncRet ADC_Get_SensorData_3(uint16_t* p_Sensor_V_Data);
/* Obtain the voltage of no. 4 EMG sensor */
extern t_FuncRet ADC_Get_SensorData_4(uint16_t* p_Sensor_V_Data);
/* Obtain the voltage of Vref */
extern t_FuncRet ADC_Get_Vref(uint16_t* p_Vref);

/* Mean filtering function */
extern uint16_t Data_Mean_Filter_U16(Mean_Filter_U16* p_MeanFilterStruct,uint16_t Temp_Data_Buf[]);
/* Mean filtering Reset function */
extern void Mean_Filter_Rest_U16(Mean_Filter_U16* p_MeanFilterStruct);

/* Initialize the Kalman filter */
extern void KalmanFilter_Init(Kalman_Filter* p_Kalman_Filter);
/* Data were filtered by Kalman filter */
extern float KalmanFilter_Calculate(Kalman_Filter* p_Kalman_Filter , float InData);

/* Private macro definitions--------------------------------------------------*/

/* Global variable------------------------------------------------------------*/

/* Cache array of ADC voltage acquisition results */
static uint16_t Sensor_No1_DataBuf[MEAN_FILTER_NUM];
static uint16_t Sensor_No2_DataBuf[MEAN_FILTER_NUM];
static uint16_t Sensor_No3_DataBuf[MEAN_FILTER_NUM];
static uint16_t Sensor_No4_DataBuf[MEAN_FILTER_NUM];
static uint16_t Sensor_Vref_DataBuf[MEAN_FILTER_NUM];
/* Array index */
static uint8_t DataBuf_Index = 0;

/* Voltage value Kalman filter structure */
static Kalman_Filter KalmanFilterStruct_Sensor_No1 = {0};
static Kalman_Filter KalmanFilterStruct_Sensor_No2 = {0};
static Kalman_Filter KalmanFilterStruct_Sensor_No3 = {0};
static Kalman_Filter KalmanFilterStruct_Sensor_No4 = {0};
static Kalman_Filter KalmanFilterStruct_Sensor_Vref= {0};
/* Determines whether the Kalman filter structure is initialized */
static bool KalmanFilterInitFlag = (bool)FALSE;

/* Static function definition-------------------------------------------------*/

/* Function definition--------------------------------------------------------*/

/** 
* @description: Get the Mean filter voltage value
* @param  {uint16_t*}  p_Sensor1_V_Data : Voltage after filtering
* @param  {uint16_t*}  p_Sensor2_V_Data : Voltage after filtering
* @param  {uint16_t*}  p_Sensor3_V_Data : Voltage after filtering
* @param  {uint16_t*}  p_Sensor4_V_Data : Voltage after filtering
* @param  {uint16_t*}  p_Vref_V_Data    : Voltage after filtering
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Get_ADC_MeanFilter_Value(uint16_t* p_Sensor1_V_Data , 
								   uint16_t* p_Sensor2_V_Data ,
						           uint16_t* p_Sensor3_V_Data ,
						           uint16_t* p_Sensor4_V_Data ,
						           uint16_t* p_Vref_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	/* Initializes the filter structure */
	Mean_Filter_U16 FilterStruct = {0};
	
	/* Turn on ADC voltage acquisition once */
	ret = ADC_Get_Data();
	if(ret != (t_FuncRet)Operation_Success)
	{
		return ret;
	}
	
	/* Index Value Judgment */
	if(DataBuf_Index >= (MEAN_FILTER_NUM-1))
	{
		DataBuf_Index = 0;
	}
	else
	{
		DataBuf_Index++;
	}
	
	/* Voltage signals are collected synchronously by five channels */
	ADC_Get_SensorData_1(&Sensor_No1_DataBuf[DataBuf_Index]);
	ADC_Get_SensorData_2(&Sensor_No2_DataBuf[DataBuf_Index]);
	ADC_Get_SensorData_3(&Sensor_No3_DataBuf[DataBuf_Index]);
	ADC_Get_SensorData_4(&Sensor_No4_DataBuf[DataBuf_Index]);
	ADC_Get_Vref(&Sensor_Vref_DataBuf[DataBuf_Index]);

	/* Mean filtering */
	*p_Sensor1_V_Data = Data_Mean_Filter_U16((Mean_Filter_U16*)&FilterStruct ,(uint16_t*)Sensor_No1_DataBuf);
	Mean_Filter_Rest_U16((Mean_Filter_U16*)&FilterStruct);
	
	*p_Sensor2_V_Data = Data_Mean_Filter_U16((Mean_Filter_U16*)&FilterStruct ,(uint16_t*)Sensor_No2_DataBuf);
	Mean_Filter_Rest_U16((Mean_Filter_U16*)&FilterStruct);
	
	*p_Sensor3_V_Data = Data_Mean_Filter_U16((Mean_Filter_U16*)&FilterStruct ,(uint16_t*)Sensor_No3_DataBuf);
	Mean_Filter_Rest_U16((Mean_Filter_U16*)&FilterStruct);
	
	*p_Sensor4_V_Data = Data_Mean_Filter_U16((Mean_Filter_U16*)&FilterStruct ,(uint16_t*)Sensor_No4_DataBuf);
	Mean_Filter_Rest_U16((Mean_Filter_U16*)&FilterStruct);
	
	*p_Vref_V_Data    = Data_Mean_Filter_U16((Mean_Filter_U16*)&FilterStruct ,(uint16_t*)Sensor_Vref_DataBuf);
	Mean_Filter_Rest_U16((Mean_Filter_U16*)&FilterStruct);
	
	/* Result back on the array */
	Sensor_No1_DataBuf[DataBuf_Index]  = *p_Sensor1_V_Data;
	Sensor_No2_DataBuf[DataBuf_Index]  = *p_Sensor2_V_Data;
	Sensor_No3_DataBuf[DataBuf_Index]  = *p_Sensor3_V_Data;
	Sensor_No4_DataBuf[DataBuf_Index]  = *p_Sensor4_V_Data;
	Sensor_Vref_DataBuf[DataBuf_Index] = *p_Vref_V_Data;

	return ret;
}

/** 
* @description: Obtain the result after Kalman filter
* @param  {float*}  p_Sensor1_V_Data : Voltage after filtering
* @param  {float*}  p_Sensor2_V_Data : Voltage after filtering
* @param  {float*}  p_Sensor3_V_Data : Voltage after filtering
* @param  {float*}  p_Sensor4_V_Data : Voltage after filtering
* @param  {float*}  p_Vref_V_Data    : Voltage after filtering
* @return {t_FuncRet } : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Get_ADC_KalmanFilter_Value(float* p_Sensor1_V_Data , 
						             float* p_Sensor2_V_Data ,
						             float* p_Sensor3_V_Data ,
						             float* p_Sensor4_V_Data ,
						             float* p_Vref_V_Data)
{
	t_FuncRet ret= (t_FuncRet)Operation_Success;
	
	uint16_t temp_Sensor1_V_Data = 0;
    uint16_t temp_Sensor2_V_Data = 0;
	uint16_t temp_Sensor3_V_Data = 0;
	uint16_t temp_Sensor4_V_Data = 0;
	uint16_t temp_Vref_V_Data    = 0;
	
	/* First time set the Kalman filter parameters */
	if(KalmanFilterInitFlag == (bool)FALSE)
	{
		KalmanFilter_Init((Kalman_Filter*)&KalmanFilterStruct_Sensor_No1);
		KalmanFilter_Init((Kalman_Filter*)&KalmanFilterStruct_Sensor_No2);
		KalmanFilter_Init((Kalman_Filter*)&KalmanFilterStruct_Sensor_No3);
		KalmanFilter_Init((Kalman_Filter*)&KalmanFilterStruct_Sensor_No4);
		KalmanFilter_Init((Kalman_Filter*)&KalmanFilterStruct_Sensor_Vref);
		
		KalmanFilterInitFlag = (bool)TRUE;
	}

	/* Get the median filter voltage value */
	ret = Get_ADC_MeanFilter_Value((uint16_t*)&temp_Sensor1_V_Data , 
								   (uint16_t*)&temp_Sensor2_V_Data ,
						           (uint16_t*)&temp_Sensor3_V_Data ,
						           (uint16_t*)&temp_Sensor4_V_Data ,
						           (uint16_t*)&temp_Vref_V_Data);
	if(ret != (t_FuncRet)Operation_Success)
	{
		return ret;
	}
	
	/* The voltage data after Kalman filter is obtained */
	*p_Sensor1_V_Data = ROUND_TO_UINT16(1000*KalmanFilter_Calculate((Kalman_Filter*)&KalmanFilterStruct_Sensor_No1 , (float)temp_Sensor1_V_Data/1000));
	*p_Sensor2_V_Data = ROUND_TO_UINT16(1000*KalmanFilter_Calculate((Kalman_Filter*)&KalmanFilterStruct_Sensor_No2 , (float)temp_Sensor2_V_Data/1000));
	*p_Sensor3_V_Data = ROUND_TO_UINT16(1000*KalmanFilter_Calculate((Kalman_Filter*)&KalmanFilterStruct_Sensor_No3 , (float)temp_Sensor3_V_Data/1000));
	*p_Sensor4_V_Data = ROUND_TO_UINT16(1000*KalmanFilter_Calculate((Kalman_Filter*)&KalmanFilterStruct_Sensor_No4 , (float)temp_Sensor4_V_Data/1000));
	*p_Vref_V_Data    = ROUND_TO_UINT16(1000*KalmanFilter_Calculate((Kalman_Filter*)&KalmanFilterStruct_Sensor_Vref, (float)temp_Vref_V_Data/1000));

	return ret;
}
 


