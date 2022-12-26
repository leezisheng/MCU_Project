/**
  ******************************************************************************
  * File Name          : USART_Printf.c
  *
  * Description        : This file contains basic operations on the USART2¡¢USART6¡¢
  *					     including initialization, reading data, returning data, and so on
  *
  *                      USART6  ------> Used to control the serving motor (Baud Rate 115200)
  *                      USART1  ------> Used to Send and receive gyroscope data(Baud Rate 9600)
  *                      USART2  ------> Used for man-machine interaction with the serial port screen(Baud Rate 115200)
  *
  * Pin               : 
  *					     PA9     ------> USART1_TX
  *					 	 PA10    ------> USART1_RX
  *
  *						 PA2     ------> USART2_TX
  *						 PD6     ------> USART2_RX
  *						 
  *					     PC6     ------> USART6_TX
  *						 PC7     ------> USART6_RX
  *
  * 					In this file, SWO is used instead of UART to achieve Printf printing function
  *	Data reference:
  *                     https://www.cnblogs.com/wanban/p/11789160.html
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/

#include "USART_Printf.h"
#include "ServoMotor_Control.h"
#include "usart.h"
#include "tim.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
/* External function declaration----------------------------------------------*/

/* The parameters of the gyroscope data are analyzed */
extern t_FuncRet CopeSerial2Data(unsigned char ucData);

/* Private macro definitions--------------------------------------------------*/

/* St-link ITM debugging outputs related macro definitions */
#define ITM_Port8(n) (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n) (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n) (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA 0x01000000

/* Global variable------------------------------------------------------------*/

/* Pointer to the serial port handle */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

/* Variable that determines whether the serial port reception is complete */
static bool isUartRxCompleted = FALSE;
/* Extern variable: The serial port receives the data array */
extern SendDataFrame RxDataStruct;
/* Data receive structure pointer */
extern SendDataFrame* p_RxDataStruct;
/* Serial port 6 Receives data */
static uint8_t USART6_Rx_Data = 0;

/* ++++++++++++Serial port 6 interrupt callback function variable++++++++++++ */
/* Flag bit: Indicates whether to start receiving data */
volatile static bool isGotFrameHeader = (bool)FALSE;
/* Data frame header counts */
static uint8_t frameHeaderCount = 0;
/* The length of the data */
static uint8_t dataLength = 2;
/* Received data count */
static uint8_t dataCount = 0;
/* Command Parameter Count */
static uint8_t command_parameter_count = 0;
/* Res receives return variables for the serial port */
static uint8_t Res = 0;

/* ++++++++++++Serial port 1 interrupt callback function variable++++++++++++ */
/* Serial port 1 Receives data */
static uint8_t USART1_Rx_Data = 0;
/* Static variable that determines whether data parsing is complete for serial port 1 */
volatile static t_FuncRet Gyroscope_Ret = Operation_Success;

/* ++++++++++++Serial port 2 interrupt callback function variable++++++++++++ */
/* Serial port 2 Receives data */
static uint8_t USART2_Rx_Data = 0;

/* Static function definition-------------------------------------------------*/

/* Convert an integer to a string,itoa ( integer to array ) */
static char *itoa( int value, char *string, int radix );

/* Function definition--------------------------------------------------------*/

/* SWO is used instead of UART2 to realize Printf printing function by using port 0 */
struct __FILE 
{ 
	int handle; 
	/* Add whatever you need here */ 
};
FILE __stdout;
FILE __stdin;
    
