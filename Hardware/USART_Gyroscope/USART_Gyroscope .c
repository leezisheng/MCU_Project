/**
  ******************************************************************************
  * File Name          : USART_Gyroscope .c
  * Description        : This file includes the declaration and macro definition 
  *                      of the serial gyroscope read and write operation functions
  ******************************************************************************
 */
  
/* Includes ------------------------------------------------------------------*/
#include "USART_Gyroscope.h"
#include "USART_Printf.h"
#include "usart.h"
#include <string.h>

/* External function declaration----------------------------------------------*/


/* Private macro definitions--------------------------------------------------*/


/* Global variable------------------------------------------------------------*/

/* ++++++++++++++++++++++++++++++Command string++++++++++++++++++++++++++++++ */

/* The z-axis Angle goes to zero */
const uint8_t YAWCMD[3]     	= {0XFF,0XAA,0X52};
/* Calibration of acceleration */
const uint8_t ACCCMD[3] 		= {0XFF,0XAA,0X67};
/* Toggles sleep mode and working mode */
const uint8_t SLEEPCMD[3] 		= {0XFF,0XAA,0X60};
/* Set the serial port output mode of the gyroscope */
const uint8_t UARTMODECMD[3] 	= {0XFF,0XAA,0X61};
/* Set the gyro IIC output mode */
const uint8_t IICMODECMD[3] 	= {0XFF,0XAA,0X62};

/* Data cache array */
static unsigned char ucRxBuffer[250];
/* Receive data count variable */
static unsigned char ucRxCnt = 0;

/* Gyroscope data structure */
static Struct_Acc 		S_Acc;
static Struct_Gyro 	    S_AngleAcc;
static Struct_Angle 	S_Angle;

/* Gyroscope data */
volatile static float angle_x;
volatile static float angle_y;
volatile static float angle_z;

volatile static float acc_x;
volatile static float acc_y;
volatile static float acc_z;

volatile static float gyro_x;
volatile static float gyro_y;
volatile static float gyro_z;

/* Static function definition-------------------------------------------------*/

/* Function definition--------------------------------------------------------*/

/** 
* @description: The data returned by gyroscope is analyzed
* @param  {unsigned char}  ucData  :  data that Serial port 1 interrupts receiving to be parsed
* @return {t_FuncRet}      ret     :  Operation_Fail    - Data frame received error, receive again
*                                     Operation_Success - Complete a data reception
*                                     Operation_Wait   - This data reception is still in progress
* @author: leeqingshui 
*/
t_FuncRet CopeSerial2Data(unsigned char ucData)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	// Store the received data into a buffer
	ucRxBuffer[ucRxCnt++]=ucData;
	
	// If the data header is incorrect, the search for the 0x55 data header starts again
	if (ucRxBuffer[0]!=0x55) 
	{
		ucRxCnt=0;
		ret = (t_FuncRet)Operation_Fail;
		return ret;
	}
	
	// If the number of data is less than 11, return
	if (ucRxCnt<11) 
	{
		ret = (t_FuncRet)Operation_Wait;
		return ret;
	}
	else
	{
		/* 
			Determine what kind of data the data is and then copy it to the corresponding structure. 
			Some data packets can only be received after the corresponding output is opened by the host computer
		*/
		switch(ucRxBuffer[1])
		{
			/*
				Memcpy is a memory copy function that comes with the compiler. 
				It needs to refer to "string.h" to copy the characters of the receive buffer into the data structure, 
				so as to realize the data parsing
			*/
			case 0x51:	memcpy(&S_Acc,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&S_AngleAcc,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&S_Angle,&ucRxBuffer[2],8);break;

		}
		
		/* Data parsing is complete. Clear the cache */
		ucRxCnt=0;
		
		ret = (t_FuncRet)Operation_Success;
	}
	
	return (t_FuncRet)ret ;
}


