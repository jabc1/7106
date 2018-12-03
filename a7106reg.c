#include "a7106reg.h"

/*********************************************************************
**  Global Variable Declaration
*********************************************************************/
Uint8           _100us;
Uint8           timer;
Uint16          RxCnt;
Uint32          Err_ByteCnt;
Uint32          Err_BitCnt;
Uint8           tmpbuf[64]; 
Uint8           Seq;
Uint8           Err_HopCnt;
Uint8  BitCount_Tab[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
const Uint8  ID_Tab[4]={0x54,0x75,0xC5,0x2A}; //ID code

const Uint16  A7106Config[]=
{
    //      address   name              Descript
    //      -------   ----              ---------
    0x00,   //0x00  ; MODE_REG
    0x62,   //0x01  ; MODE_CONTROL_REG  FIFO mode, Enable ARSSI, Enable AIF
    0x00,   //0x02  ; CALIBRATION_REG
    0x3F,   //0x03  ; FIFO1_REG         FIFO end = 63+1
    0x40,   //0x04  ; FIFO2_REG
    0x00,   //0x05  ; FIFO_REG
    0x00,   //0x06  ; IDCODE_REG
    0x00,   //0x07  ; RCOSC1_REG
    0x00,   //0x08  ; RCOSC2_REG
    0x00,   //0x09  ; RCOSC3_REG
    0x00,   //0x0A  ; CKO_REG
    0x01,   //0x0B  ; GIO1_REG          GIO1 = WTR
    0x01,   //0x0C  ; GIO2_REG          GIO2 = WTR
    0x05,   //0x0D  ; CLOCK_REG
    0x00,   //0x0E  ; DATARATE_REG      500K bps
    0x64,   //0x0F  ; PLL1_REG
    0x9E,   //0x10  ; PLL2_REG
    0x4B,   //0x11  ; PLL3_REG
    0x00,   //0x12  ; PLL4_REG
    0x02,   //0x13  ; PLL5_REG
    0x16,   //0x14  ; TX1_REG           f(dev) = 186kHz
    0x2B,   //0x15  ; TX2_REG
    0x12,   //0x16  ; DELAY1_REG        TX, PLL setting time = 60us
    0x40,   //0x17  ; DELAY2_REG        Crystal Turn on delay = 600us
    0x62,   //0x18  ; RX_REG            BW = 500kHz, Up side band
    0x80,   //0x19  ; RXGAIN1
    0x80,   //0x1A  ; RXGAIN2
    0x00,   //0x1B  ; RXGAIN3
    0x0A,   //0x1C  ; RXGAIN4
    0x32,   //0x1D  ; RSSI_REG
    0x03,   //0x1E  ; ADC_REG
    0x0F,   //0x1F  ; CODE1_REG         ID=4bytes, Preamble=4bytes, enable CRC
    0x16,   //0x20  ; CODE2_REG
    0x00,   //0x21  ; CODE3_REG
    0x00,   //0x22  ; IFCAL1_REG
    0x00,   //0x23  ; IFCAL2_REG
    0x00,   //0x24  ; VCOCCAL_REG       VCO current auto calibration
    0x00,   //0x25  ; VCOCAL1_REG
    0x23,   //0x26  ; VCOCAL2_REG
    0x70,   //0x27  ; BATTERY_REG       Regulator = 1.8V, Enable Quick discharge
    0x17,   //0x28  ; TXTEST_REG        TX power = 0dBm
    0x47,   //0x29  ; RXDEM1_REG
    0x80,   //0x2A  ; RXDEM2_REG
    0x77,   //0x2B  ; CPC_REG           RGC=3, CPC=3, LVR=1, CELS=1
    0x01,   //0x2C  ; CRYSTALTEST_REG
    0x45,   //0x2D  ; PLLTEST_REG
    0x19,   //0x2E  ; VCOTEST1_REG
    0x00,   //0x2F  ; VCOTEST2_REG
    0x01,   //0x30  ; IFAT_REG
    0x0F,   //0x31  ; RSCALE_REG
    0x00,   //0x32  ; FILTERTEST_REG
    0x7F,   //0x33  ; TMV_REG           Ramp Up / Ramp Down enable
};

const Uint8 HopTab[5]=
{
    22, //2411M
    42, //2421M
    82, //2441
    122,//2461
    162 //2481
};
const Uint8 RfTab[4]=
{
    20,//2410M
    40,//2420M
    60,//2430M
    80,//2440M
};




