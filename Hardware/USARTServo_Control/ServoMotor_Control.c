/**
  ******************************************************************************
  * File Name          : ServoMotor_Control.c
  *
  * Description        : This folder includes functions to control the manipulator 
  *                      control and read the internal parameters of the manipulator	
  *                      
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "USART_Printf.h"
#include "ServoMotor_Control.h"
#include "usart.h"

#include "tim.h"
/* External function declaration----------------------------------------------*/

/* extern function : Serial port sending function */
extern t_FuncRet USART6_SendBuf(uint8_t* DataBuf , uint8_t Length_DataBuf);
/* Check whether the serial port receives the information */
extern bool isRxCompleted(void);

/* Private macro definitions--------------------------------------------------*/

/* Serial port send macro definition */
#define ServoMotorWrite  USART6_SendBuf

/* Global variable------------------------------------------------------------*/

/* Timer 3 handle pointer */
extern TIM_HandleTypeDef htim3;

/* Global variable: The serial port receives the data struct */
SendDataFrame RxDataStruct= {0};
/* Data receive structure pointer */
SendDataFrame* p_RxDataStruct = &RxDataStruct;

/* Static function definition-------------------------------------------------*/

/* The structure array conversion function is used to evaluate the checksum */
static t_FuncRet CmdStruct_To_Array_Checksum(SendDataFrame* p_CmdStruct , uint8_t* p_DataBuf);
/* Split the instruction structure into arrays to facilitate subsequent sending */
static t_FuncRet CmdStruct_To_Array(SendDataFrame* p_CmdStruct , uint8_t* p_DataBuf ,uint8_t Length_CmdStruct);
/* Computes an instruction data checksum */
static uint8_t   Get_CheckSum(uint8_t* p_buf);

/* Function definition--------------------------------------------------------*/

/** 
* @description: Servo motor write ID number
*				The ID ranges from 0 to 253. The value is converted to hexadecimal 0x00 to 0xFD
* 				Broadcast ID: ID number 254(0xFE) is the broadcast ID. 
*				If the ID number issued by the controller is 254(0xFE), all steering engines receive commands, 
*				but do not return a response message, (except reading the steering engine ID number, 
*				see the instructions below for details) to prevent bus conflicts.
* @param  {uint8_t} oldID : ID number of the service motor to be operated 
* @param  {uint8_t} newID : The ID number of the servo motor to be written
* @return {t_FuncRet}     : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_SetID(uint8_t oldID, uint8_t newID)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* create command sends the data frame structure */ 
	SendDataFrame CmdInfoStruct;
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6] = {0};
	
	/* Data frame head */
	CmdInfoStruct.HeaderFrame_1        = LOBOT_SERVO_FRAME_HEADER;
	CmdInfoStruct.HeaderFrame_2        = LOBOT_SERVO_FRAME_HEADER;
	/* ID number of the service motor to be operated */
	CmdInfoStruct.Servor_ID            = oldID;
	/* command Data length*/
	CmdInfoStruct.DataLength           = DATA_LENGTH_LOBOT_SERVO_ID_WRITE;
	/* command number */
	CmdInfoStruct.Command       	   = LOBOT_SERVO_ID_WRITE;
	/* command parameter */
	CmdInfoStruct.Command_Parameter[0] = newID;
	/* Structure is converted to an array */
	CmdStruct_To_Array((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf , sizeof(CmdInfoStruct));
	/* checksum */
	CmdInfoStruct.Checksum             = Get_CheckSum((uint8_t*)&DataBuf);

	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf);
    
	/* Send instructions */
	ret = ServoMotorWrite((uint8_t*)&DataBuf , sizeof(DataBuf));
	
	return ret;
}


