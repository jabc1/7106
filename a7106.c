#include "a7106.h"
#include "a7106reg.h"
#include "a7106gpio.h"
#include "delay.h"
#include "gpio.h"
#include <string.h>
#define     speed       5
#if 1 
const Uint8  PN9_Tab[]=
{   
	0x41,'z','y','d',0x45,0x46,0x47,0x48,
	0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,
	0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
	0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,
	0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,
	0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,
	0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x41,
	0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49
};  // This table are 64bytes PN9 pseudo random code.
#else
const Uint8  PN9_Tab[]=
{   0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0
};  // This table are 64bytes PN9 pseudo random code.

#endif
void test11()
{
	SET_GPIO_H(SCKPROT);
	SysTick_delay_us(speed);
	SET_GPIO_L(SCKPROT);
	SysTick_delay_us(speed);

}
/*********************************************************************
** initRF
*********************************************************************/
void initRF(void)
{
    A7106_GPIO_init();
    A7106_Reset();  //reset A7106 RF chip
    A7106_WriteID();//write ID code
    A7106_WriteReg(CPC_REG, 0x77);    // RGC=3
    A7106_Config(); //config A7106 chip
    A7106_WriteReg(CPC_REG, 0x57);    // RGC=1
    A7106_Cal();    //calibration IF,vco, vcoc
    A7106_RCO_Cal(); // calibration RC Osc (OPTION)
	
	A7106_WriteFIFO(); //write data to tx fifo
	Seq = 0;
	Err_HopCnt = 0;
	SET_GPIO_IN_PP(GIO1PROT);
	SET_GPIO_OUT(LED);
	SET_GPIO_OUT(LED1);
	while(1)
	{
		SetCH(HopTab[0]);
		StrobeCmd(CMD_RX); //entry tx & transmit
		while(READ_GPIO_PIN(GIO1PROT))
		{
			SET_GPIO_H(LED1);
		}
		SET_GPIO_L(LED1);
		if(READ_GPIO_PIN(GIO1PROT))
		{
			while(1)
			{
                StrobeCmd(CMD_STBY);
                Seq++;
                Seq %= 5;
                Err_HopCnt++;
                if(Err_HopCnt > 5)
                {
                    Seq = 0;
                    Err_HopCnt = 0;
                    SetCH(HopTab[0]);
                    StrobeCmd(CMD_RX);
                    while(1)
                    {
                        if(!READ_GPIO_PIN(GIO1PROT))
                        {
                            break;
                        }
                    }
                }
			}
		}
		else
		{
			RxPacket();
		}
	}
}

void rf_writeid(const uint8_t* id_buf)
{
	int i;

	SET_GPIO_L(CSCPROT);
	ByteSend(IDCODE_REG);
	for(i = 0; i < 4; i++)
	{
		ByteSend(id_buf[i]);
	}
	SET_GPIO_H(CSCPROT);
}

void rf_setfifolen(uint8_t len)
{
	A7106_WriteReg(FIFO1_REG, len - 1);
}

uint8_t rf_writefifo(const uint8_t* src, uint8_t len)
{
	uint8_t  i;

	rf_setfifolen(len);
	ByteSend(CMD_TFR);

	SET_GPIO_L(CSCPROT);
	ByteSend(FIFO_REG);
	for(i = 0; i < len; i++)
	{
		ByteSend(src[i]);
	}
	SET_GPIO_H(CSCPROT);

	return i;
}

/************************************************************************
**  Reset_RF
************************************************************************/
void A7106_Reset(void)
{
    A7106_WriteReg(MODE_REG, 0x00); //reset RF chip
}

/************************************************************************
**  WriteID
************************************************************************/
Uint8 d1,d2,d3,d4;
void A7106_WriteID(void)
{
    Uint8 i;
    //Uint8 d1,d2,d3,d4;

    SET_GPIO_L(CSCPROT);
    ByteSend(IDCODE_REG);
    for (i=0; i < 4; i++)
        ByteSend(ID_Tab[i]);
    SET_GPIO_H(CSCPROT);

    //for check
    SET_GPIO_L(CSCPROT);
    ByteSend(IDCODE_REG | 0x40);
    d1=ByteRead();
    d2=ByteRead();
    d3=ByteRead();
    d4=ByteRead();
    SET_GPIO_H(CSCPROT); 

}

