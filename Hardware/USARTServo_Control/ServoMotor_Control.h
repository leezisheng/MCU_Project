/**
  ******************************************************************************
  * File Name          : ServoMotor_Control.h
  *
  * Description        : This folder contains macro definitions for function 
  *                      declarations and instructions that control the manipulator 
  *                      control, read the internal parameters of the manipulator, 
  *                      and so on
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERVOR_H
#define __SERVOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Common macro definitions---------------------------------------------------*/

/* Communication protocol related macro definition */
/* 
	Communication protocol format£º
	| frame header | frame header | ID number | Data length | Command | command parameter 1 ... command parameter N | checksum
	 
	1. frame header :
		Sending two 0x55 consecutively indicates data arrival 
		
	2. ID number :
		Each steering engine has an ID number, which ranges from 0 to 253, converted to hexadecimal 0x00 to 0xFD
		Broadcast ID: ID number 254 (0xFE) is the broadcast ID. After the broadcast ID is issued, all steering engines receive commands, 
		but do not return reply information
	
	3. Data length:
		Data length is equal to the Length of the data to be sent (including one byte of itself), that is, 
		the Length of the data plus 3 is equal to the Length of the packet of instructions, from the frame header to the checksum
	
	4. Command £º
		Commands that control steering gear, such as position and speed control
		
		There are two kinds of instructions, write instructions and read instructions. 
		Write command: 
			Generally followed by parameters, the corresponding function of the parameters written into the steering engine, to complete a certain action. 
			Read command: generally no parameter is taken after it is received by the steering engine. 
		After receiving the read command, the steering engine will immediately return corresponding data. 
		The returned command value is the same as the "read command" value sent to the steering engine, and has parameters.
		Therefore, the upper computer should be ready to change itself into the read state immediately after sending the read instruction
		
		The suffix "_WRITE" indicates a write instruction, and the suffix "_READ" indicates a read instruction
	
	5. command parameter £º
		Control information that needs to be supplemented in addition to instructions
		
	6. checksum £º
		Checksum = ~ (ID + Length + Cmd+ Prm1 + ...  Prm N) If the number of computations in parentheses 
		exceeds 255, the lowest byte is used, and "~" indicates the inverse byte
*/

/* frame header */ 
#define LOBOT_SERVO_FRAME_HEADER         			 0x55

/* Parameter Indicates the maximum data length */ 
#define MAX_DATA_LENGTH         			 		 12

/* Write Angle information and speed information , and the data length is 7 */
#define LOBOT_SERVO_MOVE_TIME_WRITE      			 1
#define DATA_LENGTH_LOBOT_SERVO_MOVE_TIME_WRITE  	 7

/* Read the command name SERVO_MOVE_TIME_WRITE to send the Angle and time values to the steering engine , and the data length is 3 */ 
#define LOBOT_SERVO_MOVE_TIME_READ       			 2
#define DATA_LENGTH_LOBOT_SERVO_MOVE_TIME_READ  	 3

/* 
	The difference is that after the command reaches the steering gear, the steering gear will not turn immediately. 
    The steering gear will turn only after the command name SERVO_MOVE_START with the command value of 11 is delivered to the steering gear , 
	and the data length is 7 
*/
#define LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 			 7
#define DATA_LENGTH_LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 7

/* 
	Read the command name SERVO_MOVE_TIME_WAIT_WRITE the preset Angle and preset time interval sent to the steering engine ,
	and the data length is 3
*/
#define LOBOT_SERVO_MOVE_TIME_WAIT_READ  			 8
#define DATA_LENGTH_LOBOT_SERVO_MOVE_TIME_WAIT_READ  3

/* Used with the SERVO_MOVE_TIME_WAIT_WRITE command , and the data length is 3*/
#define LOBOT_SERVO_MOVE_START           			 11
#define DATA_LENGTH_LOBOT_SERVO_MOVE_START           3

/* 
	This command reaches the steering gear, and if the steering gear is turning, it stops immediately , 
	and the data length is 3
*/
#define LOBOT_SERVO_MOVE_STOP            			 12
#define DATA_LENGTH_LOBOT_SERVO_MOVE_STOP            3

/*
	This command will re-write the ID value to the steering engine, and save the power failure
	and the data length is 4
*/
#define LOBOT_SERVO_ID_WRITE             			 13
#define DATA_LENGTH_LOBOT_SERVO_ID_WRITE             4

/* Read the servo ID value , and the data length is 3 */
#define LOBOT_SERVO_ID_READ              			 14
#define DATA_LENGTH_LOBOT_SERVO_ID_READ              3