/** 
* @description: Send to the service motor to turn to the Angle and rotation time (that is, to control the rotation speed), 
*				the instruction takes effect immediately, 
*				the steering engine will turn to the set Angle at a constant speed within the set time
* @param  {uint8_t}   id       : ID of the service motor to be operated
* @param  {int16_t}   position : The set steering gear rotation Angle ranges from 0 to 1000, 
*								 the corresponding Angle ranges from 0 to 240 degrees, 
*								 that is, the steering gear can continuously change the Angle of 0.24 degrees
*								 The higher eight digits are valid
* @param  {uint16_t}  time     : The time range is 0 to 30,000 milliseconds
*								 The higher eight digits are valid
* @return {t_FuncRet}          : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_Move_Immediately(uint8_t id, int16_t position, uint16_t time)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* create command sends the data frame structure */ 
	SendDataFrame CmdInfoStruct;
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6];
	
	/* Data frame head */
	CmdInfoStruct.HeaderFrame_1        = LOBOT_SERVO_FRAME_HEADER;
	CmdInfoStruct.HeaderFrame_2        = LOBOT_SERVO_FRAME_HEADER;
	/* ID number of the service motor to be operated */
	CmdInfoStruct.Servor_ID            = id;
	/* command Data length*/
	CmdInfoStruct.DataLength           = DATA_LENGTH_LOBOT_SERVO_MOVE_TIME_WRITE;
	/* command number */
	CmdInfoStruct.Command       	   = LOBOT_SERVO_MOVE_TIME_WRITE;
	/* command parameter */
	CmdInfoStruct.Command_Parameter[0] = GET_LOW_BYTE(position);
	CmdInfoStruct.Command_Parameter[1] = GET_HIGH_BYTE(position);
	CmdInfoStruct.Command_Parameter[2] = GET_LOW_BYTE(time);
	CmdInfoStruct.Command_Parameter[3] = GET_HIGH_BYTE(time);
	/* checksum */
	CmdStruct_To_Array((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf , sizeof(CmdInfoStruct));
	CmdInfoStruct.Checksum             = Get_CheckSum((uint8_t*)&DataBuf);
	
	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf);
    
	/* Send instructions */
	ret = ServoMotorWrite((uint8_t*)&DataBuf , sizeof(DataBuf));

	return ret;
}


/** 
* @description: Set the motor inside the steering gear is offloaded and power is off.
*				Range 0 or 1,0 means offloaded and power is off.
*				At this time, the steering gear has no torque output. 1 represents the loading motor, at this time the steering engine has torque output, 
*				the default value is 0.
* @param  {uint8_t}   id  : ID of the service motor to be operated
* @return {t_FuncRet}     : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_Unload(uint8_t id)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* create command sends the data frame structure */ 
	SendDataFrame CmdInfoStruct;
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6];
	
	/* Data frame head */
	CmdInfoStruct.HeaderFrame_1        = LOBOT_SERVO_FRAME_HEADER;
	CmdInfoStruct.HeaderFrame_2        = LOBOT_SERVO_FRAME_HEADER;
	/* ID number of the service motor to be operated */
	CmdInfoStruct.Servor_ID            = id;
	/* command Data length*/
	CmdInfoStruct.DataLength           = DATA_LENGTH_LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE;
	/* command number */
	CmdInfoStruct.Command       	   = LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE;
	/* command parameter */
	CmdInfoStruct.Command_Parameter[0] = 0;
	/* checksum */
	CmdStruct_To_Array((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf , sizeof(CmdInfoStruct));
	CmdInfoStruct.Checksum             = Get_CheckSum((uint8_t*)&DataBuf);
	
	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf);
    
	/* Send instructions */
	ret = ServoMotorWrite((uint8_t*)&DataBuf , sizeof(DataBuf));

	return ret;
}


/** 
* @description: Set the motor inside the steering gear is loaded and power is on
*				Range 0 or 1,0 means offloaded and power is off.
*				At this time, the steering gear has no torque output. 1 represents the loading motor, at this time the steering engine has torque output, 
*				the default value is 0.
* @param  {uint8_t}   id  : ID of the service motor to be operated
* @return {t_FuncRet}     : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_Load(uint8_t id)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* create command sends the data frame structure */ 
	SendDataFrame CmdInfoStruct;
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6];
	
	/* Data frame head */
	CmdInfoStruct.HeaderFrame_1        = LOBOT_SERVO_FRAME_HEADER;
	CmdInfoStruct.HeaderFrame_2        = LOBOT_SERVO_FRAME_HEADER;
	/* ID number of the service motor to be operated */
	CmdInfoStruct.Servor_ID            = id;
	/* command Data length*/
	CmdInfoStruct.DataLength           = DATA_LENGTH_LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE;
	/* command number */
	CmdInfoStruct.Command       	   = LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE;
	/* command parameter */
	CmdInfoStruct.Command_Parameter[0] = 1;
	/* checksum */
	CmdStruct_To_Array((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf , sizeof(CmdInfoStruct));
	CmdInfoStruct.Checksum             = Get_CheckSum((uint8_t*)&DataBuf);
	
	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf);
    
	/* Send instructions */
	ret = ServoMotorWrite((uint8_t*)&DataBuf , sizeof(DataBuf));

	return ret;
}


