/**
  ******************************************************************************
  * File Name          : USART_Printf.h
  *
  * Description        : This header file is mainly serial port send function and 
  *						 serial port receive function and serial port print redirection
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_PRINTF_H
#define __USART_PRINTF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Serial port 2 interrupts sending associated macro definitions */

/* Send buffer capacity, adjusted as needed */
#define TX_2_BUF_LEN  256 


/* Extern Variable------------------------------------------------------------*/


/* Data structure declaration-------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* 
	Serial port 6 sends instructions to control the manipulator,
	The receiving manipulator returns a state variable
*/
t_FuncRet USART6_Printf_Polling(const char* Data, ...);
/* Send the array using serial port 6 in Polling mode */
t_FuncRet USART6_SendBuf_Polling(uint8_t* DataBuf , uint8_t Length_DataBuf);
/* Enable the serial port. 6 Receive an interrupt */
t_FuncRet USART6_Start_IT(void);
/* Serial port 6 Data receive callback function */
void HAL_USART6_RxCpltCallback(void);
/* Use this function to determine whether the serial port6 reception is complete */
bool USART6_isRxCompleted(void);
/* Clear the serial port periodically. 6 Receive data to prevent a jam */
void USART6_RecvDataClear(void);

/*
	Serial port 1 receives gyroscope data correlation function
*/
/* Enable the serial port. 1 Receive an interrupt */
t_FuncRet USART1_Start_IT(void);
/* Serial port 1 Data receive callback function */
void HAL_USART1_RxCpltCallback(void);
/* Serial port 6 The receiver is cleared periodically */
t_FuncRet USART1_isRxComplete(void);

/*
	Serial port 2 used for man-machine interaction with the serial port screen(Baud Rate 115200)
*/
/* Send the array using serial port 2 in Polling mode */
t_FuncRet USART2_SendBuf_Polling(uint8_t* DataBuf , uint8_t Length_DataBuf);
/* Enable the serial port 2 Receive an interrupt */
t_FuncRet USART2_Start_IT(void);
/* Serial port 2 string sending function */
t_FuncRet USART2_Printf_IT(const char* Data, ...);


#ifdef __cplusplus
}
#endif
#endif /* __USART_PRINTF_H */

