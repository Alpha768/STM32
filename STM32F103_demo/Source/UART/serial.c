/*----------------------------------------------------------------------------
 * Name:    serial.c
 * Purpose: serial port handling for MCB2300
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
 

#include "Include.h"

#ifdef USE_UART1
static U8 Uart1RevRingBuffer[UART_BUFFER_SIZE];
static RingBuffer_t Uart1ReceiveBuffer;
static U16 Uart1RevTimeOut=0;
static U8 Uart1TransmitRingBuffer[UART_BUFFER_SIZE];
static RingBuffer_t Uart1TransmitBuffer;
static CommCallBack *pUart1CallBack=NULL;
#endif

#ifdef USE_UART2
static U8 Uart2RevRingBuffer[UART_BUFFER_SIZE];
static RingBuffer_t Uart2ReceiveBuffer;
static U16 Uart2RevTimeOut=0;
static U8 Uart2TransmitRingBuffer[UART_BUFFER_SIZE];
static RingBuffer_t Uart2TransmitBuffer;
static CommCallBack *pUart2CallBack=NULL;
#endif
//-----------------------------------------------------------------------
#ifdef USE_UART1
static void Uart1Hook(void);
#endif
#ifdef USE_UART2
static void Uart2Hook(void);
#endif
//-----------------------------------------------------------------------


#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


#define CR1_CLEAR_Mask            ((u16)0xE9F3)  /* USART CR1 Mask */
#define CR2_STOP_CLEAR_Mask       ((u16)0xCFFF)  /* USART CR2 STOP Bits Mask */
#define CR3_CLEAR_Mask            ((u16)0xFCFF)  /* USART CR3 Mask */


#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

//__use_no_semihosting was requested, but _ttywrch was 
_ttywrch(int ch)
{
ch = ch;
}


//void ser_InitUart2 (unsigned long baudrate, unsigned int  databits,
//                  unsigned int  parity,   unsigned int  stopbits);
//void ser_InitUart1 (unsigned long baudrate, unsigned int  databits,
//                  unsigned int  parity,   unsigned int  stopbits);
 /*----------------------------------------------------------------------------
 STM32 USART setup.
 initializes the USARTx register
 *----------------------------------------------------------------------------*/

void ser_InitUart2 (unsigned long baudrate, unsigned int  databits,
                  unsigned int  parity,   unsigned int  stopbits) {

  unsigned int lcr_p, lcr_s, lcr_d,pclk1;

  switch (databits) {
	case 9:
	  lcr_d = 0x00001000;							   /* 9 Data bits */	  
	break;
    case 8:                                            /* 8 Data bits */
    default:
      lcr_d = 0x00000000;
    break;
  }

  switch (stopbits) {
    case 1:                                            /* 1,5 Stop bits */
	  lcr_s = 0x00003000;
	break;
    case 2:                                            /* 2   Stop bits */
      lcr_s = 0x00002000;
    break;
    case 0:                                            /* 1   Stop bit */
    default:
      lcr_s = 0x00000000;
    break;
  }

  switch (parity) {
    case 1:                                            /* Parity Odd */
      lcr_p = 0x00000600;
		  if(databits==8)
				lcr_d = 0x00001000;							   /* 9 Data bits */
    break;
    case 2:                                            /* Parity Even */
      lcr_p = 0x00000400;
		  if(databits==8)
				lcr_d = 0x00001000;							   /* 9 Data bits */
    break;
    //case 3:                                            /* Parity Mark */
    //  lcr_p = 0x28;
    //break;
    //case 4:                                            /* Parity Space */
    //  lcr_p = 0x38;
    //break;
    case 0:                                            /* Parity None */
    default:
      lcr_p = 0x00;
    break;
  }
	
	//===========================================================================
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
	AFIO->MAPR   &= ~(1 << 3);                              // clear USART2 remap
	//RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN;                  // enable clock for GPIOA
  //  GPIOA->CRL   &= ~(0xFFUL  << 8);                      // Clear PA2, PA3
  //  GPIOA->CRL   |=  (0x0BUL  << 8);                      // USART2 Tx (PA2)  alternate output push-pull
  //  GPIOA->CRL   |=  (0x04UL  << 12);                     // USART2 Rx (PA3)  input floating
    InitPortBit(PORTA,2,AF_PUSH_PULL,1);
	InitPortBit(PORTA,3,INPUT_FLOATING,1);

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;                   // enable clock for USART2

	USART2->CR1 &= CR1_CLEAR_Mask;
	USART2->CR2 &= CR2_STOP_CLEAR_Mask;
	USART2->CR3 &= CR3_CLEAR_Mask;
    pclk1=stm32_GetPCLK1();
    USART2->BRR  = __USART_BRR(pclk1, baudrate); // set baudrate
    USART2->CR1  |= lcr_d;                       // set Data bits
    USART2->CR2  |= lcr_s;                       // set Stop bits
    USART2->CR1 |= lcr_p;                         // set Parity
    USART2->CR3  = 0;                       // Set Flow Control

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable

	  USART2->CR1 |= 0x00000020;
      USART2->CR2 |= 0;
      USART2->CR3 |= 0;
      NVIC->ISER[1]  = NVIC->ISER[1]|(1 << (USART2_IRQChannel & 0x1F));   // enable interrupt

	  USART2->CR1 |= USART_CR1_UE;                            // USART enable
} 

