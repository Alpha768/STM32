#include "Include.h"

#define __PWM_CH1__ 0x01
#define __PWM_CH2__ 0x02
#define __PWM_CH3__ 0x04
#define __PWM_CH4__ 0x08

#define __GROUP_INDEX1__ 0x10
#define __GROUP_INDEX2__ 0x20
#define __GROUP_INDEX3__ 0x40
#define __GROUP_INDEX4__ 0x80

typedef struct
{
  U8 index;
	U32 freq;//Hz
	U8  duty;//0~100
	BOOLEAN first;
} PwmPara_TypeDef;

static PwmPara_TypeDef PwmPara[]=
{
	{PWM_CH01,0,0,1},
	{PWM_CH02,0,0,1},
	{PWM_CH03,0,0,1},
	{PWM_CH04,0,0,1},
	{PWM_CH05,0,0,1},
	{PWM_CH06,0,0,1},
	{PWM_CH07,0,0,1},
	{PWM_CH08,0,0,1},
	{PWM_CH09,0,0,1},
	{PWM_CH10,0,0,1},
	{PWM_CH11,0,0,1},
	{PWM_CH12,0,0,1},
	{SOFT_PWM_CH01,0,0,1},
	{SOFT_PWM_CH02,0,0,1},
	{SOFT_PWM_CH03,0,0,1},
	{SOFT_PWM_CH04,0,0,1},
	{SOFT_PWM_CH05,0,0,1},
};

static void SetPwmFrequency(U8 index,U32 Hz)
{
	U16 u16Val;
	U8 i,duty;
	TIM_TypeDef * TIMx;
    U16 psc=0;
	if(Hz>=2000)
	psc=0;
	else
	psc=144-1; 
	if(index & __GROUP_INDEX2__)
		TIMx=TIM2;
	else if(index & __GROUP_INDEX3__)
		TIMx=TIM3;
	else if(index & __GROUP_INDEX4__)
		TIMx=TIM4;
	else
		return;
	TIMx->PSC=psc;          //预分频器分频
   	TIMx->ARR=(U16)((U32)72000000/(psc+1)/Hz);          //设定计数器自动重装值 
  for(i=0;i<COUNTOF(PwmPara);i++)	
	{
		if(PwmPara[i].index==index)
		{
			PwmPara[i].freq=Hz;
			duty=PwmPara[i].duty;
			break;
		}
	}
	
	if(i==COUNTOF(PwmPara))
		  return;
	
	u16Val=TIMx->ARR;
	u16Val=(u16Val*duty)/100;
	
	if(index & __PWM_CH1__)
		TIMx->CCR1=u16Val;
	if(index & __PWM_CH2__)
		TIMx->CCR2=u16Val;
	if(index & __PWM_CH3__)
		TIMx->CCR3=u16Val;
	if(index & __PWM_CH4__)
		TIMx->CCR4=u16Val;
}
/*
static void SetPwmValid(U8 index,U16 u16Val)
{
	TIM_TypeDef * TIMx;
	if(index & __GROUP_INDEX2__)
		TIMx=TIM2;
	else if(index & __GROUP_INDEX3__)
		TIMx=TIM3;
	else if(index & __GROUP_INDEX4__)
		TIMx=TIM4;
	else
		return;
	if(index & __PWM_CH1__)
		TIMx->CCR1=u16Val;
	if(index & __PWM_CH2__)
		TIMx->CCR2=u16Val;
	if(index & __PWM_CH3__)
		TIMx->CCR3=u16Val;
	if(index & __PWM_CH4__)
		TIMx->CCR4=u16Val;
}
*/
static void  SetPwmDutyPercent(U8 index,U8 DutyPercent)//0~100
{
	U8 i;
    U16 u16Val;
		TIM_TypeDef * TIMx;
	if(index & __GROUP_INDEX2__)
		TIMx=TIM2;
	else if(index & __GROUP_INDEX3__)
		TIMx=TIM3;
	else if(index & __GROUP_INDEX4__)
		TIMx=TIM4;
	else
		return;
	u16Val=TIMx->ARR;
	u16Val=(u16Val*DutyPercent)/100;
	
	if(index & __PWM_CH1__)
		TIMx->CCR1=u16Val;
	if(index & __PWM_CH2__)
		TIMx->CCR2=u16Val;
	if(index & __PWM_CH3__)
		TIMx->CCR3=u16Val;
	if(index & __PWM_CH4__)
		TIMx->CCR4=u16Val;
	
	for(i=0;i<COUNTOF(PwmPara);i++)	
	{
		if(PwmPara[i].index==index)
		{
			PwmPara[i].duty=DutyPercent;
			break;
		}
	}
}

