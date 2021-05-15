
#include <stm32f10x_lib.h>                  /* stm32f10x definitions */
#include "type.h"
#include "STM32_Reg.h"                                  // missing bit definitions
#include "I2C.h"
#include "Time.h"


#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7

void I2CSoftInit(void)
{
  /* Configure I2C1 pins: SCL and SDA */

  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;                   // enable clock for GPIOB
  //--------------
  GPIOB->CRL   &= ~(0xFFUL  << 24);                      // Clear PB6, PB7
  GPIOB->CRL   |=  (0x07UL  << 24);                      // PB6 Speed_50MHz,Mode_AF_OD
  GPIOB->CRL   |=  (0x07UL  << 28);                     // PB7 Speed_50MHz,Mode_AF_OD
  //GPIOB->CRL   &= ~(0xFFUL  << 24);                      // Clear PB6, PB7
  //GPIOB->CRL   |=  (0x03UL  << 24);                      // PB6 Speed_50MHz,Mode_AF_OD
  //GPIOB->CRL   |=  (0x03UL  << 28);                     // PB7 Speed_50MHz,Mode_AF_OD
}


#define TRY_NUM	 5

void IicBusDelay( void )
{
    U16 i=250;
	while(i)
	{
	i--;
	}
}

void ClrSDA(void)
{
  //GPIOB->CRL   &= ~(0x0FUL  << 28);                      // Clear PB7
  //GPIOB->CRL   |=  (0x03UL  << 28);                     // SDA General purpose output Push-pull, max speed 50 MHz
	SDA_L;
	IicBusDelay();
}
U8 GetSDA(void)
{
  //GPIOB->CRL   &= ~(0x0FUL  << 28);                      // Clear PB7
  //GPIOB->CRL   |=  (0x08UL  << 28);                     // SDA General purpose input, max speed 50 MHz
   return SDA_read;
}
void ClrSCL(void)
{
  //GPIOB->CRL   &= ~(0x0FUL  << 24);                      // Clear PB6
  //GPIOB->CRL   |=  (0x03UL  << 24);                     // SCL General purpose output Push-pull, max speed 50 MHz
    SCL_L; 
	IicBusDelay();
}

U8 GetSCL(void)
{
  //GPIOB->CRL   &= ~(0x0FUL  << 24);                      // Clear PB6
  //GPIOB->CRL   |=  (0x08UL  << 24);                     // SCL General purpose input, max speed 50 MHz
	return SCL_read;
}

void SetSDA(void)
{
	U8 chr1;
	SDA_H;
	//GPIOB->CRL   &= ~(0x0FUL  << 28);                      // Clear PB7
    //GPIOB->CRL   |=  (0x03UL  << 28);                     // SDA General purpose output Push-pull, max speed 50 MHz

	//if( GetSDA() == 1 ) return ;
	//for(chr1=0; chr1<255; chr1++)			// 为避免数据传送时受IIC总线上大电容（100-1000P）的影响
	//	if( GetSDA() == 1 ) break ;
	IicBusDelay();
}

void SetSCL(void)
{
    //U8 chr1;
	SCL_H;
	//  GPIOB->CRL   &= ~(0x0FUL  << 24);                      // Clear PB6
    // GPIOB->CRL   |=  (0x03UL  << 24);                     // SCL General purpose output Push-pull, max speed 50 MHz

	//if( GetSCL() == 1 ) return ;
	//for(chr1=0; chr1<255; chr1++)			// 为避免数据传送时受IIC总线上大电容（100-1000P）的影响，以及适应某些反应速度慢的IC在做出反应前拉低SCL的做法
	//	if( GetSCL() == 1 ) break ;
	IicBusDelay();
}
	


void SendACK(U8 ack)
{
  if(!ack)  //非应答
    SetSDA();            
  else    //应答
    ClrSDA();
  IicBusDelay();
  SetSCL();
  IicBusDelay();
  ClrSCL();                     
  IicBusDelay();
}

U8 IicStart(void)
{
    IicBusDelay();
	SetSDA();       
    IicBusDelay();  
	SetSCL();
    IicBusDelay();
	if( GetSDA()==0) 
	{
		return I2C_BUS_ERROR;
	}
	if( GetSCL()==0 ) 
	{
		return I2C_BUS_ERROR;
	}
	ClrSDA();           
    IicBusDelay();
	ClrSCL();         
    IicBusDelay();
	return I2C_BUS_OK;
}

U8 IicStop( void )
{
  IicBusDelay();
  ClrSDA();    
  IicBusDelay();  
  SetSCL();
  IicBusDelay();
  SetSDA();    
  IicBusDelay(); 
  return 0; 
}

U8 IicSendByte(U8 chr)
{
  U8 i;

  for(i=0;i<8;i++)  
  {
    if(chr&0x80)
      SetSDA();   
    else  
      ClrSDA();                
    SetSCL();       
    IicBusDelay();        
    ClrSCL(); 
    chr<<=1;
    IicBusDelay();
  }
  IicBusDelay();

  SetSDA();                
  IicBusDelay();
  SetSCL();
  IicBusDelay();
  if(GetSDA())
	  return I2C_BUS_ERROR;       
  ClrSCL();
  IicBusDelay();
  return I2C_BUS_OK;
}

U8 IicReceiveByte(U8 ack)
{
  U8 i,rval;

  rval=0; 
  SetSDA();
  for(i=0;i<8;i++)
  {
    IicBusDelay();          
    ClrSCL();                  
    IicBusDelay();
    SetSCL();                
    IicBusDelay();
    rval<<=1;
    if(GetSDA())
      rval++;  
  }
  ClrSCL();    
  IicBusDelay();
  SendACK(ack);
  return(rval);  
}

void IicWriteByte( U8 SlaveAddr, U8 RegAddr, U8 RegData )
{
	U8 BusTryCnt = TRY_NUM ;
	U8 f_Error;
	do{
		f_Error=I2C_BUS_OK;
		f_Error|=IicStart();
		f_Error|=IicSendByte(SlaveAddr);
		f_Error|=IicSendByte(RegAddr);
		f_Error|=IicSendByte(RegData);
		IicStop();
		if(f_Error==I2C_BUS_OK)
			return;
	}while((--BusTryCnt)&&(f_Error==I2C_BUS_ERROR));
}

U8 IicReadByte( U8 SlaveAddr, U8 RegAddr )
{
	U8 BusTryCnt = TRY_NUM ;
	U8 ucData;
	U8 f_Error;
	do{
		f_Error=I2C_BUS_OK;
		f_Error|=IicStart();
		f_Error|=IicSendByte(SlaveAddr);
		f_Error|=IicSendByte(0);
		f_Error|=IicSendByte(RegAddr);
		f_Error|=IicStart();
		f_Error|=IicSendByte(SlaveAddr+1);
		ucData=IicReceiveByte(NACK);
		IicStop();
	if(f_Error==I2C_BUS_OK)
	    return ucData;
	}while((--BusTryCnt)&&(f_Error==I2C_BUS_ERROR));
	return 0xff;
}