/*
	The command reaches the steering gear, which will turn immediately for deviation adjustment ,
	The deviation value adjusted by this command does not support power failure saving ,
	and the data length is 4
*/
#define LOBOT_SERVO_ANGLE_OFFSET_ADJUST  			 17
#define DATA_LENGTH_LOBOT_SERVO_ANGLE_OFFSET_ADJUST  4

/* Save deviation value , and support power failure save , and the data length is 3*/
#define LOBOT_SERVO_ANGLE_OFFSET_WRITE   			 18
#define DATA_LENGTH_LOBOT_SERVO_ANGLE_OFFSET_WRITE   3

/* Read the deviation value set by the steering gear */
#define LOBOT_SERVO_ANGLE_OFFSET_READ    			 19
#define DATA_LENGTH_LOBOT_SERVO_ANGLE_OFFSET_READ    3

/* 
	The command is sent to the steering gear, whose rotation Angle will be limited between the minimum and maximum rotation. 
	And the Angle limit value can be saved after power failure , 
	and the data length is 7
*/
#define LOBOT_SERVO_ANGLE_LIMIT_WRITE    			 20
#define DATA_LENGTH_LOBOT_SERVO_ANGLE_LIMIT_WRITE    7

/* Read the limit value of steering gear Angle , and the data length is 3 */
#define LOBOT_SERVO_ANGLE_LIMIT_READ     			 21
#define DATA_LENGTH_LOBOT_SERVO_ANGLE_LIMIT_READ     3

/*
	The command is sent to the steering gear, whose input voltage will be limited between the minimum and maximum. 
	Out of the range of the steering gear LED light will flash alarm, (if the LED alarm is set) in order to protect the steering gear, 
	its motor will be in the unloading power off state, at this time, the steering gear will not output torque, 
	and the input voltage limit value supports power loss saving ,
	and the data length is 7
*/
#define LOBOT_SERVO_VIN_LIMIT_WRITE      			 22
#define DATA_LENGTH_LOBOT_SERVO_VIN_LIMIT_WRITE      7

/* Read the limit value of the input voltage of the steering gear , and the data length is 3 */
#define LOBOT_SERVO_VIN_LIMIT_READ       			 23
#define DATA_LENGTH_LOBOT_SERVO_VIN_LIMIT_READ       3

/* 
	Maximum internal temperature limit of the steering gear, range of 50~100¡æ, default value 85¡æ, if the internal temperature of the 
	steering gear exceeds this value, the LED light of the steering gear will flash alarm, (if the LED alarm is set) in order to protect 
	the steering gear, the motor in the steering gear will be unloaded power, at this time the steering gear will not output torque, 
	Until the temperature is lower than this value, the steering engine will enter the working state again, 
	and this value supports power failure preservation 
	The data length is 4
*/
#define LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 			 24
#define DATA_LENGTH_LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 4

/* Read the maximum temperature limit inside the steering gear , The data length is 3 */
#define LOBOT_SERVO_TEMP_MAX_LIMIT_READ  			 25
#define DATA_LENGTH_LOBOT_SERVO_TEMP_MAX_LIMIT_READ  3

/* Read the real-time temperature inside the steering gear , and the data length is 3 */
#define LOBOT_SERVO_TEMP_READ            			 26
#define DATA_LENGTH_LOBOT_SERVO_TEMP_READ            3

/* Read the current input voltage inside the steering gear , and the data length is 3 */
#define LOBOT_SERVO_VIN_READ             			 27
#define DATA_LENGTH_LOBOT_SERVO_VIN_READ             3

/* Read the actual Angle position value of the steering gear , and the data length is 3 */
#define LOBOT_SERVO_POS_READ             			 28
#define DATA_LENGTH_LOBOT_SERVO_POS_READ             3

/*
	Set steering GEAR MODE. Range 0 or 1. 0 represents position control mode, 1 represents motor control mode, default is 0
	The data length is 7
*/
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  			 29
#define DATA_LENGTH_LOBOT_SERVO_OR_MOTOR_MODE_WRITE  7

/* Read the parameters related to steering gear mode , and the data length is 3 */
#define LOBOT_SERVO_OR_MOTOR_MODE_READ   			 30
#define DATA_LENGTH_LOBOT_SERVO_OR_MOTOR_MODE_READ   3

/* 
	Set the motor inside the steering gear is offloaded and power is off. Range 0 or 1,0 means offloaded and power is off. 
	At this time, the steering gear has no torque output. 1 represents the loading motor, at this time the steering engine has torque output, 
	the default value is 0.
	The data length is 4
*/
#define LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 			 31
#define DATA_LENGTH_LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 4

/* Read the state of the motor inside the steering gear , and the data length is 3 */
#define LOBOT_SERVO_LOAD_OR_UNLOAD_READ  			 32
#define DATA_LENGTH_LOBOT_SERVO_LOAD_OR_UNLOAD_READ  3