/** 
* @description: Send instructions to the gyroscope
* @param  {uint8_t *[3]} data :  An order array to be sent 
* @return {{t_FuncRet} : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Send_Command(const uint8_t data[3])
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	static uint8_t tx_buff;
	
	for(int i=0;i<3;i++)
	{
		tx_buff = data[i];
		
		// Data transmission failure
		if((HAL_UART_Transmit(&huart1,&tx_buff,1,0Xff)) != HAL_OK)
		{
			ret = Operation_Fail;
		}
	}

	return (t_FuncRet)ret ;
}

/** 
* @description: Gyroscope calibration
* @param  {void} 
* @return {{t_FuncRet} : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet Gyroscope_Calibration(void)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* Calibration of acceleration */
	ret = Send_Command(ACCCMD);
	
	if(ret == Operation_Fail)
	{
		return (t_FuncRet)ret;
	}
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
	
	/* Wait until the internal calibration of the module is good. The internal calculation of the module will take some time */
	HAL_Delay(100);
	
	/* Z axis Angle calibration */
	ret = (Send_Command(YAWCMD)) & ret;
	if(ret == Operation_Fail)
	{
		return (t_FuncRet)ret;
	}
	
	#ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif
	
	/* Wait until the internal calibration of the module is good. The internal calculation of the module will take some time */
	HAL_Delay(10);

	return (t_FuncRet)ret;
}



/** 
* @description: Return the X-axis acceleration
* @param  {void} 
* @return {{float} : the X-axis acceleration
* @author: leeqingshui 
*/
float Get_Xaxis_Acc(void)
{
	acc_x = (float)S_Acc.Acc_DataBuf[0]/32768*16;
	return acc_x;
}

/** 
* @description: Return the Y-axis acceleration
* @param  {void} 
* @return {{float} : the Y-axis acceleration
* @author: leeqingshui 
*/
float Get_Yaxis_Acc(void)
{
	acc_y = (float)S_Acc.Acc_DataBuf[1]/32768*16;
	return acc_y;
}

/** 
* @description: Return the Z-axis acceleration
* @param  {void} 
* @return {{float} : the Z-axis acceleration
* @author: leeqingshui 
*/
float Get_Zaxis_Acc(void)
{
	acc_z = (float)S_Acc.Acc_DataBuf[2]/32768*16;
	return acc_z;
}

/** 
* @description: Return the X-axis angel
* @param  {void} 
* @return {{float} : the X-axis angel
* @author: leeqingshui 
*/
float Get_Xaxis_Angle(void)
{
	angle_x = (float)S_Angle.Angle_DataBuf[0]/32768*180;
	return angle_x;
}

/** 
* @description: Return the Y-axis angel
* @param  {void} 
* @return {{float} : the Y-axis angel
* @author: leeqingshui 
*/
float Get_Yaxis_Angle(void)
{
	angle_y = (float)S_Angle.Angle_DataBuf[1]/32768*180;
	return angle_y;
}

/** 
* @description: Return the Z-axis angel
* @param  {void} 
* @return {{float} : the Z-axis angel
* @author: leeqingshui 
*/
float Get_Zaxis_Angle(void)
{
	angle_z = (float)S_Angle.Angle_DataBuf[2]/32768*180;
	return angle_z;
}

/** 
* @description: Return the X-axis Angle_Acc
* @param  {void} 
* @return {{float} : the X-axis Angle_Acc
* @author: leeqingshui 
*/
float Get_Xaxis_Angle_Acc(void)
{
	gyro_x = (float)S_AngleAcc.Angle_Acc_DataBuf[0]/32768*2000;
	return gyro_x;
}

/** 
* @description: Return the Y-axis Angle_Acc
* @param  {void} 
* @return {{float} : the Y-axis Angle_Acc
* @author: leeqingshui 
*/
float Get_Yaxis_Angle_Acc(void)
{
	gyro_y = (float)S_AngleAcc.Angle_Acc_DataBuf[1]/32768*2000;
	return gyro_y;
}

/** 
* @description: Return the Z-axis Angle_Acc
* @param  {void} 
* @return {{float} : the Z-axis Angle_Acc
* @author: leeqingshui 
*/
float Get_Zaxis_Angle_Acc(void)
{
	gyro_z = (float)S_AngleAcc.Angle_Acc_DataBuf[2]/32768*2000;
	return gyro_z;
}