/************************************************************************
**  A7106_WriteReg
************************************************************************/
void A7106_WriteReg(Uint8 addr, Uint8 dataByte)
{
    Uint8 i;
//    SET_GPIO_OUT(SDIOPROT);
    SET_GPIO_L(CSCPROT);

    for(i=0;i<8;i++)
    {
        if(addr & 0x80)
            SET_GPIO_H(SDIOPROT);
        else
            SET_GPIO_L(SDIOPROT);
		
		SET_GPIO_H(SCKPROT);
		SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
        addr = addr << 1;
		SysTick_delay_us(speed);
    }
	//SysTick_delay_us(speed);
    //send data byte
    for(i = 0; i < 8; i++)
    {
        if(dataByte & 0x80)
            SET_GPIO_H(SDIOPROT);
        else
            SET_GPIO_L(SDIOPROT);
		SET_GPIO_H(SCKPROT);
		SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
		SysTick_delay_us(speed);
        dataByte = dataByte<< 1;		
    }

    SET_GPIO_H(CSCPROT);   
	//SET_GPIO_OUT(SDIOPROT);	
}

/************************************************************************
**  A7106_ReadReg
************************************************************************/
Uint8 A7106_ReadReg(Uint8 addr)
{
    Uint8 i;
    Uint8 tmp;
    SET_GPIO_OUT(SDIOPROT);
    SET_GPIO_L(CSCPROT);
    addr |= 0x40; //bit cmd=0,r/w=1
    for(i = 0; i < 8; i++)
    {

        if(addr & 0x80)
            SET_GPIO_H(SDIOPROT);
        else
            SET_GPIO_L(SDIOPROT);
        SET_GPIO_H(SCKPROT);
		SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
        addr = addr << 1;
		SysTick_delay_us(speed);
    }

    SET_GPIO_H(SDIOPROT);
    SET_GPIO_IN(SDIOPROT);
    
    //read data
    for(i = 0; i < 8; i++)
    {
		if(READ_GPIO_PIN(SDIOPROT))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
		
		SET_GPIO_H(SCKPROT);
		SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
		SysTick_delay_us(speed);
    }
	SET_GPIO_H(CSCPROT);

    return tmp;
}

/************************************************************************
**  ByteSend
************************************************************************/
void ByteSend(Uint8 src)
{
    Uint8 i;
	//SET_GPIO_OUT(SDIOPROT);
    for(i = 0; i < 8; i++)
    {
		
        if(src & 0x80)
            SET_GPIO_H(SDIOPROT);
        else
            SET_GPIO_L(SDIOPROT);
		
		SET_GPIO_H(SCKPROT);
        SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
		SysTick_delay_us(speed);
		
        src = src << 1;
    }

}
/************************************************************************
**  ByteRead
************************************************************************/
Uint8 ByteRead(void)
{
    Uint8 i,tmp;

    SET_GPIO_H(SDIOPROT);
    SET_GPIO_IN(SDIOPROT);

    for(i = 0; i < 8; i++)
    {
		if(READ_GPIO_PIN(SDIOPROT))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
		
		SET_GPIO_H(SCKPROT);
        SysTick_delay_us(speed);
        SET_GPIO_L(SCKPROT);
		SysTick_delay_us(speed);
    }
	
	SET_GPIO_OUT(SDIOPROT);
    return tmp;
}
/*********************************************************************
** SetCH
*********************************************************************/
void SetCH(Uint8 ch)
{
    A7106_WriteReg(PLL1_REG, ch);
}
/*********************************************************************
** A7106_WriteFIFO
*********************************************************************/
void A7106_WriteFIFO(void)
{
    Uint8 i;
	SET_GPIO_OUT(SDIOPROT);
    SET_GPIO_L(CSCPROT);
    ByteSend(FIFO_REG);
    for(i=0; i <64; i++)
        ByteSend(PN9_Tab[i]);
    SET_GPIO_H(CSCPROT);
}
/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(Uint8 cmd)
{
	//SET_GPIO_OUT(SDIOPROT);
    SET_GPIO_L(CSCPROT);
    ByteSend(cmd);
    SET_GPIO_H(CSCPROT);
}

