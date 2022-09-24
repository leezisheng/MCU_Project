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

/* Private macro definitions--------------------------------------------------*/

/* Global variable------------------------------------------------------------*/

/* Cache array of ADC voltage acquisition results */
static float Angle_X_Buff[MEAN_FILTER_NUM];
static float Angle_Y_Buff[MEAN_FILTER_NUM];
static float Angle_Z_Buff[MEAN_FILTER_NUM];
static float Gyro_X_Buff[MEAN_FILTER_NUM];
static float Gyro_Y_Buff[MEAN_FILTER_NUM];
static float Gyro_Z_Buff[MEAN_FILTER_NUM];
/* Array index */
static uint8_t DataBuf_Index = 0;

/* Static function definition-------------------------------------------------*/


/* Function definition--------------------------------------------------------*/