/* 
	Set the LED on and off state. The range is 0 or 1. 0 indicates that the LED is steady on. 
	1 indicates that the LED is always off. The default value is 0 
	The data length is 4
*/
#define LOBOT_SERVO_LED_CTRL_WRITE       			 33
#define DATA_LENGTH_LOBOT_SERVO_LED_CTRL_WRITE       4

/* Read the LED light status , and the data length is 3 */
#define LOBOT_SERVO_LED_CTRL_READ        			 34
#define DATA_LENGTH_LOBOT_SERVO_LED_CTRL_READ        3

/* 
	Set the value of LED flashing alarm caused by steering engine failure, ranging from 0 to 7
	The data length is 4
*/
#define LOBOT_SERVO_LED_ERROR_WRITE      			 35
#define DATA_LENGTH_LOBOT_SERVO_LED_ERROR_WRITE      4

/* Read the steering gear fault alarm value , and the data length is 3 */
#define LOBOT_SERVO_LED_ERROR_READ       			 36
#define DATA_LENGTH_LOBOT_SERVO_LED_ERROR_READ       3

/* Common macro function -----------------------------------------------------*/

/* The macro gets the lower octet of A */
#define GET_LOW_BYTE(DATA) 					((uint8_t)(DATA))
/* The macro gets the higher eight digits of A */
#define GET_HIGH_BYTE(DATA) 				((uint8_t)((DATA) >> 8))
/* The macro function synthesizes the high eight bits into 16 bits */
#define BYTE_TO_HW(DATA_A , DATA_B) 		((((uint16_t)(DATA_A)) << 8) | (uint8_t)(DATA_B))
/* Macro function that determines whether it is a command */
#define IS_SERVOMOTOR_COMMAND(PERIPH)   	((PERIPH >= 0x01)&&(PERIPH <= 0x36))
/* Determine whether to serve the motor ID number macro function */
#define IS_SERVOMOTOR_ID_NUMBER(PERIPH)     ((PERIPH >= 0x00)&&(PERIPH <= 0x253))
/* Macro function: Determine the length of data */
#define IS_DATA_LENGTH(PERIPH)				((PERIPH >= 0x03)&&(PERIPH <= 0x07))


/* Data structure declaration-------------------------------------------------*/

/* 
	Servo serial port data transmission frame structure 
	Flexible arrays are used to construct data packets of indefinite length
*/
typedef struct
{
	/* frame header */
	uint8_t HeaderFrame_1;
	uint8_t HeaderFrame_2;
	
	/* Servo motor ID number*/
	uint8_t Servor_ID;
	
	/* Data frame length */
	uint8_t DataLength;
	
	/* Command number */
	uint8_t Command;
	
	/* 
		To use flexible arrays, select C99 Mode in the C/C ++ TAB of Keil5 and fill in -- C99 (or -- GNU) in Misc Control in the C/C ++ TAB of KeIL5. 
		In would delete the Misc TAB control of data, 
		can consult blog https://blog.csdn.net/gpf19930310/article/details/85048400
	*/
	/* 
		Because of the difficulty of implementation, I'm going to use a fixed-length array instead of a variable-length array, which, 
		in fact, I should have used
	*/
	uint8_t Command_Parameter[MAX_DATA_LENGTH];
	
	/* Data frame checksum */
	uint8_t Checksum;

}SendDataFrame;


/* Extern Variable------------------------------------------------------------*/

/* Extern variable: The serial port receives the data array */
extern SendDataFrame RxDataStruct;
/* Data receive structure pointer */
extern SendDataFrame* p_RxDataStruct;

/* Function declaration-------------------------------------------------------*/

/* Servo motor write ID number */
t_FuncRet ServoMotor_SetID(uint8_t oldID, uint8_t newID);
/* Serve the motor at once */
t_FuncRet ServoMotor_Move_Immediately(uint8_t id, int16_t position, uint16_t time);
/* Set the motor inside the steering gear is offloaded and power is off */
t_FuncRet ServoMotor_Unload(uint8_t id);
/* Set the motor inside the steering gear is loaded and power is on */
t_FuncRet ServoMotor_Load(uint8_t id);
/* Read current steering gear Angle */
t_FuncRet ServoMotor_Read_Position(uint8_t id ,int32_t* p_angle);
/* Read the steering gear return parameters */
t_FuncRet ServoMotor_Read_Ret(int32_t* p_ret);
/* Steering gear control test: Control rotation of No. 0 to 6 steering gear */
t_FuncRet ServoMotor_Control_Test(void);


#ifdef __cplusplus
}
#endif
#endif /* __SERVOR_H */

