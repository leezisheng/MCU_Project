/**
  ******************************************************************************
  * File Name          : HMI_Control.h
  *
  * Description        : Including the serial port panel control function declaration 
  *                      and related data structure declaration
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HMI_CONTROL_H
#define __HMI_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"



/* Common macro definitions---------------------------------------------------*/




/* Extern Variable------------------------------------------------------------*/




/* Data structure declaration-------------------------------------------------*/

/* component ID enumeration */
typedef enum
{
    Num_ID_0    = 0,
    Num_ID_1    = 1,
    Num_ID_2    = 2,
    Num_ID_3    = 3,

    Num_ID_4    = 4,
    Num_ID_5    = 5,
    Num_ID_6    = 6,
    Num_ID_7    = 7,

    Num_ID_8    = 8,
    Num_ID_9    = 9,
    Num_ID_10   = 10,
    Num_ID_11   = 11,

    Num_ID_12   = 12,
    Num_ID_13   = 13,
    Num_ID_14   = 14,
    Num_ID_15   = 15,
    
    Num_ID_16   = 16,
    Num_ID_17   = 17,
    Num_ID_18   = 18,
    Num_ID_19   = 19,

    Num_ID_20   = 20,
    Num_ID_21   = 21,
    Num_ID_22   = 22,
    Num_ID_23   = 23,

    Num_ID_24   = 24,
    Num_ID_25   = 25,
    Num_ID_26   = 26,
    Num_ID_27   = 27,

    Num_ID_28   = 28,
    Num_ID_29   = 29,
    Num_ID_30   = 30,
    Num_ID_31   = 31,
    
    Num_ID_32   = 32,
    Num_ID_33   = 33,
    Num_ID_34   = 34,
    Num_ID_35   = 35,
}ComponentID;

/* Digital component structure declaration */
typedef struct 
{
    ComponentID Num_Component_ID;
    int32_t Num_Component_Val;
}HMI_Control_Num_Component;

/* Curve component structure declaration */
typedef struct 
{
    ComponentID Curve_Component_ID;
    int32_t Curve_Component_Channel;
    int32_t Curve_Component_Val;
}HMI_Control_Curve_Component;

/* Function declaration-------------------------------------------------------*/

/* Example Modify the value of the corresponding ID digital component */
t_FuncRet HMI_Control_Set_NumComponent_Value(HMI_Control_Num_Component* p_HMI_Control_Num_Component);
/* Change the value of the corresponding channel in the corresponding ID curve component */
t_FuncRet HMI_Control_Set_CurveComponent_Value(HMI_Control_Curve_Component* p_HMI_Control_Curve_Component);

#ifdef __cplusplus
}
#endif
#endif /* __HMI_CONTROL_H */
