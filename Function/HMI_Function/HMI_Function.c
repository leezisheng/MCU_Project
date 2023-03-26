/**
  ******************************************************************************
  * File Name          : HMI_Function.c
  * Description        : The following functions are defined: serial port panel 
  *                      curve update function, serial port panel data display 
  *                      function and related structure declaration
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "USART_Printf.h"
#include "HMI_Function.h"
#include "HMI_Control.h"
#include "DigtalSignal_Process.h"
#include <stdio.h>

/* External function declaration----------------------------------------------*/

/* Private macro definitions--------------------------------------------------*/

/* Macro definition for waveform display components */
#define     CH0_CURVE_ID        (ComponentID)Num_ID_1
#define     CH1_CURVE_ID        (ComponentID)Num_ID_2
#define     CH2_CURVE_ID        (ComponentID)Num_ID_3
#define     CH3_CURVE_ID        (ComponentID)Num_ID_4

#define     CH0_CHANNEL_NUM     0
#define     CH1_CHANNEL_NUM     0
#define     CH2_CHANNEL_NUM     0
#define     CH3_CHANNEL_NUM     0

/* Macro definition for waveform digital feature display components */
#define     CH_NUMCP_0         (ComponentID)Num_ID_10
#define     CH_NUMCP_1         (ComponentID)Num_ID_11
#define     CH_NUMCP_2         (ComponentID)Num_ID_12

#define     CH_NUMCP_3         (ComponentID)Num_ID_17
#define     CH_NUMCP_4         (ComponentID)Num_ID_19
#define     CH_NUMCP_5         (ComponentID)Num_ID_21

#define     CH_NUMCP_6         (ComponentID)Num_ID_23
#define     CH_NUMCP_7         (ComponentID)Num_ID_25
#define     CH_NUMCP_8         (ComponentID)Num_ID_27

#define     CH_NUMCP_9         (ComponentID)Num_ID_29
#define     CH_NUMCP_10        (ComponentID)Num_ID_31
#define     CH_NUMCP_11        (ComponentID)Num_ID_33


/* Global variable------------------------------------------------------------*/


/* Static function definition-------------------------------------------------*/



/* Function definition--------------------------------------------------------*/

/* 
* @description : Refresh the screen curve component
* @param  {uint16_t*} value_0 : Pointer to the voltage value of CH0
* @param  {uint16_t*} value_1 : Pointer to the voltage value of CH1
* @param  {uint16_t*} value_2 : Pointer to the voltage value of CH2
* @param  {uint16_t*} value_3 : Pointer to the voltage value of CH3
* @return {uint8_t} : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet HMI_Refresh_Curve_Component(uint16_t* value_0,
                                      uint16_t* value_1,
                                      uint16_t* value_2,
                                      uint16_t* value_3)
{
    t_FuncRet ret = (t_FuncRet)Operation_Success;
    
    /* Initializes the component structure variable */
    static HMI_Control_Curve_Component CurveComponent_0;
    static HMI_Control_Curve_Component CurveComponent_1;
    static HMI_Control_Curve_Component CurveComponent_2;
    static HMI_Control_Curve_Component CurveComponent_3;
    
    
    /* 
        The waveform displays data ranging from 0 to 255, while the voltage data ranges from 0 to 3300
        Therefore, data range transformation is required
    */
    int32_t temp_value_0 = DATA_RANGE_CONV(*value_0);
    int32_t temp_value_1 = DATA_RANGE_CONV(*value_1);
    int32_t temp_value_2 = DATA_RANGE_CONV(*value_2);
    int32_t temp_value_3 = DATA_RANGE_CONV(*value_3);
    
    /* Complete component structure member assignment and data sending operations */
    
    CurveComponent_0.Curve_Component_ID       =   CH0_CURVE_ID;
    CurveComponent_0.Curve_Component_Channel  =   CH0_CHANNEL_NUM;
    CurveComponent_0.Curve_Component_Val      =   temp_value_0;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&CurveComponent_0);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif
    
    CurveComponent_1.Curve_Component_ID       =   CH1_CURVE_ID;
    CurveComponent_1.Curve_Component_Channel  =   CH1_CHANNEL_NUM;
    CurveComponent_1.Curve_Component_Val      =   temp_value_1;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&CurveComponent_1);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif
    
    CurveComponent_2.Curve_Component_ID       =   CH2_CURVE_ID;
    CurveComponent_2.Curve_Component_Channel  =   CH2_CHANNEL_NUM;
    CurveComponent_2.Curve_Component_Val      =   temp_value_2;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&CurveComponent_2);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif
    
    CurveComponent_3.Curve_Component_ID       =   CH3_CURVE_ID;
    CurveComponent_3.Curve_Component_Channel  =   CH3_CHANNEL_NUM;
    CurveComponent_3.Curve_Component_Val      =   temp_value_3;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&CurveComponent_3);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    return ret;
}

