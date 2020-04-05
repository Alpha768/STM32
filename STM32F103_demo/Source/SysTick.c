
#include "Include.h"

volatile U32 SysTickCounter=0;

void SysTick_Handler(void)
{
  	  SysTickCounter++;
  	  Timer10MsCallBack();
}

U32 GetSysTime(void)
{
	return(SysTickCounter);
}