/** 
* @description: Read the current actual Angle position value of the steering gear
*               return data format of Serve motor is :
*					Parameter 1: the lower eight digits of the current Angle position value of the steering gear.
*					Parameter 2: eight digits higher than the current Angle position value of the steering gear, no default value.
* @param  {uint8_t}  id       : ID of the service motor to be operated
* @param  {int32_t*} p_angle  : Pointer parameter, Angle value to be read
* @return {t_FuncRet}         : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_Read_Position(uint8_t id , int32_t* p_angle)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* create command sends the data frame structure */ 
	SendDataFrame CmdInfoStruct;
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6];
	
	/* Data frame head */
	CmdInfoStruct.HeaderFrame_1        = LOBOT_SERVO_FRAME_HEADER;
	CmdInfoStruct.HeaderFrame_2        = LOBOT_SERVO_FRAME_HEADER;
	/* ID number of the service motor to be operated */
	CmdInfoStruct.Servor_ID            = id;
	/* command Data length*/
	CmdInfoStruct.DataLength           = DATA_LENGTH_LOBOT_SERVO_POS_READ;
	/* command number */
	CmdInfoStruct.Command       	   = LOBOT_SERVO_POS_READ;
	/* command parameter */

	/* checksum */
	CmdStruct_To_Array((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf , sizeof(CmdInfoStruct));
	CmdInfoStruct.Checksum             = Get_CheckSum((uint8_t*)&DataBuf);
	
	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&CmdInfoStruct , (uint8_t*)&DataBuf);
    
	/* Send instructions */
	ret = ServoMotorWrite((uint8_t*)&DataBuf , sizeof(DataBuf));
	
	/* Reading the return value */	
	ServoMotor_Read_Ret(p_angle);
	
	return ret;
}


/** 
* @description: Read the current actual Angle position value of the steering gear
*               return data format of Serve motor is :
*					Parameter 1: the lower eight digits of the current Angle position value of the steering gear.
*					Parameter 2: eight digits higher than the current Angle position value of the steering gear, no default value.
* @param  {int32_t*} p_ret : Pointer parameter, return value to be read
* @return {t_FuncRet}      : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
t_FuncRet ServoMotor_Read_Ret(int32_t* p_ret)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	/* Structure is converted to an array */
	uint8_t DataBuf[MAX_DATA_LENGTH + 6];
	/* Delay counting variable */
	int count = 500;
	/* command parameter */
	uint8_t cmd;
	static uint8_t checksum = 0;
	
	while(!isRxCompleted())
	{
		count--;
		/* Data reception timed out and a failure flag bit was returned */
		if(count < 0)
			return (t_FuncRet)Operation_Wait;
	}
	
	/* Structure is converted to an array */
	CmdStruct_To_Array_Checksum((SendDataFrame*)&RxDataStruct , (uint8_t*)&DataBuf);
	
	/* Data is successfully received , Data parsing begins */
	/* 
		Communication protocol format£º
		| frame header | frame header | ID number | Data length | Command | command parameter 1 ... command parameter N | checksum
	*/
	
	/* Computes the data frame checksum */
	checksum = Get_CheckSum((uint8_t*)&DataBuf);
	if(((uint8_t)checksum) != (uint8_t)RxDataStruct.Checksum)
	{
		/* The checksum does not match. The failure flag bit is returned */
		return (t_FuncRet)Operation_Fail;
	}
	
	cmd = RxDataStruct.Command;
	
	/* Parsing command parameters */ 
	switch(cmd)
	{
		case LOBOT_SERVO_POS_READ:
			/* Take out return parameters */
			/* 
			Command name SERVO_POS_READ Command value 28 Data length 5
				Parameter 1: the lower eight digits of the current Angle position value of the steering gear.
				Parameter 2: eight digits higher than the current Angle position value of the steering gear, no default value.
			Note: The Angle position value returned should be converted to a signed short int, because the Angle read may be negative
			*/
			*p_ret = (int32_t)BYTE_TO_HW(RxDataStruct.Command_Parameter[1] ,RxDataStruct.Command_Parameter[0]);
			ret = (t_FuncRet)Operation_Success;
			UART6_Reset();
			return ret;
		default:
			break;
	}

	return ret;
}


