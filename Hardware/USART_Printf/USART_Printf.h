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

/* Extern Variable------------------------------------------------------------*/

/* Data structure declaration-------------------------------------------------*/

/* Function declaration-------------------------------------------------------*/

/* 
	Serial port 6 emulates the printf function
	Serial port redirection: Serial port 2 only!! 
*/
t_FuncRet USART6_printf(const char* Data, ...);
/* Send the array using serial port 6 */
t_FuncRet USART6_SendBuf(uint8_t* DataBuf , uint8_t Length_DataBuf);
/* Enable the serial port. 6 Receive an interrupt */
t_FuncRet USART6_Start_IT(void);
/* Serial port 6 Data receive callback function */
void HAL_UART6_RxCpltCallback(void);
/* Use this function to determine whether the serial port reception is complete */
bool isRxCompleted(void);
/* Enable the serial port. 1 Receive an interrupt */
t_FuncRet USART1_Start_IT(void);
/* Serial port 1 Data receive callback function */
void HAL_UART1_RxCpltCallback(void);

#ifdef __cplusplus
}
#endif
#endif /* __USART_PRINTF_H */

