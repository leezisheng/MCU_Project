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
const char YAWCMD[3] = {0XFF,0XAA,0X52};
/* Calibration of acceleration */
const char ACCCMD[3] = {0XFF,0XAA,0X67};
/* Toggles sleep mode and working mode */
const char SLEEPCMD[3] = {0XFF,0XAA,0X60};
/* Set the serial port output mode of the gyroscope */
const char UARTMODECMD[3] = {0XFF,0XAA,0X61};
/* Set the gyro IIC output mode */
const char IICMODECMD[3] = {0XFF,0XAA,0X62};

/* Data cache array */
static unsigned char ucRxBuffer[250];
/* Receive data count variable */
static unsigned char ucRxCnt = 0;

/* Gyroscope data structure */
static Struct_Acc 		S_Acc;
static Struct_Gyro 	S_AngleAcc;
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
* @return {void}
* @author: leeqingshui 
*/
void CopeSerial2Data(unsigned char ucData)
{
	// Store the received data into a buffer
	ucRxBuffer[ucRxCnt++]=ucData;
	
	// If the data header is incorrect, the search for the 0x55 data header starts again
	if (ucRxBuffer[0]!=0x55) 
	{
		ucRxCnt=0;
		return;
	}
	
	// If the number of data is less than 11, return
	if (ucRxCnt<11) 
	{
		return;
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
	}
}


/** 
* @description: Send instructions to the gyroscope
* @param  {uint8_t *[3]} data :  An order array to be sent 
* @return {{t_FuncRet} : if success , return (t_FuncRet)Operatin_Success
* @author: leeqingshui 
*/
t_FuncRet Send_Command(uint8_t data[3])
{
	t_FuncRet ret = (t_FuncRet)Operatin_Success;
	
	static uint8_t tx_buff;
	
	for(int i=0;i<3;i++)
	{
		tx_buff = data[i];
		
		// Data transmission failure
		if((HAL_UART_Transmit(&huart1,&tx_buff,1,0Xff)) != HAL_OK)
		{
			ret = Operatin_Fail;
		}
	}

	return (t_FuncRet)ret ;
}

/** 
* @description: Data conversion 
*/
void Get_TrueData(void)
{
	acc_x = (float)S_Acc.Acc_DataBuf[0]/32768*16;
	acc_y = (float)S_Acc.Acc_DataBuf[1]/32768*16;
	acc_z = (float)S_Acc.Acc_DataBuf[2]/32768*16;
		
	gyro_x = (float)S_AngleAcc.Angle_Acc_DataBuf[0]/32768*2000;
	gyro_y = (float)S_AngleAcc.Angle_Acc_DataBuf[1]/32768*2000;
	gyro_z = (float)S_AngleAcc.Angle_Acc_DataBuf[2]/32768*2000;
		
	angle_x = (float)S_Angle.Angle_DataBuf[0]/32768*180;
	angle_y = (float)S_Angle.Angle_DataBuf[1]/32768*180;
	angle_z = (float)S_Angle.Angle_DataBuf[2]/32768*180;
}