/** 
* @description: Split the instruction structure into arrays to facilitate subsequent sending
* @param  {SendDataFrame} CmdStruct : the command structure to be split
* @param  {uint8_t*} 	  DataBuf   : array to send
* @param  {uint8_t } 	  DataBuf   : length of array to send
* @return {t_FuncRet}               : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
static t_FuncRet CmdStruct_To_Array(SendDataFrame* p_CmdStruct , uint8_t* p_DataBuf ,uint8_t Length_CmdStruct)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	
	// A pointer enforces a cast
	uint8_t* p_StructElem = (uint8_t*)p_CmdStruct;
	
	for(int ptr_offset = 0; ptr_offset < Length_CmdStruct; ptr_offset++)
	{
		p_DataBuf[ptr_offset] = (*(p_StructElem+ptr_offset));
	}
	
	return ret;
}


/** 
* @description: Split the instruction structure into arrays to facilitate subsequent sending
* @param  {SendDataFrame} CmdStruct : the command structure to be split
* @param  {uint8_t*} 	  DataBuf   : array to send
* @param  {uint8_t } 	  DataBuf   : length of array to send
* @return {t_FuncRet}               : if success , return (t_FuncRet)Operation_Success
* @author: leeqingshui 
*/
static t_FuncRet CmdStruct_To_Array_Checksum(SendDataFrame* p_CmdStruct , uint8_t* p_DataBuf)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	int ptr_offset = 0;

	// A pointer enforces a cast
	uint8_t* p_StructElem = (uint8_t*)p_CmdStruct;

	for(ptr_offset = 0; ptr_offset < p_CmdStruct->DataLength+2; ptr_offset++)
	{
		p_DataBuf[ptr_offset] = (*(p_StructElem+ptr_offset));
	}

	p_DataBuf[ptr_offset] = p_CmdStruct->Checksum;

	/* Remove the element after cmdinfostruct. Checksum */
	for(int count = ptr_offset + 1;count<MAX_DATA_LENGTH + 6;count++)
	{
		p_DataBuf[count] = 0;
	}

	return ret;
}

/** 
* @description: Computes an instruction data checksum
* @param  {uint8_t*} DataBuf  : array to send
* @return {uint8_t}  Checksum : Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N) , 
*                               If the number of computations in parentheses exceeds 255, 
*								the lowest byte is used, and the "~" indicates the inverse byte
* @author: leeqingshui 
*/
static uint8_t Get_CheckSum(uint8_t* p_buf)
{
	uint16_t checksum   = 0;
	uint8_t  ptr_offset = 0;
	
	/* Checksum = ~ (ID + Length + Cmd+ Prm1 + ... Prm N) */
    for (ptr_offset = 2; ptr_offset < (*(p_buf+3) + 2); ptr_offset++) 
	{
		checksum = checksum + (uint16_t)*(p_buf+ptr_offset);
    }
	
	/* 
		If the number of computations in parentheses exceeds 255, the lowest byte is used, and the "~" indicates the inverse byte
	*/
	return (uint8_t)~checksum;
}

/* Steering gear control test: Control rotation of No. 0 to 6 steering gear */
t_FuncRet ServoMotor_Control_Init(void)
{
	t_FuncRet ret = (t_FuncRet)Operation_Success;
	uint8_t temp_id = 0;
	
	for(temp_id = 0;temp_id<=6;temp_id++)
	{
		ServoMotor_Move_Immediately(temp_id, 0, 300);
		HAL_Delay(50);
	}
	
	/* 
		Timer 3 is interrupted periodically(10Hz), 
		and the receive clearance function of serial port 6 is invoked 
	*/
	HAL_Delay(50);
	/* Clear the IT flag bit */
	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE ); 
	
	/* Enable timer 3 Interrupt */
	if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	{
		ret = Operation_Fail;
		return (t_FuncRet)ret;
	}
	
	return (t_FuncRet)ret;
}


