#include <stm32f10x_lib.h>                  /* stm32f10x definitions */
#include "type.h"
#include "STM32_Reg.h"    // missing bit definitions
#include "IO.h"

//#define IR_SYS_CODE  0x61
//#define IR_USER_CODE 0xA0
//http://www.sbprojects.com/knowledge/ir/nec.php

#define SetIrPin() SetPortBit(PORTC,0)
#define ClrIrPin() ClrPortBit(PORTC,0)


//STATUS
#define IR_STOP  	0
#define IR_START 	1
#define IR_HEAD 	2
#define IR_BIT  	3
#define IR_TAIL 	4
#define IR_BIT_L  5
#define IR_BIT_H  6

//ir send
U8	IrStatus = IR_STOP;
U16	IrTimeCnt=0;
U8	BitCnt;
U32	BitBuf;

#define TIMER_8BIT 	0
#define TIMER_16BIT 1

//#define USE_TIMER_BITS   TIMER_8BIT
#define USE_TIMER_BITS   TIMER_16BIT


//waveform  level width
#if (USE_TIMER_BITS == TIMER_16BIT)
  #define C_IR_HEAD_L  	9000//9ms
	#define C_IR_HEAD_H  	4500//4.5ms
	#define C_IR_BIT0_L  	560  //0.56ms
	#define C_IR_BIT0_H  	565  //0.565ms
	#define C_IR_BIT1_L  	565  //0.565ms
	#define C_IR_BIT1_H  	1685  //1.685ms
	#define C_IR_TAIL   	560  //0.56ms
	static U16 NecSimulator(void)
	{
		U16 ReVal;
		//U16 TimeHigh,TimeLow;
		//Send remote code 
		if(IrTimeCnt < 0xff) IrTimeCnt++;
		switch( IrStatus )
		{
			case IR_START:
				BitCnt = 0;
				ClrIrPin();
				IrStatus = IR_HEAD;
			  ReVal=C_IR_HEAD_L;
				break;					
			case IR_HEAD:
				SetIrPin();
			  IrStatus = IR_BIT_L;
			  ReVal=C_IR_HEAD_H;
				break;
      case IR_BIT_L:
				ClrIrPin();
			  IrStatus = IR_BIT_H;
			  if(BitBuf&0x80000000)
				{
						ReVal = C_IR_BIT1_L;
				}
				else
				{
						ReVal = C_IR_BIT0_L;
				}
        break;		
      case IR_BIT_H:
				SetIrPin();
			  if(BitBuf&0x80000000)
				{
						ReVal = C_IR_BIT1_H;
				}
				else
				{
						ReVal = C_IR_BIT0_H;
				}
				if(BitCnt < 32)
				{
						BitCnt++;
						BitBuf = BitBuf<<1;
					  IrStatus = IR_BIT_L;
				}
				else
					IrStatus = IR_TAIL;
        break;	
      case IR_TAIL:
				ClrIrPin();
			  IrStatus = IR_STOP;
			  ReVal=C_IR_TAIL;
        break;				
			case IR_STOP:
					 SetIrPin();
			  ReVal=255;
				break;
	 }
	 return ReVal;
	}
#else
	#define C_IR_HEAD_L  	37 //(37x256)us
	#define C_IR_HEAD_H  	15 //(15x256)us
	#define C_IR_BIT0_L  	2  //(2x256)us
	#define C_IR_BIT0_H  	2  //(2x256)us
	#define C_IR_BIT1_L  	2  //(2x256)us
	#define C_IR_BIT1_H  	6  //(6x256)us
	#define C_IR_TAIL   	2  //(2x256)us
	
	static U16 NecSimulator(void)
	{
		U16 TimeHigh,TimeLow;
		//Send remote code 
		if(IrTimeCnt < 0xff) IrTimeCnt++;
		switch( IrStatus )
		{
			case IR_START:
				ClrIrPin();
				IrTimeCnt = 0;
				IrStatus = IR_HEAD;
				break;					
			case IR_HEAD:
				if(IrTimeCnt == C_IR_HEAD_L)
						SetIrPin();
				else if(IrTimeCnt == C_IR_HEAD_H+C_IR_HEAD_L)
				{
					ClrIrPin();
					IrTimeCnt = 0;
					BitCnt = 0;
					IrStatus = IR_BIT;
				}
				break;				
			case IR_BIT:
				if(BitCnt < 32)
				{
					if(BitBuf&0x80000000)
					{
						TimeLow = C_IR_BIT1_L;
						TimeHigh = C_IR_BIT1_H;
					}
					else
					{
						TimeLow = C_IR_BIT0_L;
						TimeHigh = C_IR_BIT0_H;
					}
					if(IrTimeCnt == TimeLow)
							SetIrPin();
					else if(IrTimeCnt == TimeLow+TimeHigh)
					{
						ClrIrPin();
						IrTimeCnt = 0;
						BitCnt++;
						BitBuf = BitBuf<<1;
					}
				}
				else
				{
						IrStatus = IR_TAIL;
				}
				break;					
			case IR_TAIL:
				if(IrTimeCnt == C_IR_TAIL)
				{
					SetIrPin();
					IrStatus = IR_STOP;
					IrTimeCnt = 0;
				}
				break;
			case IR_STOP:
					 SetIrPin();
				break;
		}
		return 255;
	}
#endif




U8 IRcodeTransmit(U8 SysCode,U8 UserCode,U8 Key)
{
   U16 tm;
   
   if(IrStatus != IR_STOP) //busy 
   	return FALSE;
   
   tm = SysCode;
   tm = tm << 8;
   tm = tm + UserCode;
   BitBuf = tm;
   BitBuf = BitBuf << 16;
   tm = Key;
   tm = tm << 8;
   tm = tm + (0xff - Key);
   BitBuf = BitBuf + tm;
   SetIrPin();
   IrStatus = IR_START;
   return TRUE;
}

void InitIrSimulator(void)
{
	  IrStatus = IR_STOP;
	  //----------------------------------------------
	  InitPortBit(PORTC,0,OUTPUT_PUSH_PULL,1);
	  //----------------------------------------------
	  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                     // enable clock for TIM1

    TIM1->PSC = 71;// set prescaler  72MHz/(71+1)=1MHz
    TIM1->ARR = 255;// set auto-reload  (255+1)=256us
    TIM1->RCR = 0; // set repetition counter

    TIM1->CR1 = 0; // reset command register 1
    TIM1->CR2 = 0; // reset command register 2
	  
	  TIM1->DIER = TIM_IT_Update;                           // enable interrupt
    NVIC->ISER[0]  = (1 << (TIM1_UP_IRQChannel & 0x1F));  // enable interrupt
	  TIM1->CR1 |= TIMX_CR1_CEN;                              // enable timer
}


void TIM1_UP_IRQHandler(void)  
{                
 if(TIM1->SR&0X0001) 
 {  
   TIM1->ARR = NecSimulator();
 }      
  TIM1->SR&=~(1<<0);        
}  