void ser_InitUart1 (unsigned long baudrate, unsigned int  databits,
                  unsigned int  parity,   unsigned int  stopbits)
{
	  unsigned int lcr_p, lcr_s, lcr_d,pclk2;

  switch (databits) {
	case 9:
	  lcr_d = 0x00001000;							   /* 9 Data bits */	  
	break;
    case 8:                                            /* 8 Data bits */
    default:
      lcr_d = 0x00000000;
    break;
  }

  switch (stopbits) {
    case 1:                                            /* 1,5 Stop bits */
	  lcr_s = 0x00003000;
	break;
    case 2:                                            /* 2   Stop bits */
      lcr_s = 0x00002000;
    break;
    case 0:                                            /* 1   Stop bit */
    default:
      lcr_s = 0x00000000;
    break;
  }

  switch (parity) {
    case 1:                                            /* Parity Odd */
      lcr_p = 0x00000600;
		  if(databits==8)
				lcr_d = 0x00001000;							   /* 9 Data bits */
    break;
    case 2:                                            /* Parity Even */
      lcr_p = 0x00000400;
		  if(databits==8)
				lcr_d = 0x00001000;							   /* 9 Data bits */
    break;
    //case 3:                                            /* Parity Mark */
    //  lcr_p = 0x28;
    //break;
    //case 4:                                            /* Parity Space */
    //  lcr_p = 0x38;
    //break;
    case 0:                                            /* Parity None */
    default:
      lcr_p = 0x00;
    break;
  }
                                            
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
    AFIO->MAPR   &= ~(1 << 2);                              // clear USART1 remap
    //RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                   // enable clock for GPIOA
    //GPIOA->CRH   &= ~(0xFFUL  << 4);                      // Clear PA9, PA10
    //GPIOA->CRH   |=  (0x0BUL  << 4);                      // USART1 Tx (PA9)  alternate output push-pull
    //GPIOA->CRH   |=  (0x04UL  << 8);                      // USART1 Rx (PA10) input floating
	  InitPortBit(PORTA,9,AF_PUSH_PULL,1);
	  InitPortBit(PORTA,10,INPUT_FLOATING,1);

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;                   // enable clock for USART1

	USART1->CR1 &= CR1_CLEAR_Mask;
	USART1->CR2 &= CR2_STOP_CLEAR_Mask;
	USART1->CR3 &= CR3_CLEAR_Mask;
    pclk2=stm32_GetPCLK2();   
    USART1->BRR  = __USART_BRR(pclk2, baudrate); // set baudrate
    USART1->CR1  |= lcr_d;                       // set Data bits
    USART1->CR2  |= lcr_s;                       // set Stop bits
    USART1->CR1 |= lcr_p;                         // set Parity
    USART1->CR3  = 0;                       // Set Flow Control

    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable

     // interrupts used
      USART1->CR1 |= 0x00000020;
      USART1->CR2 |= 0;
      USART1->CR3 |= 0;
      NVIC->ISER[1] = NVIC->ISER[1] | (1 << (USART1_IRQChannel & 0x1F));   // enable interrupt
	  

    USART1->CR1 |= USART_CR1_UE;                            // USART enable

}

static  void ser_InitPort(unsigned char index,unsigned long baudrate, unsigned int  databits,
                  unsigned int  parity,   unsigned int  stopbits)
{
        #ifdef USE_UART1
        if(index == UART_CH1)
             ser_InitUart1 (baudrate,databits,parity,stopbits);
        #endif
	#ifdef USE_UART2
	if(index == UART_CH2)
	ser_InitUart2 (baudrate,databits,parity,stopbits);
	#endif
}

