
#include "Include.h"

#if 0

#define CLOCK 72/8 // ±÷”=72M

//us—” ±∫Ø ˝ 
void Delay1Us(U32 Us)
{
        U8 n;                    
        while(Us--)for(n=0;n<CLOCK;n++);          
}

void Delay1Ms(U32 ms)
{
 /*
  U16 i;
	//TIM1->CR1 |= TIMX_CR1_CEN;                              // enable timer
	if(TIM1->SR & TIMX_SR_UIF)
	   TIM1->SR = TIM1->SR & (~TIMX_SR_UIF);
  for(i=0;i<ms;i++)
  {
  	  while((TIM1->SR & TIMX_SR_UIF)==0);
	  TIM1->SR = TIM1->SR & (~TIMX_SR_UIF);
  }
  */
	/*
  U16 i;
  U32 j;
  for(i=0;i<ms;i++)
   for(j=0;j<(1250+80);j++)
        ;
	*/
	
	 U32 n;                    
   while(ms--)for(n=0;n<(CLOCK*1000);n++);
}

void Delay10Ms(U16 m10s)
{
	/*
	while( m10s-- != 0) 
	{
		Delay1Ms(10);
	}
	*/
	U32 n;                    
  while(m10s--)for(n=0;n<(CLOCK*10000);n++);
}
#else
//#define HCLK 72000000

void Delay1Us(U32 Us)
{
    unsigned int dwCurCounter=0;                                //
    unsigned int dwPreTickVal=SysTick->VAL;                     //
    unsigned int dwCurTickVal;                                  //
    
	  //Us=Us*(HCLK/1000000);
	  Us=Us*(stm32_GetSYSCLK()/1000000);
	  
    for(;;){
        dwCurTickVal=SysTick->VAL;
        if(dwCurTickVal<dwPreTickVal){
            dwCurCounter=dwCurCounter+dwPreTickVal-dwCurTickVal;
        }
        else{
            dwCurCounter=dwCurCounter+dwPreTickVal+SysTick->LOAD-dwCurTickVal;
        }
        dwPreTickVal=dwCurTickVal;
        if(dwCurCounter>=Us){
            return;
        }
    }
}
void Delay1Ms(U32 ms)
{
	unsigned int i=ms;
	while(i--)
	{
	Delay1Us(1000);
	}
}

void Delay10Ms(U32 m10s)
{
	unsigned int i=m10s;
	while(i--)
	{
	Delay1Us(10000);
	}
}

#endif
//=====================================================================================
static TimerQueue TimerPool[TIMER_MAX]={{0,0,0,NULL}};

void QueueTimerInit(void)
{
	U8 Index=0;
	for(Index=0;Index<TIMER_MAX;Index++)
	{
		TimerPool[Index].Flags=0;
		TimerPool[Index].Interval=0;
		TimerPool[Index].TimerCnt=0;
		TimerPool[Index].TimerCallBack=NULL;
	}
}

U8 QueueTimerCreate(U32 Interval,BOOLEAN bStart,BOOLEAN bRepeat,BOOLEAN bPolling,CallBackPtr TimerCallBack)
{
	U8 Index;

	if(TimerCallBack == NULL)
		return 0xff;

    for(Index=0;Index<TIMER_MAX;Index++)
	{
		if(TimerPool[Index].Flags==0)
        {
			  TimerPool[Index].TimerCallBack=TimerCallBack;
			  TimerPool[Index].Interval=Interval;
			  TimerPool[Index].TimerCnt=Interval;
			  if(bRepeat)
              TimerPool[Index].Flags=TIMER_REPEAT;
				if(bPolling)
              TimerPool[Index].Flags|=TIMER_POLLING;
			  if(bStart)
              TimerPool[Index].Flags|=TIMER_START;
			  TimerPool[Index].Flags|=TIMER_USED;
			  return Index;
		}
	}
	return 0xff;
}

U8 QueueTimerDelete(U8 Index)
{
	if(Index <TIMER_MAX)
      TimerPool[Index].Flags=0;
	else
	  return FALSE;
	return TRUE;
}

U8 QueueTimerStop(U8 Index)
{
	if(Index <TIMER_MAX)
      TimerPool[Index].Flags&=(~(TIMER_START|TIMER_OUT));
	else
	  return FALSE;
	return TRUE;
}

U8 QueueTimerStart(U8 Index)
{
	if(Index <TIMER_MAX)
	{
	  TimerPool[Index].TimerCnt=TimerPool[Index].Interval;
      TimerPool[Index].Flags|=TIMER_START;
	}
	else
	  return FALSE;
	return TRUE;
}

U8 QueueTimerInterval(U8 Index,U32 Interval)
{
	if(Index <TIMER_MAX)
	{
		TimerPool[Index].Interval=Interval;
	  TimerPool[Index].TimerCnt=TimerPool[Index].Interval;
	}
	else
	  return FALSE;
	return TRUE;
}

U8 QueueTimerRestore(U8 Index)
{
	if(Index <TIMER_MAX)
	{
	  TimerPool[Index].TimerCnt=TimerPool[Index].Interval;
	}
	else
	  return FALSE;
	return TRUE;
}

U8 QueueTimerFlag(U8 Index,BOOLEAN bRepeat,BOOLEAN bPolling)
{
	if(Index <TIMER_MAX)
	{
		if(bRepeat)
      TimerPool[Index].Flags|=TIMER_REPEAT;
		else
			TimerPool[Index].Flags&=~TIMER_REPEAT;
		if(bPolling)
      TimerPool[Index].Flags|=TIMER_POLLING;
		else
			TimerPool[Index].Flags&=~TIMER_POLLING;
	}
	else
	  return FALSE;
	return TRUE;
}

void TimerQueueTick(U8 TickMs)
{
	U8 Index;
	for(Index=0;Index<TIMER_MAX;Index++)
	{
        if((TimerPool[Index].Flags&(TIMER_USED|TIMER_START))!=(TIMER_USED|TIMER_START))
            continue;
		if(TimerPool[Index].TimerCnt==0)
					  continue;
        if(TimerPool[Index].TimerCnt>=TickMs)
			TimerPool[Index].TimerCnt-=TickMs;
		else
            TimerPool[Index].TimerCnt=0;
		if(TimerPool[Index].TimerCnt==0)
		{
			TimerPool[Index].Flags|=TIMER_OUT;
			if(TimerPool[Index].Flags&TIMER_REPEAT)
				TimerPool[Index].TimerCnt=TimerPool[Index].Interval;
			if(TimerPool[Index].Flags&TIMER_POLLING)
								continue;
			if(TimerPool[Index].TimerCallBack!=NULL)
				TimerPool[Index].TimerCallBack();
		}
	}
}

void QueueTimerPolling(void)
{
	U8 Index;
	for(Index=0;Index<TIMER_MAX;Index++)
	{
		if((TimerPool[Index].Flags&(TIMER_USED|TIMER_POLLING))!=(TIMER_USED|TIMER_POLLING))
            continue;
		if(TimerPool[Index].Flags&TIMER_OUT)
		{
			TimerPool[Index].Flags&=(~TIMER_OUT);
			if(TimerPool[Index].TimerCallBack!=NULL)
			TimerPool[Index].TimerCallBack();
		}
	}
}
//=====================================================================================

void Timer10MsCallBack(void)
{
	//m_BaseTick ++ ;
	TimerQueueTick(BASE_TIME);
	/*
	if (m_BaseTick == (1000 / BASE_TIME))//1s 
	{
		//g_TimerTick ++ ;
		m_BaseTick = 0 ;
	}
	*/
}