/*********************************************************************
** RxPacket
*********************************************************************/
void RxPacket(void)
{
    Uint8 i;
    Uint8 recv;
    Uint8 tmp;

    RxCnt++;
    SET_GPIO_L(CSCPROT);
	memset(tmpbuf,0,sizeof((uint8_t *)tmpbuf));
    ByteSend(FIFO_REG | 0x40);
    for(i=0; i <64; i++)
    {
        recv = ByteRead();
        tmpbuf[i]=recv;
        if((recv ^ PN9_Tab[i])!=0)
        {
            tmp = recv ^ PN9_Tab[i];
            Err_BitCnt += (BitCount_Tab[tmp>>4] + BitCount_Tab[tmp & 0x0F]);
        }
    }
	
    SET_GPIO_H(CSCPROT);
}
/*********************************************************************
** Err_State
*********************************************************************/
void Err_State(void)
{
    //ERR display
    //Error Proc...
    //...
    while(1)
    {
        ;
    }
}
/*********************************************************************
** calibration
*********************************************************************/
void A7106_Cal(void)
{
    Uint8 tmp=0;
    Uint8 fb, vcb, vb, rh, rl;

    //==================================================
    // calibration IF, VB, VCB, RSSI
    StrobeCmd( CMD_STBY );
    A7106_WriteReg( CALIBRATION_REG, 0x0F );
    do
    {
        tmp = A7106_ReadReg( CALIBRATION_REG );
    } while ( tmp & 0x0F );

    tmp = A7106_ReadReg( IFCAL1_REG );
    if ( tmp & 0x10 )
        Err_State();
    fb = tmp & 0x0F;

    tmp = A7106_ReadReg( VCOCCAL_REG );
    if ( tmp & 0x10 )
        Err_State();
    vcb = tmp & 0x0F;

    tmp = A7106_ReadReg( VCOCAL1_REG );
    if ( tmp & 0x08 )
        Err_State();
    vb = tmp& 0x07;

    rh = A7106_ReadReg( RXGAIN2_REG );
    rl = A7106_ReadReg( RXGAIN3_REG );
}
/*********************************************************************
** A7106_RCO_Cal
*********************************************************************/
void A7106_RCO_Cal(void)
{
    Uint8 tmp, rcoc, rcot, retry, i;

    for ( rcot = 0; rcot <= 3; rcot++ )
    {
        for ( retry = 0; retry < 10; retry++ )
        {
            A7106_WriteReg( RCOSC3_REG, 0x00 );   // RCO oscillator disable
            A7106_WriteReg( RCOSC3_REG, (0x0C | (rcot<<4)) );   // RCOSC_E=1, CALW=1
            do
            {
                tmp = A7106_ReadReg( RCOSC1_REG ) & 0x80;
            } while( tmp );
            rcoc = A7106_ReadReg( RCOSC1_REG ) & 0x7F;
            if ( rcoc >= 20 )
            {
                return;
            }

            A7106_WriteReg( RCOSC3_REG, 0x00 );     // RCO oscillator disable
            A7106_WriteReg( RCOSC3_REG, (0x04 | (rcot<<4)) );   // RCOSC_E=1
            for ( i = 0; i<10*(retry+1); i++ )
            {
                A7106_WriteReg( CPC_REG, A7106Config[CPC_REG] | 0x80 );     // ROSCS=1
                A7106_WriteReg( CPC_REG, A7106Config[CPC_REG] & 0x7F );     // ROSCS=0
            }
            A7106_WriteReg( RCOSC3_REG, (0x0C | (rcot<<4)) );   // RCOSC_E=1, CALW=1

            do
            {
                tmp = A7106_ReadReg( RCOSC1_REG ) & 0x80;
            }
            while( tmp );
            rcoc = A7106_ReadReg( RCOSC1_REG ) & 0x7F;
            if ( rcoc >= 20 )
            {
                return;
            }
        }
    }

    Err_State(); // RCOC always too small
}

/*********************************************************************
** A7106_Config
*********************************************************************/
void A7106_Config(void)
{
    Uint8 i;

    //0x00 mode register, for reset
    //0x02 calibration register
    //0x05 fifo data register
    //0x06 id code register
    //0x23 IF calibration II, only read
    //0x32 filter test register

    for ( i=1; i<=0x33; i++ )
    {
        if ( (i==0x02) || (i==0x05) || (i==0x06) || (i==0x23) || (i==0x32) )
            continue;

        A7106_WriteReg( i, A7106Config[i] );
    }
}