/*----------------------------------------------------------------------------
  read data from serial port
 *---------------------------------------------------------------------------*/
int ser_Read (unsigned char *buffer, int length) 
{
  return 0;
}

/*----------------------------------------------------------------------------
  write data to the serial port
 *---------------------------------------------------------------------------*/

static int SER_PutChar (unsigned char index,int c) {

	#ifdef USE_UART2
	if(index == UART_CH2)
	{
		while (!(USART2->SR & 0x0080));
		USART2->DR = (c & 0x1FF);
	}
	#endif 
	#ifdef USE_UART1
	if(index == UART_CH1)
	{
		while (!(USART1->SR & 0x0080));
		USART1->DR = (c & 0x1FF);
	}
	#endif
  return (c);
}

static int ser_Write (unsigned char index,unsigned char *buffer, unsigned int length) 
{
  unsigned int i;
	for(i=0;i<length;i++)
	{
		SER_PutChar(index,buffer[i]);
	}
  return 0;
}


//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	//while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
        // USART1->DR = (u8) ch;    
        #ifdef USE_UART1
        SER_PutChar(UART_CH1,ch);
	#endif
	
	#ifdef USE_UART2
        SER_PutChar(UART_CH2,ch);
        #endif

	return ch;
}
#ifdef USE_UART1
/*----------------------------------------------------------------------------
  USART1_IRQHandler
  Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler (void) {
  volatile unsigned int IIR;
   unsigned char Value;
    IIR = USART1->SR;
    if (IIR & USART_FLAG_RXNE) {                  // read interrupt
      USART1->SR &= ~USART_FLAG_RXNE;	          // clear interrupt
	  //SER_BUF_WR(ser_in, USART1->DR & 0x1FF);     /* Read Rx FIFO to buffer */
			Uart1RevTimeOut=500;
			Value=USART1->DR & 0x1FF;
      QueueWrite(&Uart1ReceiveBuffer,&Value,1);
			Uart1Hook();
    }
    /*
    if (IIR & USART_FLAG_TXE) {
      USART1->SR &= ~USART_FLAG_TXE;	          // clear interrupt
//-----------------------------------------------------------------------
		if (SER_BUF_COUNT(ser_out) != 0) {
      USART1->DR = SER_BUF_RD(ser_out)& 0x1FF;                     // Write to the Tx FIFO
    }
	else {
	  USART1->CR1 &= ~USART_FLAG_TXE;		      // disable TX interrupt if nothing to send
	}
//-----------------------------------------------------------------------
    }
		*/
}
#endif

#ifdef USE_UART2
/*----------------------------------------------------------------------------
  USART2_IRQHandler
  Handles USART2 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART2_IRQHandler (void) {
  volatile unsigned int IIR;
    //U32 val;
	  unsigned char Value;
    IIR = USART2->SR;
	//if(IIR & USART_FLAG_ORE)
	//   val = USART2->DR;
	//if(IIR & USART_FLAG_FE)
	//   val = USART2->DR;
	//if(IIR & USART_FLAG_NE)
	//   val = USART2->DR;
    if (IIR & USART_FLAG_RXNE) {                  // read interrupt
      USART2->SR &= ~USART_FLAG_RXNE;	          // clear interrupt
	  //SER_BUF_WR(ser_in, USART2->DR & 0x1FF);     /* Read Rx FIFO to buffer */
			Uart2RevTimeOut=500;
			Value=USART2->DR & 0x1FF;
      QueueWrite(&Uart2ReceiveBuffer,&Value,1);
			Uart2Hook();
    }
    /*
    if (IIR & USART_FLAG_TXE) {
      USART2->SR &= ~USART_FLAG_TXE;	          // clear interrupt
//-----------------------------------------------------------------------
		if (SER_BUF_COUNT(ser_out) != 0) {
      USART2->DR = SER_BUF_RD(ser_out)& 0x1FF;                     // Write to the Tx FIFO

    }
	else {

	  USART2->CR1 &= ~USART_FLAG_TXE;		      // disable TX interrupt if nothing to send
	}
//-----------------------------------------------------------------------
    }
		*/
}
#endif
//#########################################################################
static void TimeOutCheck(void)
{
	#ifdef USE_UART1
	 if(Uart1RevTimeOut)
	 {
		 Uart1RevTimeOut--;
		 if(Uart1RevTimeOut==0)
				QueueClear(&Uart1ReceiveBuffer);
	 }
	 #endif

	 #ifdef USE_UART2
	 if(Uart2RevTimeOut)
	 {
		 Uart2RevTimeOut--;
		 if(Uart2RevTimeOut==0)
				QueueClear(&Uart2ReceiveBuffer);
	 }
	 #endif
}