static void PwmGpioInit(U8 index)
{
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable clock for GPIOA
	//RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // enable clock for GPIOB
	switch(index)
	{
		case PWM_CH01:
			//GPIOA->CRL&=0xFFFFFFF0;//清PA0设置 
			//GPIOA->CRL|=0x0000000A;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_0;      //PA0 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_0; 
		  InitPortBit(PORTA,0,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,0);
	  break;
	  case PWM_CH02:
			//GPIOA->CRL&=0xFFFFFF0F;//清PA1设置 
			//GPIOA->CRL|=0x000000A0;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_1;      //PA1 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_1; 
		  InitPortBit(PORTA,1,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,1);
			break;
		case PWM_CH03:
			//GPIOA->CRL&=0xFFFFF0FF;//清PA2设置 
			//GPIOA->CRL|=0x00000A00;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_2;      //PA2 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_2; 
		  InitPortBit(PORTA,2,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,2);
			break;
	  case PWM_CH04:
			//GPIOA->CRL&=0xFFFF0FFF;//清PA3设置 
			//GPIOA->CRL|=0x0000A000;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_3;      //PA3 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_3; 
		  InitPortBit(PORTA,3,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,3);
	    break;
		//------------------------------------------------
		case PWM_CH05:
			//GPIOA->CRL&=0xF0FFFFFF;//清PA6设置 
			//GPIOA->CRL|=0x0A000000;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_6;      //PA6 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_6; 
		  InitPortBit(PORTA,6,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,6);
	  break;
	  case PWM_CH06:
			//GPIOA->CRL&=0x0FFFFFFF;//清PA7设置 
			//GPIOA->CRL|=0xA0000000;//复用功能输出   
			//GPIOA->ODR|=(U32)GPIO_Pin_7;      //PA7 上拉
			//GPIOA->BRR=(U32)GPIO_Pin_7; 
		  InitPortBit(PORTA,7,AF_PUSH_PULL,1);
		  ClrPortBit(PORTA,7);
			break;
		case PWM_CH07:
			//GPIOB->CRL&=0xFFFFFFF0;//清PB0设置 
			//GPIOB->CRL|=0x0000000A;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_0;      //PB0 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_0; 
		  InitPortBit(PORTB,0,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,0);
			break;
	  case PWM_CH08:
			//GPIOB->CRL&=0xFFFFFF0F;//清PB1设置 
			//GPIOB->CRL|=0x000000A0;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_1;      //PB1 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_1; 
		  InitPortBit(PORTB,1,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,1);
	    break;
		//----------------------------------------------------
		case PWM_CH09:
			//GPIOB->CRL&=0xF0FFFFFF;//清PB6设置 
			//GPIOB->CRL|=0x0A000000;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_6;      //PB6 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_6;
      InitPortBit(PORTB,6,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,6);		
			break;
	  case PWM_CH10:
			//GPIOB->CRL&=0x0FFFFFFF;//清PB7设置 
			//GPIOB->CRL|=0xA0000000;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_7;      //PB7 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_7;
      InitPortBit(PORTB,7,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,7);		
			break;
		case PWM_CH11:
			//GPIOB->CRH&=0xFFFFFFF0;//清PB8设置 
			//GPIOB->CRH|=0x0000000A;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_8;      //PB8 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_8; 
		  InitPortBit(PORTB,8,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,8);
			break;
	  case PWM_CH12:
			//GPIOB->CRH&=0xFFFFFF0F;//清PB9设置 
			//GPIOB->CRH|=0x000000A0;//复用功能输出   
			//GPIOB->ODR|=(U32)GPIO_Pin_9;      //PB9 上拉
			//GPIOB->BRR=(U32)GPIO_Pin_9; 
		  InitPortBit(PORTB,9,AF_PUSH_PULL,1);
		  ClrPortBit(PORTB,9);
	    break;
		default:
			break;
	}
}
//Hz参数为PWM频率，DutyPercent为占空比 
static void InitPWM(U8 index,U32 Hz,U8 DutyPercent)
{
	TIM_TypeDef * TIMx;
	if(index & __GROUP_INDEX2__)
		TIMx=TIM2;
	else if(index & __GROUP_INDEX3__)
		TIMx=TIM3;
	else if(index & __GROUP_INDEX4__)
		TIMx=TIM4;
	else
		return;
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;       //TIM2 时钟使能 
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;       //TIM3 时钟使能 
	RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;       //TIM4 时钟使能   
	
  PwmGpioInit(index);
 
  //TIMx->CCMR1=0;
  //TIMx->CCMR2=0;	
	//TIMx->CCER=0;
	//TIMx->CR1=0;
	//TIMx->CR2=0;
  SetPwmFrequency(index,Hz);
	SetPwmDutyPercent(index,DutyPercent);
	if(index & __PWM_CH1__)
	{
		TIMx->CCMR1|=6<<4;      //CH1 PWM2模式     
	  TIMx->CCMR1|=1<<3;      //CH1 预装载使能      
	  TIMx->CCER|=1<<0;       //OC1  输出使能 
	}
	
	if(index & __PWM_CH2__)
	{
		TIMx->CCMR1|=6<<12;      //CH2 PWM2模式     
	  TIMx->CCMR1|=1<<11;      //CH2 预装载使能      
	  TIMx->CCER|=1<<4;       //OC2  输出使能 
	}
	
	if(index & __PWM_CH3__)
	{
	TIMx->CCMR2|=6<<4;      //CH3 PWM2模式     
	TIMx->CCMR2|=1<<3;      //CH3 预装载使能      
	TIMx->CCER|=1<<8;       //OC3  输出使能    
  }
	
	if(index & __PWM_CH4__)
	{
		TIMx->CCMR2|=6<<12;      //CH4 PWM2模式     
	  TIMx->CCMR2|=1<<11;      //CH4 预装载使能      
	  TIMx->CCER|=1<<12;       //OC4  输出使能 
	}
	
	TIMx->CR1=0x8000;       //ARPE使能  
	TIMx->CR1|=0x01;        //使能定时器
}
//=============================================================================
PwmIo *pSoftPwmIo=NULL;
#define MAX_CNT 5
#define PWM_CYCLE  1000 //s
static BOOLEAN UsingSoftPWM[MAX_CNT]={FALSE,FALSE,FALSE,FALSE,FALSE};
static U32 pwm_cycle[MAX_CNT]={PWM_CYCLE,PWM_CYCLE,PWM_CYCLE,PWM_CYCLE,PWM_CYCLE};
static U8  pwm_duty[MAX_CNT]={100,100,100,100,100};
static BOOLEAN IsHigh[MAX_CNT]={FALSE,FALSE,FALSE,FALSE,FALSE};
static U32 HighVal[MAX_CNT]={500,500,500,500,500};
static U32 LowVal[MAX_CNT]={500,500,500,500,500};

