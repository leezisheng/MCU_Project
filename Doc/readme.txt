/*======================================================Project description================================================================*/

Brief :






/*=============================================================code layers=================================================================*/

1. Folder organization :
    (1) Haedware folder - Direct operation of peripheral and kernel
    (2) Function folder - encapsulates peripheral and kernel operations with adjustable parameters, including peripheral Function files
    (3) Task folder - Functions and variables associated with RTOS tasks
    (4) Common folder - Files Common to the project, such as data structures
    (5) Doc folder -- Documentation

2. code layer:

/*=====================================================GPIO pin use with peripherals=======================================================*/

1. Clock source input and output :
    (1) PC14-OSC32_IN    RCC_OSC32_IN
    (2) PC15-OSC32_OUT   RCC_OSC32_OUT
    (3) PH0 - OSC_IN     RCC_OSC_IN
    (4) PH1 - OSC_OUT    RCC_OSC_OUT

2. LED:
    (1) PD12-LD4 [Green Led]
    (2) PD13-LD3 [Orange Led]
    (3) PD14-LD5 [Red Led]
    (4) PD15-LD6 [Blue Led]

3. ADC1:
    (1) PA1-ADC1_IN1
    (2) PA3-ADC1_IN3
    (3) PA5-ADC1_IN5
    (4) PA6-ADC1_IN6
    (5) Vref

4. USART:
    (1) USART6 : PC6-USART6_TX  PC7-USART6_RX
    (2) USART1 : PA9-USART1_TX  PA10-USART1_RX
    (3) USART2 : PA2-USART2_TX  PD6-USART2_RX

5. SWD:
    (1) PA13-SYS_JTMS-SWDIO
    (2) PA14-SYS_JTCK-SWCLK

6. GPIO:
    (1) PB0-RUNTIME_TEST_GPIO

/*===============================================================Key points================================================================*/

1. Why display "unknown signals" on the Display of Filling Signals in Logic Analyzer during MDK Debugging?
    WARNING :The M4-core microcontroller does not support the logic analyzer of MDK
    
2. Note that ADC should have a calibration function before turning on the ADC, but the STM32-F4 chip does not support ADC calibration

3. STM32F4 ADC Problem 1: only one conversion in ADC-DMA scan mode
    Note the problem of only one conversion in ADC scan mode, which is a common problem of STM32F4 series. For solutions, please refer to:
    https://www.cnblogs.com/SpaceVision/p/14967055.html
    
4. STM32F4 ADC Probelm 2: ADC measurement is inaccurate and the jump value is large
    Note that in this case, because the VDDA value is unknown, the ADC channel sample value must be calibrated using an internal reference voltage,
    please refer to:
    If VDDA is connected to VDD, the fluctuation of power supply will greatly affect the accuracy of ADC
    https://blog.csdn.net/weixin_43128823/article/details/82803927