#ifdef USE_UART1
static void Uart1Hook(void)
{
	U8 buffer[UART_BUFFER_SIZE];
	U16 count=0;
	int Index =-1;
	if(pUart1CallBack==NULL)
		return;
	if(pUart1CallBack->HookCallBack==NULL)
		return;
	while(1)
	{
		Index++;
	  if(pUart1CallBack->HookCallBack[Index]==0)
			break;
		pUart1CallBack->HookCallBack[Index](&Uart1ReceiveBuffer,&Uart1TransmitBuffer);
		count=QueueDataCount(&Uart1TransmitBuffer);
		QueueRead(&Uart1TransmitBuffer,buffer,count);		
		ser_Write(UART_CH1,buffer,count);
	}
}
#endif 

#ifdef USE_UART2
static void Uart2Hook(void)
{
	U8 buffer[UART_BUFFER_SIZE];
	U16 count=0;
	int Index =-1;
	if(pUart2CallBack==NULL)
		return;
	if(pUart2CallBack->HookCallBack==NULL)
		return;
	while(1)
	{
		Index++;
	  if(pUart2CallBack->HookCallBack[Index]==0)
			break;
		pUart2CallBack->HookCallBack[Index](&Uart2ReceiveBuffer,&Uart2TransmitBuffer);
		count=QueueDataCount(&Uart2TransmitBuffer);
		QueueRead(&Uart2TransmitBuffer,buffer,count);
		ser_Write(UART_CH2,buffer,count);
	}
}
#endif

#ifdef USE_UART1
static void Uart1RevCallBack(void)
{
	U8 buffer[UART_BUFFER_SIZE];
	U16 count=0;
	int Index =-1;
	if(pUart1CallBack==NULL)
		return;
	if(pUart1CallBack->ReceiveCallBack==NULL)
		return;
	while(1)
	{
		Index++;
	  if(pUart1CallBack->ReceiveCallBack[Index]==0)
			break;
		pUart1CallBack->ReceiveCallBack[Index](&Uart1ReceiveBuffer,&Uart1TransmitBuffer);
		count=QueueDataCount(&Uart1TransmitBuffer);
		QueueRead(&Uart1TransmitBuffer,buffer,count);
		ser_Write(UART_CH1,buffer,count);
	}
}
#endif

#ifdef USE_UART2
static void Uart2RevCallBack(void)
{
	U8 buffer[UART_BUFFER_SIZE];
	U16 count=0;
	int Index =-1;
	if(pUart2CallBack==NULL)
		return;
	if(pUart2CallBack->ReceiveCallBack==NULL)
		return;
	while(1)
	{
		Index++;
	  if(pUart2CallBack->ReceiveCallBack[Index]==0)
			break;
		pUart2CallBack->ReceiveCallBack[Index](&Uart2ReceiveBuffer,&Uart2TransmitBuffer);
		count=QueueDataCount(&Uart2TransmitBuffer);
		QueueRead(&Uart2TransmitBuffer,buffer,count);
		ser_Write(UART_CH2,buffer,count);
	}
}
#endif