static U32 TimerValue[MAX_CNT]={0,0,0,0,0};

static void SoftPWM(U8 Index,U32 cycle,U8 duty)
{
	U8 Port,Bit,i;
	if(Index>=MAX_CNT)
		  return ;
	if(pSoftPwmIo==NULL)
		return;
	for(i=0;i<MAX_CNT;i++)
  {
		if(pSoftPwmIo[i].Port==0xff || pSoftPwmIo[i].Bit == 0xff)
			return;
		if(i==Index)
		{
	  	Port=pSoftPwmIo[Index].Port;
			Bit=pSoftPwmIo[Index].Bit;
			break;
		}
	}
	
	if(UsingSoftPWM[Index]==FALSE)
	{
		InitPortBit(Port,Bit,OUTPUT_PUSH_PULL,1);
		UsingSoftPWM[Index]=TRUE;
	}
	
	if(pwm_cycle[Index]==cycle && pwm_duty[Index]==duty)
		return ;

	pwm_cycle[Index]=cycle;
	pwm_duty[Index]=duty;

	HighVal[Index]=pwm_cycle[Index]*pwm_duty[Index]/100;
	LowVal[Index]=pwm_cycle[Index]*(100-pwm_duty[Index])/100;

	if(pwm_duty[Index]==100)
	{
		   TimerValue[Index]=pwm_cycle[Index];
			 //SetPortBit(PORTC,15);
		   SetPortBit(Port,Bit);
	}
	else if(pwm_duty[Index]==0)
	{
		   TimerValue[Index]=pwm_cycle[Index]; 
		   //ClrPortBit(PORTC,15);
		   ClrPortBit(Port,Bit);
	}
	else
	{
		 if(IsHigh[Index])
		 {
			 TimerValue[Index]=HighVal[Index];
			 //SetPortBit(PORTC,15);
			 SetPortBit(Port,Bit);
		 }
		 else
		 {
			 TimerValue[Index]=LowVal[Index];
			 //ClrPortBit(PORTC,15);
			 ClrPortBit(Port,Bit);
		 }
   }
}


