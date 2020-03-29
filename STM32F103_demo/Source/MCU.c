#include "Include.h"

//#define SCB_VTOR (*((vu32*)(SCB_BASE + 0x08)))
#define  STM32F_IFLASH_SIZE 0x20000//128Kbytes
#define  STM32F_IFLASH_PAGE_SIZE 1024//1Kbytes
#define  STM32F_IFLASH  0x8000000
	
#define  USER_FLASH_START STM32F_IFLASH

static BOOLEAN f_GlobalInterrupt=TRUE;
void DisableGlobalInterrupt(void)
{
	__disable_irq();
	f_GlobalInterrupt=FALSE;
}

void EnableGlobalInterrupt(void)
{
	__enable_irq();
	f_GlobalInterrupt=TRUE;
}

void RestoreGlobalInterrupt(void)
{
	if(f_GlobalInterrupt)
		__enable_irq();
	else
		__disable_irq();
}

void MCUInit(void)
{
		//----------------------------
	u32 index = 0;
  
  NVIC->ICER[0] = 0xFFFFFFFF;
  NVIC->ICER[1] = 0x000007FF;
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICPR[1] = 0x000007FF;
  
  for(index = 0; index < 0x0B; index++)
  {
     NVIC->IPR[index] = 0x00000000;
  }
	//GPIOC->BRR |=  1<<7;//Clr PC7 green led
	 //----------------------------
	RCC->CR |= (U32)0x00000001; //????8MHz??

	RCC->CFGR &= (U32)0xF8FF0000; //????????CFGR????27-31?????,24-26?MCO? 
  
	RCC->CR &= (U32)0xFEF6FFFF; //???CR???,?????2?
	RCC->CR &= (U32)0xFFFBFFFF; //?18=0,HSE????,???HSE????????

	RCC->CFGR &= (U32)0xFF80FFFF; //??CFGR???,?????3?

	RCC->CIR = 0x009F0000; //???CIR???????,?????4?
	//RCC->CFGR2 = 0x00000000;
	//-----------------------------
	SCB->VTOR = USER_FLASH_START;
}