static U32 UART_init(U32 cmd,va_list arg_ptr)
{
	U8 index=va_arg(arg_ptr, VA_U8);
  unsigned long baudrate=va_arg(arg_ptr, VA_U32);
	unsigned int  databits=va_arg(arg_ptr, VA_U32);
	unsigned int  parity=va_arg(arg_ptr, VA_U32);
	unsigned int  stopbits=va_arg(arg_ptr, VA_U32);
	CommCallBack *p=va_arg(arg_ptr, CommCallBack *);
	#ifdef USE_UART1
	if(index == UART_CH1)
	{
		Uart1ReceiveBuffer.size=UART_BUFFER_SIZE;
		Uart1ReceiveBuffer.buffer=Uart1RevRingBuffer;
		Uart1ReceiveBuffer.Qinit=TRUE;
		Uart1TransmitBuffer.size=UART_BUFFER_SIZE;
		Uart1TransmitBuffer.buffer=Uart1TransmitRingBuffer;
		Uart1TransmitBuffer.Qinit=TRUE;
		if(p!=NULL)
		{
			pUart1CallBack=p;
			QueueTimerCreate(p->ReceiveInterval,TRUE,TRUE,p->IsPolling,Uart1RevCallBack);
		}
	}
	#endif
	#ifdef USE_UART2
	if(index == UART_CH2)
	{
		Uart2ReceiveBuffer.size=UART_BUFFER_SIZE;
		Uart2ReceiveBuffer.buffer=Uart2RevRingBuffer;
		Uart2ReceiveBuffer.Qinit=TRUE;
		Uart2TransmitBuffer.size=UART_BUFFER_SIZE;
		Uart2TransmitBuffer.buffer=Uart2TransmitRingBuffer;
		Uart2TransmitBuffer.Qinit=TRUE;
		if(p!=NULL)
		{
			pUart2CallBack=p;
			QueueTimerCreate(p->ReceiveInterval,TRUE,TRUE,p->IsPolling,Uart2RevCallBack);
		}
	}
	#endif
	QueueTimerCreate(10,TRUE,TRUE,FALSE,TimeOutCheck);
	ser_InitPort(index,baudrate,databits,parity,stopbits);
	return TRUE;
}

static U32 UART_Transmit(U32 cmd,va_list arg_ptr)
{
	U8 index=va_arg(arg_ptr, VA_U8);
	U8* pBuffer=va_arg(arg_ptr, U8*);
	U32 length=va_arg(arg_ptr, VA_U32);
	ser_Write(index,pBuffer,length);
	return TRUE;
}

static U32 UART_Receive(U32 cmd,va_list arg_ptr)
{
	U8 index=va_arg(arg_ptr, VA_U8);
	U8* pBuffer=va_arg(arg_ptr, U8*);
	U32 length=va_arg(arg_ptr, VA_U32);
	U32 ReVal;
	#ifdef USE_UART1
	if(index == UART_CH1)
	{
		ReVal=QueueRead(&Uart1ReceiveBuffer,pBuffer,length);
		Uart1RevTimeOut=500;
	}
	#endif
	#ifdef USE_UART2
	if(index == UART_CH2)
	{
		ReVal=QueueRead(&Uart2ReceiveBuffer,pBuffer,length);
		Uart2RevTimeOut=500;
	}
	#endif
	return ReVal;
}

static U32 UART_RevCnt(U32 cmd,va_list arg_ptr)
{
	U8 index=va_arg(arg_ptr, VA_U8);
	U32* pCount=va_arg(arg_ptr, U32*);
	#ifdef USE_UART1
	if(index == UART_CH1)
	{
	*pCount=QueueDataCount(&Uart1ReceiveBuffer);
		Uart1RevTimeOut=500;
	}
	#endif
	#ifdef USE_UART2
	if(index == UART_CH2)
	{
	*pCount=QueueDataCount(&Uart2ReceiveBuffer);
		Uart2RevTimeOut=500;
	}
	#endif
	return TRUE;
}

static U32 UART_RevLook(U32 cmd,va_list arg_ptr)
{
	U8 index=va_arg(arg_ptr, VA_U8);
	U8* pBuffer=va_arg(arg_ptr, U8*);
	U32 Count=va_arg(arg_ptr, VA_U32);
	#ifdef USE_UART1
	if(index == UART_CH1)
	{
	QueueLook(&Uart1ReceiveBuffer,pBuffer,Count);
		Uart1RevTimeOut=500;
	}
	#endif
	#ifdef USE_UART2
	if(index == UART_CH2)
	{
	QueueLook(&Uart2ReceiveBuffer,pBuffer,Count);
		Uart2RevTimeOut=500;
	}
	#endif
	return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
  _PARAMETER_UNUSED_(arg_ptr);
  //PRINTF("STM32F UART Online !!!\n",0);
	return TRUE;
}

static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{UARTCTL_INIT,		UART_init},
	{UARTCTL_TRANSMIT,	UART_Transmit},
	{UARTCTL_RECEIVE,UART_Receive},
	{UARTCTL_REVCNT,UART_RevCnt},
	{UARTCTL_REVLOOK,UART_RevLook},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_UART_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_UART_Driver =
{   
	NULL,
	UART_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_UART_PatchTable
};