static void SoftPWM_polling(U8 Index,U8 Tick)
{
	U8 Port,Bit;
	if(Index>=MAX_CNT)
		  return ;
	if(pSoftPwmIo==NULL)
		return;
	if(UsingSoftPWM[Index]==FALSE)
		return;
	if(TimerValue[Index]>Tick)
		TimerValue[Index]=TimerValue[Index]-Tick;

	if(TimerValue[Index]>Tick)
		return;
	Port=pSoftPwmIo[Index].Port;
	Bit=pSoftPwmIo[Index].Bit;
	IsHigh[Index]=~IsHigh[Index];

	if(pwm_duty[Index]==100)
	{
		TimerValue[Index]=pwm_cycle[Index];
		//SetPortBit(PORTC,15);
		SetPortBit(Port,Bit);
	}
	else if(pwm_duty[Index]==0)
	{
		TimerValue[Index]=pwm_cycle[Index]; 
		//ClrPortBit(PORTC,15);
		ClrPortBit(Port,Bit);
	}
	else
	{
		if(IsHigh[Index])
		{
			TimerValue[Index]=HighVal[Index];
			//SetPortBit(PORTC,15);
			SetPortBit(Port,Bit);
		}
		else
		{
			TimerValue[Index]=LowVal[Index];
			//ClrPortBit(PORTC,15);
			ClrPortBit(Port,Bit);
		}
   }
}
//================================================================

static void SoftPWM_Polling(void)
{
	U8 i;
	for(i=0;i<MAX_CNT;i++)
  {
		if(pSoftPwmIo[i].Port==0xff || pSoftPwmIo[i].Bit == 0xff)
			break;
	  SoftPWM_polling(i,10);
	}
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
	pSoftPwmIo=va_arg(arg_ptr, PwmIo *);
	if(pSoftPwmIo!=NULL)
	QueueTimerCreate(100,TRUE,TRUE,TRUE,SoftPWM_Polling);
         printf("STM32F PWM Online !!!\n");
	return TRUE;
}