/* 
* @description : Refresh the screen Number component 
* @param  {uint16_t*} value_x : x:0~11
*                               Pointer to the time domain characteristic variable of voltage
* @return {uint8_t} : if success,return Operation_Success
* @author: leeqingshui 
*/
t_FuncRet HMI_Refresh_Num_Component(uint16_t* value_0,
                                    uint16_t* value_1,
                                    uint16_t* value_2,
                                    uint16_t* value_3,
                                    uint16_t* value_4,
                                    uint16_t* value_5,
                                    uint16_t* value_6,
                                    uint16_t* value_7,
                                    uint16_t* value_8,
                                    uint16_t* value_9,
                                    uint16_t* value_10,
                                    uint16_t* value_11)
{
    t_FuncRet ret = (t_FuncRet)Operation_Success;
    
    /* Initializes the component structure variable */
    static HMI_Control_Num_Component NumberComponent_0;
    static HMI_Control_Num_Component NumberComponent_1;
    static HMI_Control_Num_Component NumberComponent_2;
    
    static HMI_Control_Num_Component NumberComponent_3;
    static HMI_Control_Num_Component NumberComponent_4;
    static HMI_Control_Num_Component NumberComponent_5;
    
    static HMI_Control_Num_Component NumberComponent_6;
    static HMI_Control_Num_Component NumberComponent_7;
    static HMI_Control_Num_Component NumberComponent_8;
    
    static HMI_Control_Num_Component NumberComponent_9;
    static HMI_Control_Num_Component NumberComponent_10;
    static HMI_Control_Num_Component NumberComponent_11;
    
    /* Complete component structure member assignment and data sending operations */

    /* Channel 0 time domain feature display operation */
    
    NumberComponent_0.Num_Component_ID  =   CH_NUMCP_0;
    NumberComponent_0.Num_Component_Val =   (int32_t)*value_0;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_0);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_1.Num_Component_ID  =   CH_NUMCP_1;
    NumberComponent_1.Num_Component_Val =   (int32_t)*value_1;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_1);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_2.Num_Component_ID  =   CH_NUMCP_2;
    NumberComponent_2.Num_Component_Val =   (int32_t)*value_2;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_2);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    /* Channel 1 time domain feature display operation */
    
    NumberComponent_3.Num_Component_ID  =   CH_NUMCP_3;
    NumberComponent_3.Num_Component_Val =   (int32_t)*value_3;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_3);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_4.Num_Component_ID  =   CH_NUMCP_4;
    NumberComponent_4.Num_Component_Val =   (int32_t)*value_4;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_4);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_5.Num_Component_ID  =   CH_NUMCP_5;
    NumberComponent_5.Num_Component_Val =   (int32_t)*value_5;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_5);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    /* Channel 2 time domain feature display operation */

    NumberComponent_6.Num_Component_ID  =   CH_NUMCP_6;
    NumberComponent_6.Num_Component_Val =   (int32_t)*value_6;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_6);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_7.Num_Component_ID  =   CH_NUMCP_7;
    NumberComponent_7.Num_Component_Val =   (int32_t)*value_7;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_7);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_8.Num_Component_ID  =   CH_NUMCP_8;
    NumberComponent_8.Num_Component_Val =   (int32_t)*value_8;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_8);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    /* Channel 3 time domain feature display operation */
    NumberComponent_9.Num_Component_ID  =   CH_NUMCP_9;
    NumberComponent_9.Num_Component_Val =   (int32_t)*value_9;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_9);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_10.Num_Component_ID  =   CH_NUMCP_10;
    NumberComponent_10.Num_Component_Val =   (int32_t)*value_10;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_10);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    NumberComponent_11.Num_Component_ID  =   CH_NUMCP_11;
    NumberComponent_11.Num_Component_Val =   (int32_t)*value_11;
    ret = HMI_Control_Set_NumComponent_Value((HMI_Control_Num_Component*)&NumberComponent_11);
    
    #ifdef USE_FULL_ASSERT
        assert_param(ret != Operation_Fail);
	#endif

    return ret;
}



