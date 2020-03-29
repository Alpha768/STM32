
#include "Include.h"

#ifdef USE_AES
U8  AesKey[32]="1234567890ABCDEFGHIJabcdefghijOK";
#endif

//---------------------------------------
#define BOOT_ADDR     0x20002800
//#define SCB_VTOR (*((vu32*)(SCB_BASE + 0x08)))
 typedef  void (*pFunction)(void);
 pFunction boot_code_entry;
 U32 JumpAddress;
 __asm void Set_MSP(U32 mainStackPointer)
{
  msr msp, r0
  bx lr
}
__asm void __ISB(void)
{
	ISB
	bx lr
}
 void execute_boot_code(void)
 {
	//----------------------------
	u32 index = 0;
  //__disable_irq();
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
	USB_Connect(FALSE);
	
    __ISB();
 	// relocate vector table
	SCB->VTOR = BOOT_ADDR;
 	/* Jump to user application */
    JumpAddress = *(volatile U32*) (BOOT_ADDR+4);
    boot_code_entry = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    Set_MSP(*(volatile U32*) BOOT_ADDR);
    boot_code_entry();
 }
//---------------------------------------

void IapCmd(U8* buffer)
{
	U8 *p;
	U32 Address,DataLen,i,Len;
	U8 command=*buffer;
	 switch(command)
	 {
		 case USB_CMD_MAGIC:
			 buffer[0]='A';
			 buffer[1]='P';
			 buffer[2]='P';
			 UsbPackageTransmit(buffer,3);
			 break;
		 case USB_CMD_BUFFER_SIZE:
			 //1024
			 buffer[0]=(USB_BUFFER_SIZE/2)&0xff;
		   buffer[1]=((USB_BUFFER_SIZE/2)>>8)&0xff;
		   buffer[2]=0x00;
		   buffer[3]=0x00;
		   UsbPackageTransmit(buffer,4);
			 break;
		 case USB_CMD_WRITE://load boot
			 Address=buffer[4];
		   Address=Address<<8 | buffer[3];
		   Address=Address<<8 | buffer[2];
		   Address=Address<<8 | buffer[1];
		   //---------------------------------------
		   DataLen=buffer[8];
		   DataLen=DataLen<<8 | buffer[7];
		   DataLen=DataLen<<8 | buffer[6];
		   DataLen=DataLen<<8 | buffer[5];
		   p=(U8*)(Address+BOOT_ADDR);
		   #ifdef USE_AES
		   AES_DECODE(AesKey,&buffer[9],DataLen);
		   #endif
		   for(i=0;i<DataLen;i++)
		       p[i]=buffer[9+i];
				break;
		 case USB_CMD_READ://read boot
			 Address=buffer[4];
		   Address=Address<<8 | buffer[3];
		   Address=Address<<8 | buffer[2];
		   Address=Address<<8 | buffer[1];
		   //---------------------------------------
		   DataLen=buffer[8];
		   DataLen=DataLen<<8 | buffer[7];
		   DataLen=DataLen<<8 | buffer[6];
		   DataLen=DataLen<<8 | buffer[5];
		   p=(U8*)(Address+BOOT_ADDR);
		   //for(i=0;i<DataLen;i++)
		   //    buffer[i]=p[i];
		   //PackageTransmit(buffer,DataLen);
		   do
			 {
				 if(DataLen>(USB_BUFFER_SIZE*2-2))
					 Len=(USB_BUFFER_SIZE*2-2);
				 else
					 Len=DataLen;
				 
				 for(i=0;i<Len;i++)
						buffer[i]=p[i];
                #ifdef USE_AES
					AES_ENCODE(AesKey,buffer,Len);
				#endif				 
				 UsbPackageTransmit(buffer,Len);
				 DataLen=DataLen-Len;
				 p=p+Len;
			 }while(DataLen);
				break;
		 case USB_CMD_RUN_BOOT:
			  execute_boot_code();
				break;
		 default:
			 break;
	 }
}





//*****************************************************************



