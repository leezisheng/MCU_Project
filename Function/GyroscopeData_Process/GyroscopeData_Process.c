/**
  ******************************************************************************
  * File Name          : GyroscopeData_Process.c
  * Description        : This file includes the declaration of the function for 
  *                      mean filtering the motion data collected by the gyroscope
  ******************************************************************************
 */
  
/* Includes ------------------------------------------------------------------*/
#include "GyroscopeData_Process.h"
#include "DigtalSignal_Process.h"
#include "USART_Gyroscope.h"


/* External function declaration----------------------------------------------*/

/* Return the X-axis angel */
extern float Get_Xaxis_Angle(void);
/* Return the Y-axis angel */
extern float Get_Yaxis_Angle(void);
/* Return the Z-axis angel */
extern float Get_Zaxis_Angle(void);

/* Return the X-axis Angle_Acc */
extern float Get_Xaxis_Angle_Acc(void);
/* Return the Y-axis Angle_Acc */
extern float Get_Yaxis_Angle_Acc(void);
/* Return the Z-axis Angle_Acc */
extern float Get_Zaxis_Angle_Acc(void);

/* Mean filtering function */
extern uint16_t Data_Mean_Filter(Mean_Filter* p_MeanFilterStruct,uint16_t Temp_Data_Buf[]);
/* Mean filtering Reset function */
extern void Mean_Filter_Rest(Mean_Filter* p_MeanFilterStruct);

/* Serial port 6 The receiver is cleared periodically */
extern t_FuncRet UART1_isRxComplete(void);

/* Private macro definitions--------------------------------------------------*/

/* Global variable------------------------------------------------------------*/

/* Cache array of the motion data collected by the gyroscope acquisition results */
static float Angle_X_Buff[MEAN_FILTER_NUM];
static float Angle_Y_Buff[MEAN_FILTER_NUM];
static float Angle_Z_Buff[MEAN_FILTER_NUM];
static float Gyro_X_Buff[MEAN_FILTER_NUM];
static float Gyro_Y_Buff[MEAN_FILTER_NUM];
static float Gyro_Z_Buff[MEAN_FILTER_NUM];

/* Array index */
static uint8_t DataBuf_Index = 0;

/* count variable */



/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/

/** 
* @description: Get the median filter the motion data value
* @param  {uint16_t*}  p_angle_x : Data after filtering
* @param  {uint16_t*}  p_angle_y : Data after filtering
* @param  {uint16_t*}  p_angle_z : Data after filtering
* @param  {uint16_t*}  p_gyro_x  : Data after filtering
* @param  {uint16_t*}  p_gyro_y  : Data after filtering
* @param  {uint16_t*}  p_gyro_z  : Data after filtering
* @return {t_FuncRet } : if success,return Operatin_Success
* @author: leeqingshui 
*/
t_FuncRet Get_MotionData_MeanFilter_Value(uint16_t* p_angle_x , 
										  uint16_t* p_angle_y ,
						                  uint16_t* p_angle_z ,
						                  uint16_t* p_gyro_x  ,
								          uint16_t* p_gyro_y  ,
						                  uint16_t* p_gyro_z )
{
	t_FuncRet ret= (t_FuncRet)Operatin_Success;
	
	/* Initializes the filter structure */
	Mean_Filter FilterStruct = {0};
	
	/* Determine whether a reception is complete */
	ret = UART1_isRxComplete();
	if(ret != (t_FuncRet)Operatin_Success)
	{
		return ret;
	}
	
	/* Index Value Judgment */
	if(DataBuf_Index > (MEAN_FILTER_NUM-1))
	{
		DataBuf_Index = 0;
	}
	else
	{
		DataBuf_Index++;
	}
	
	/* Put the collected data into the cache array */
	Angle_X_Buff[DataBuf_Index] = Get_Xaxis_Angle();
	Angle_Y_Buff[DataBuf_Index] = Get_Yaxis_Angle();
	Angle_Z_Buff[DataBuf_Index] = Get_Zaxis_Angle();
	
	Gyro_X_Buff[DataBuf_Index]  = Get_Xaxis_Angle_Acc();
	Gyro_Y_Buff[DataBuf_Index]  = Get_Yaxis_Angle_Acc();
	Gyro_Z_Buff[DataBuf_Index]  = Get_Zaxis_Angle_Acc();
	
	/* Mean filtering */
	*p_angle_x = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Angle_X_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	*p_angle_y = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Angle_Y_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	*p_angle_z = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Angle_Z_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	*p_gyro_x  = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Gyro_X_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	*p_gyro_y  = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Gyro_Y_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	*p_gyro_z  = Data_Mean_Filter((Mean_Filter*)&FilterStruct ,(uint16_t*)Gyro_Z_Buff);
	Mean_Filter_Rest((Mean_Filter*)&FilterStruct);
	
	/* Result back on the array */
	Angle_X_Buff[DataBuf_Index] =  *p_angle_x;
	Angle_Y_Buff[DataBuf_Index] =  *p_angle_y;
	Angle_Z_Buff[DataBuf_Index] =  *p_angle_z;
	
	Gyro_X_Buff[DataBuf_Index]  =  *p_gyro_x ;
	Gyro_Y_Buff[DataBuf_Index]  =  *p_gyro_y;
	Gyro_Z_Buff[DataBuf_Index]  =  *p_gyro_z;

	return ret;
}




