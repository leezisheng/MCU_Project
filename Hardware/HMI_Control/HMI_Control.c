/**
  ******************************************************************************
  * File Name          : HMI_Control.c
  * Description        : Including the definition of serial port panel control 
  *                      function and related data structure definition
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "USART_Printf.h"
#include "HMI_Control.h"
#include <stdio.h>


/* External function declaration----------------------------------------------*/



/* Private macro definitions--------------------------------------------------*/

/* Serial port send macro definition */
#define HMI_Write_Command USART2_Printf_IT


/* Global variable------------------------------------------------------------*/


/* Static function definition-------------------------------------------------*/



/* Function definition--------------------------------------------------------*/

/* 
* @description : Example Modify the value of the corresponding ID digital component
* @param  {HMI_Control_Num_Component*} p_HMI_Control_Num_Component : A pointer to a component structure
* @return {uint8_t} : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet HMI_Control_Set_NumComponent_Value(HMI_Control_Num_Component* p_HMI_Control_Num_Component)
{
    t_FuncRet ret = (t_FuncRet)Operation_Success;

    int32_t ID  = p_HMI_Control_Num_Component->Num_Component_ID;
    int32_t Val = p_HMI_Control_Num_Component->Num_Component_Val;

    ret = HMI_Write_Command("n%d.val=%d\xff\xff\xff",ID,Val);
    HAL_Delay(1);
    
    #ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif

    return ret;
}

/* 
* @description : Change the value of the corresponding channel in the corresponding ID curve component
* @param  {HMI_Control_Curve_Component*} p_HMI_Control_Curve_Component : A pointer to a component structure
* @return {uint8_t} : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet HMI_Control_Set_CurveComponent_Value(HMI_Control_Curve_Component* p_HMI_Control_Curve_Component)
{
    t_FuncRet ret = (t_FuncRet)Operation_Success;

    int32_t ID  = p_HMI_Control_Curve_Component->Curve_Component_ID;
    int32_t Val = p_HMI_Control_Curve_Component->Curve_Component_Val;
    int32_t Channel = p_HMI_Control_Curve_Component->Curve_Component_Channel;

    ret = HMI_Write_Command("add %d,%d,%d\xff\xff\xff",ID,Channel,Val);
    HAL_Delay(1);

    #ifdef USE_FULL_ASSERT
		assert_param(ret != Operation_Fail);
	#endif

    return ret;
}
