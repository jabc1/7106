#ifndef _a7106_h
#define _a7106_h
//#include "base.h"
#include "stm32f1xx_hal.h"
#define Sint8   signed   char
#define Uint8   uint8_t
#define Uint16  uint16_t
#define Uint32  uint32_t  

#define NO                  0
#define YES                 1

#define LOW                 0
#define HIGH                1

#define ON                  1
#define OFF                 0

#define ENABLE              1
#define DISABLE             0

#define MASTER				1
#define SLAVE				0

#define ONEWAY				0
#define TWOWAY				1



void A7106_Reset(void);
void A7106_WriteReg(Uint8, Uint8);
Uint8 A7106_ReadReg(Uint8);
void ByteSend(Uint8 src);
Uint8 ByteRead(void);
void A7106_WriteID(void);
void A7106_WriteFIFO(void);
void initRF(void);
void A7106_Config(void);
void A7106_Cal(void);
void A7106_RCO_Cal(void);
void RxPacket(void);
void StrobeCmd(Uint8);
void SetCH(Uint8);
void rf_writeid(const uint8_t* id_buf);
void rf_setfifolen(uint8_t len);
uint8_t rf_writefifo(const uint8_t* src, uint8_t len);
void test11(void);
#endif