static U32 SetFreqDuty(U32 cmd,va_list arg_ptr)
{
	U8 i;
	U8 index=va_arg(arg_ptr, VA_U8);
	U32 Hz=va_arg(arg_ptr, VA_U32);
	U8 Duty=va_arg(arg_ptr, VA_U8);
	_PARAMETER_UNUSED_(cmd);
	for(i=0;i<COUNTOF(PwmPara);i++)	
	{
		if(PwmPara[i].index==index)
		{
			break;
		}
	}
	if(i==COUNTOF(PwmPara))
		return FALSE;
	
	if(index <0x10 && Hz>1000)//soft pwm freq <= 1000
	{
		Hz=1000;
	}
	
	if(index<0x10)
	{
		SoftPWM(index,1000/Hz,Duty);
		PwmPara[i].duty=Duty;
		PwmPara[i].freq=Hz;
		PwmPara[i].first=0;
		return TRUE;
	}
	
	if(PwmPara[i].first)
	{
			InitPWM(index,Hz,Duty);
			PwmPara[i].first=0;
	}
	else
	{
			SetPwmDutyPercent(index,Duty);
			SetPwmFrequency(index,Hz);
	}
	return TRUE;
}

static U32 SetFreq(U32 cmd,va_list arg_ptr)
{
	U8 i;
	U8 index=va_arg(arg_ptr, VA_U8);
	U32 Hz=va_arg(arg_ptr, VA_U32);
	_PARAMETER_UNUSED_(cmd);
	for(i=0;i<COUNTOF(PwmPara);i++)	
	{
		if(PwmPara[i].index==index)
		{
			break;
		}
	}
	if(i==COUNTOF(PwmPara))
		return FALSE;
	
	if(index <0x10 && Hz>1000)//soft pwm freq <= 1000
	{
		Hz=1000;
	}
	
	if(index<0x10)
	{
		SoftPWM(index,1000/Hz,PwmPara[i].duty);
		PwmPara[i].freq=Hz;
		PwmPara[i].first=0;
		return TRUE;
	}
	
	if(PwmPara[i].first)
	{
		InitPWM(index,Hz,PwmPara[i].duty);
		PwmPara[i].first=0;
	}
	else
	{
		SetPwmDutyPercent(index,PwmPara[i].duty);
		SetPwmFrequency(index,Hz);
	}
	return TRUE;
}

static U32 SetDuty(U32 cmd,va_list arg_ptr)
{
	U8 i;
	U8 index=va_arg(arg_ptr, VA_U8);
	U8 Duty=va_arg(arg_ptr, VA_U8);
	_PARAMETER_UNUSED_(cmd);
	for(i=0;i<COUNTOF(PwmPara);i++)	
	{
		if(PwmPara[i].index==index)
		{
			break;
		}
	}
	if(i==COUNTOF(PwmPara))
		return FALSE;
	
	if(index<0x10)
	{
		SoftPWM(index,1000/PwmPara[i].freq,Duty);
		PwmPara[i].duty=Duty;
		PwmPara[i].first=0;
		return TRUE;
	}
	
	if(PwmPara[i].first)
	{
		InitPWM(index,PwmPara[i].freq,Duty);
		PwmPara[i].first=0;
	}
	else
	{
		SetPwmDutyPercent(index,Duty);
		SetPwmFrequency(index,PwmPara[i].freq);
	}
	return TRUE;
}

static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{PWMCTL_FREQ_DUTY,		SetFreqDuty},
	{PWMCTL_FREQ,	SetFreq},
	{PWMCTL_DUTY,	SetDuty},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_PWM_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_PWM_Driver =
{   
	NULL,
	PWM_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_PWM_PatchTable
};

//===========================================================



