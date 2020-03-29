/*----------------------------------------------------------------------------
 * Name:    serial.h
 * Purpose: serial port handling definitions
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


/*----------------------------------------------------------------------------
  Serial interface related prototypes
 *---------------------------------------------------------------------------*/
//extern int   ser_Write     (unsigned char *buffer, int length);
//extern int   ser_Read      (unsigned char *buffer, int length);
//extern void ser_InitPort (unsigned long baudrate, unsigned int  databits,
//                  unsigned int  parity,   unsigned int  stopbits);

#define UART_BUFFER_SIZE 128

#define UART_CH1 0
#define UART_CH2 1
typedef void (*__COMM_CALLBACK__)(RingBuffer_t *ReceiveBuffer,RingBuffer_t *TransmitBuffer);

typedef struct tagCommCallback{
	__COMM_CALLBACK__ *ReceiveCallBack;
	__COMM_CALLBACK__ *HookCallBack;
	U32 ReceiveInterval;
  BOOLEAN IsPolling;
}CommCallBack;

extern CommCallBack Uart1CallBack;