int fputc(int ch, FILE *f)
{
    if (DEMCR & TRCENA)
    {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    return(ch);
}

/** 
* @description: the printf function for serial port 6 redirection
*
*               When using c language programming, usually is to determine the number of parameters, 
*               the forms of function in the call, I will, in turn,
*               all the actual parameters and the corresponding formal parameters are given, 
*               but in some cases hope function can according to the need to identify the number of parameters, 
*               such as printf, the scanf function, such as the c compiler provides a series of macro deal with this situation, 
*               These macros include Va_start, va_arg and va_end, etc.
*
*               In ANSI standard form, a function with a variable number of arguments is prototyped as:
*               type funcname(type para1,...) ;
*               The ellipsis is part of the function form. 
*               Type is the type of the function return value and the formal parameter.
*
*               Va is a variable argument.
*
*               The three macros in the standard C library are used only to determine the memory address of each argument 
*               in the variable argument list. The compiler does not know the actual number of arguments:
*               1.Set flags in fixed arguments -- the printf function is determined with the first fixed string argument;
*               2.In advance set a special end mark, that is to say, more input a variable parameter, call the last variable 
*               parameter value is set to this special value, 
*               in the function body according to the value of the end of the parameter to judge whether;
*
* @param {const char*} Data : String constant
* @param {void}        ...  : Format the sent data
* @return {t_FuncRet}  ret  : if success , return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART6_Printf_Polling(const char* Data, ...)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	const char *s;
	int d;   
    char buf[16];

	/* 
	typedef struct __va_list { void *__ap; } va_list; 
	
	This variable is a pointer to the address of the parameter. 
	The value of the parameter can only be obtained by combining the parameter type with the address of the parameter. 
	Va_list is defined as char*. Some machines also define void*
	
	*/
    va_list ap;
	
	/*
	#define va_start(ap, parmN) __va_start(ap, parmN)
	
	Va_start (va_list,type) refers to the first variable argument. 
	Va_start is defined as &v + _INTSIZEOF(v), where &v is the starting address of the last fixed argument.
	*/
    va_start(ap, Data);
	
	uint8_t temp_r = 0x0d;
	uint8_t temp_n = 0x0a;

    while (*Data != 0)  // Determine if it is the end
    {                                         
        if (*Data == 0x5c)  // Determine if it is "/"
        {                                     
            switch ( *++Data ) // Determine if it is "/"
            {
                case 'r': // Check whether it is a carriage return character                                    
                if(HAL_UART_Transmit(&huart6, (uint8_t *)&temp_r, (uint16_t)1 , 1000)!=HAL_OK)
				{
					ret = (t_FuncRet)Operation_Fail;
					return (t_FuncRet)ret ;
				}
                Data ++;
                break;

                case 'n':   // Check whether it is a carriage return character                              
                if(HAL_UART_Transmit(&huart6, (uint8_t *)&temp_n, (uint16_t)1 , 1000)!=HAL_OK)
				{
					ret = (t_FuncRet)Operation_Fail;
					return (t_FuncRet)ret ;
				}
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }

        else if ( * Data == '%')  // Determines whether it is a format symbol
        {                                     
            switch ( *++Data )
            {               
                case 's':  // Check whether it is a character type   
                
                /* 
				#define va_arg(ap, type) __va_arg(ap, type)
				
				Va_arg (va_list, type) is used to obtain the value of the specified parameter type and 
				make va_list point to the starting address of a parameter, equivalent to a stack operation
				
				This macro does two things:
				(1) Use the type name entered by the user to cast the parameter address to obtain the value required by the user
				(2) Calculate the actual size of this parameter, and move the pointer to the end of this parameter, that is, 
				the first address of the next parameter, for subsequent processing.
				
				*/				
                s = va_arg(ap, const char *);
				
                for ( ; *s; s++) 
                {
					if(HAL_UART_Transmit (&huart6, (uint8_t *)s, (uint16_t)1 , 1000)!=HAL_OK)
					{
						ret = (t_FuncRet)Operation_Fail;
						return (t_FuncRet)ret ;
					}
                }
                Data++;
                break;

                case 'd':  // Checks whether it is a decimal symbol         
                    
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
                    if(HAL_UART_Transmit (&huart6, (uint8_t *)s, (uint16_t)1 , 1000)!=HAL_OK)
					{
						ret = (t_FuncRet)Operation_Fail;
						return (t_FuncRet)ret ;
					}
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else 
        {
			if(HAL_UART_Transmit (&huart6, (uint8_t *)Data++, (uint16_t)1 , 1000)!=HAL_OK)
			{
				ret = (t_FuncRet)Operation_Fail;
				return (t_FuncRet)ret ;
			}
		}
    }
	/* Clears the argument list. The collimated argument pointer arg_ptr is invalid. */
	va_end (ap);
	return (t_FuncRet)ret ;
}


/** 
* @description: Convert an integer to a string,itoa ( integer to array )
*               Converts an int integer to a string and stores the value in an array string
* @param {int}     value   :  integer
* @param {char *}  string  :  An array of characters stored after conversion
* @param {int}     radix   :  Convert base numbers, such as 2,8,10,16, etc
* @return {char}:  Pointer to a character array
* @author: leeqingshui 
*/
static char *itoa( int value, char *string, int radix )
{
    int32_t i,d;
    int32_t flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


/** 
* @description: Send an indefinite array using a serial port in Polling mode
* @param  {uint8_t*} DataBuf        : array to send
* @param  {uint8_t*} Length_DataBuf : Length of  array to print
* @return {t_FuncRet}               : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART6_SendBuf_Polling(uint8_t* DataBuf , uint8_t Length_DataBuf)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	if(HAL_UART_Transmit(&huart6, (uint8_t *)DataBuf, (uint16_t)Length_DataBuf, 100)!=HAL_OK)
	{
		ret = (t_FuncRet)Operation_Fail;
		return (t_FuncRet)ret ;
	}
	
	return (t_FuncRet)ret ;
}

/** 
* @description: Enable the serial port. 6 Receive an interrupt
* @param  {void} 
* @return {t_FuncRet} : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART6_Start_IT(void)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* Only one character can be received per interrupt */
	if(HAL_UART_Receive_IT(&huart6, (uint8_t *)&USART6_Rx_Data, 1) != HAL_OK)
	{
		ret = (t_FuncRet)Operation_Fail;
		return (t_FuncRet)ret ;
	}
	
	return (t_FuncRet)ret ;
}


/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1)
	{
		HAL_USART1_RxCpltCallback();
	}
	
	if(UartHandle->Instance == USART2)
	{

	}
	
	if(UartHandle->Instance == USART6)
	{
		HAL_USART6_RxCpltCallback();
	}	
}

/**
* @brief  Serial port 6 Data receive callback function
*/
void HAL_USART6_RxCpltCallback(void)
{
	/* 
		Only one character can be received per interrupt 
		The variable Res is the received character
	*/
	
	/* Res receives return variables for the serial port */ 
	Res = USART6_Rx_Data;
	
	/* Determine whether data has arrived: according to the frame header flag bit */
	if(!isGotFrameHeader)
	{
		if(Res == 0x55)
		{
			frameHeaderCount++;
			
			/* Two frame headers arrive */
			if (frameHeaderCount == 2) 
			{
				/* The count variable is cleared to zero */
				frameHeaderCount = 0;
				isGotFrameHeader = (bool)TRUE;
				dataCount = 1;
				
				RxDataStruct.HeaderFrame_1 = 0x55;
				RxDataStruct.HeaderFrame_2 = 0x55;
			}
		}
		else
		{
				isGotFrameHeader = (bool)FALSE;
				dataCount = 0;
				frameHeaderCount = 0;
				
				RxDataStruct.HeaderFrame_1 = 0;
				RxDataStruct.HeaderFrame_2 = 0;
		}
	}
	
	
	/* After receiving two frames, start data parsing */
	/* 
		Communication protocol format£º
		| frame header | frame header | ID number | Data length | Command | command parameter 1 ... command parameter N | checksum
	*/
	if(isGotFrameHeader)
	{
		
		/* DataCount is 2 and receives the ID number of the motors that are served */
		if(dataCount == 2)
		{
			RxDataStruct.Servor_ID = Res;
			
			/* the ID number of the motors error , skip */
			if(!IS_SERVOMOTOR_ID_NUMBER(RxDataStruct.Servor_ID))
			{	
				RxDataStruct.Servor_ID = 0;
				isGotFrameHeader = (bool)FALSE;
			}
		}
		
		/* Skip the second data, that is, wait on the motor ID number, read the data length bits */
		if(dataCount == 3)
		{
			RxDataStruct.DataLength = Res;
			dataLength = RxDataStruct.DataLength;
			
			/* the data length bits error , skip */
			if (!IS_DATA_LENGTH(dataLength)) 
			{
				dataLength = 2;
				RxDataStruct.DataLength = 0;
				isGotFrameHeader = (bool)FALSE;
			}
		}
		
		/* DataCount is 4 and receives command */
		if(dataCount == 4)
		{
			RxDataStruct.Command = Res;
			
			/* the command error , skip */
			if(!IS_SERVOMOTOR_COMMAND(RxDataStruct.Command))
			{
				RxDataStruct.Command = 0;
				isGotFrameHeader = (bool)FALSE;
			}
		}
		
		/* Parsing command parameters */
		if( (command_parameter_count < (dataLength-3)) && (dataCount > 4) )
		{
			RxDataStruct.Command_Parameter[command_parameter_count] = Res;
			command_parameter_count = command_parameter_count + 1;
			
			/* All command parameters are received successfully */
			if(command_parameter_count == dataLength-3)
			{
				goto GOTO;
			}
		}
		
		/* Command parameters are parsed, and the checksum is read */
		if((command_parameter_count == (dataLength-3))&&(command_parameter_count != 0))
		{
			RxDataStruct.Checksum = Res;
		
		}		
		
		/* Data reception completed */
		if(dataCount == (dataLength+2))
		{
			if (isUartRxCompleted == (bool)FALSE)
			{
				isUartRxCompleted = (bool)TRUE;
				
				dataCount = 0;
				isGotFrameHeader = (bool)FALSE;
				frameHeaderCount = 0;
				dataLength = 2;
				command_parameter_count = 0;
			}
			isGotFrameHeader = (bool)FALSE;
		}
		
	}
	/* The data bit count variable is incremented by one */
	GOTO:dataCount++;
	
	/* Implement multiple data returns */
	HAL_UART_Receive_IT(&huart6, (uint8_t *)&USART6_Rx_Data, 1);
}

/** 
* @description: Use this function to determine whether the serial port 6 reception is complete
* @param  {void} 
* @return {bool When the serial port 6 data is received, set the flag bit, the function returns TRUE; Otherwise return FALSE     
* @author: leeqingshui 
*/
bool USART6_isRxCompleted(void)
{
	if(isUartRxCompleted == (bool)TRUE)
	{
		isUartRxCompleted = (bool)FALSE;
		return (bool)TRUE;
	}
	else
	{
		return (bool)FALSE;
	}
}

/** 
* @description: Enable the serial port. 1 Receive an interrupt
* @param  {void} 
* @return {t_FuncRet} : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART1_Start_IT(void)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* Only one character can be received per interrupt */
	if(HAL_UART_Receive_IT(&huart1, (uint8_t *)&USART1_Rx_Data, 1) != HAL_OK)
	{
		ret = (t_FuncRet)Operation_Fail;
		return (t_FuncRet)ret ;
	}
	
	return (t_FuncRet)ret ;
}

