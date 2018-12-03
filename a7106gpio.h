#ifndef _a7106gpio_h
#define _a7106gpio_h
#include "gpio.h"
/*
SCS---->PB4
SCK---->PB5
SDIO--->(MOSI<---PB6/PB7-->MISO)
CKO---->PD5
GIO1--->PD4
GIO2--->PF0
//MODE--->PC1
 
*/
#define     CSCPROT          GPIOD
#define     CSCPROT_PIN      GPIO_PIN_15
#define     SCKPROT          GPIOD
#define     SCKPROT_PIN      GPIO_PIN_14

#if 1
#define     SDIOPROT        GPIOD
#define     SDIOPROT_PIN    GPIO_PIN_13
#else
#define     MOSIPROT        GPIOB
#define     MOSIPROT_Pin    GPIO_Pin_6
#define     MISOPROT        GPIOB
#define     MISOPROT_Pin    GPIO_Pin_7
#endif

#define     CKOPROT          GPIOD
#define     CKOPROT_PIN      GPIO_PIN_5
#define     GIO1PROT         GPIOC
#define     GIO1PROT_PIN     GPIO_PIN_9
#define     GIO2PROT         GPIOC
#define     GIO2PROT_PIN     GPIO_PIN_8

#define     LED              GPIOC
#define     LED_PIN          GPIO_PIN_0
#define     LED1              GPIOC
#define     LED1_PIN          GPIO_PIN_1
//#define     SET_GPIO_OUT(x)        GPIO_Init(x,x##_Pin, GPIO_Mode_Out_PP_Low_Fast)
//#define     SET_GPIO_IN(x)         GPIO_Init(x,x##_Pin, GPIO_Mode_In_PU_No_IT)   
//#define     SET_GPIO_H(x)          GPIO_SetBits(x,x##_Pin) 
//#define     SET_GPIO_L(x)          GPIO_ResetBits(x,x##_Pin) 
//#define     Read_GPIO(x)           GPIO_ReadInputDataBit(x,x##_Pin) 



void A7106_GPIO_init(void);

#endif