/**
* @brief  Serial port 1 Data receive callback function
*/
void HAL_USART1_RxCpltCallback(void)
{
	Gyroscope_Ret = CopeSerial2Data(USART1_Rx_Data);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&USART1_Rx_Data, 1);
}

/**
* @brief  Serial port receive interrupt error callback function
*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	if(huart->ErrorCode&HAL_UART_ERROR_ORE)
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
		HAL_UART_Receive_IT(&huart1,&USART1_Rx_Data,1);
	}
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_ErrorCallback can be implemented in the user file.
   */
}

/* Serial port 6 The receiver is cleared periodically */
void USART6_RecvDataClear(void)
{
	/* Flag bit: Indicates whether to start receiving data */
	isGotFrameHeader = (bool)FALSE;
	/* Data frame header counts */
	frameHeaderCount = 0;
	/* The length of the data */
	dataLength = 2;
	/* Received data count */
	dataCount = 0;
	/* Command Parameter Count */
	command_parameter_count = 0;
	/* Res receives return variables for the serial port */
	Res = 0;
	
	RxDataStruct.HeaderFrame_1 = 0;
	RxDataStruct.HeaderFrame_2 = 0;
}

/* Serial port 6 The receiver is cleared periodically */
t_FuncRet USART1_isRxComplete(void)
{
	return Gyroscope_Ret;
}

/** 
* @description: Send an indefinite array using a serial port in Polling mode
* @param  {uint8_t*} DataBuf        : array to send
* @param  {uint8_t*} Length_DataBuf : Length of  array to print
* @return {t_FuncRet}               : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART2_SendBuf_Polling(uint8_t* DataBuf , uint8_t Length_DataBuf)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	if(HAL_UART_Transmit(&huart2, (uint8_t *)DataBuf, (uint16_t)Length_DataBuf, 100)!=HAL_OK)
	{
		ret = (t_FuncRet)Operation_Fail;
		return (t_FuncRet)ret ;
	}
	
	return (t_FuncRet)ret ;
}

/** 
* @description: Enable the serial port 2 Receive an interrupt
* @param  {void} 
* @return {t_FuncRet} : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet USART2_Start_IT(void)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* Only one character can be received per interrupt */
	if(HAL_UART_Receive_IT(&huart2, (uint8_t *)&USART2_Rx_Data, 1) != HAL_OK)
	{
		ret = (t_FuncRet)Operation_Fail;
		return (t_FuncRet)ret ;
	}
	
	return (t_FuncRet)ret ;
}

