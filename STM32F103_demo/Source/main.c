/*----------------------------------------------------------------------------
 * Name:    memory.c
 * Purpose: USB Memory Storage Demo
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
#define		MAP_VARS_HERE
#include "Include.h"
#include "Light.h"
#include "Switch.h"

/*----------------------------------------------------------------------------
  Main Program
 *------------------------------------------------------------------------------*/
IRMP_DATA irmp_data;


static unsigned char GetKeyValue(void)
{
    //if(IO_KEY_1==0)
		//return KEY_1;
	
	return NO_COMMAND;
}

//PwmIo SoftPwmIo[]={{PORTC,6},{0xff,0xff}};
PwmIo SoftPwmIo[]={{0xff,0xff}};

void RamColdInitialize( void )
{	
	USBReceiveBuffer.size=USB_BUFFER_SIZE;
	USBReceiveBuffer.buffer=USBRevRingBuffer;
	USBReceiveBuffer.Qinit=TRUE;
	
	USBTransmitBuffer.size=USB_BUFFER_SIZE;
	USBTransmitBuffer.buffer=USBTranRingBuffer;
	USBTransmitBuffer.Qinit=TRUE;
	UsbDataCnt=0;
	ExeUsbCmdStatus=STATUS_IDLE;
	ExeUartCmdBusy=FALSE;
	
	g_SystemMode = MODE_STANBY;
	
	//m_BaseTick=0;
	
	g_Current=0;
  f_USBCableNew=FALSE;
  f_USBCableOld=FALSE;
	f_USB_Error=FALSE;
}


void
timer2_init (void)
{
#if 0
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 7;
    TIM_TimeBaseStructure.TIM_Prescaler = ((F_CPU / F_INTERRUPTS)/8) - 1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
#endif
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 					// enable clock for TIM1

    TIM2->PSC = ((stm32_GetSYSCLK()/ F_INTERRUPTS)/8) - 1;// set prescaler  72MHz/(71+1)=1MHz
    TIM2->ARR = 7;// set auto-reload	(255+1)=256us
    TIM2->RCR = 0; // set repetition counter
    
    TIM2->CR1 = 0; // reset command register 1
    TIM2->CR2 = 0; // reset command register 2
    
    TIM2->DIER = TIM_IT_Update;							// enable interrupt
    NVIC->ISER[0]  = (1 << (TIM2_IRQChannel & 0x1F));  // enable interrupt
    TIM2->CR1 |= TIMX_CR1_CEN;							  // enable timer

	
}

void
TIM2_IRQHandler(void)                                                  // Timer2 Interrupt Handler
{
 // TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
 
   TIM2->SR = (uint16_t)~TIM_IT_Update;
  (void) irmp_ISR();                                                        // call irmp ISR
  // call other timer interrupt routines...
}


int main (void) 
{
	U16 Value;

	Light *light = (Light*)Light_new();
	Switch *wall = (Switch*)Switch_new();

	
	DisableGlobalInterrupt();
    MCUInit();
    stm32_Init ();                            /* STM32 Initialization    */
	RamColdInitialize();
	InitGPIO();
	QueueTimerInit();

	DeviceRegister(&f_STM32F_UART_Driver,NULL);	
    #ifdef USE_UART1
    DeviceControl(UARTCTL_INIT,UART_CH1,9600,8,2,1,&Uart1CallBack);
    #endif
    #ifdef USE_UART2
    DeviceControl(UARTCTL_INIT,UART_CH2,9600,8,2,1,&Uart1CallBack);
    #endif

	DeviceRegister(&f_STM32F_PANEL_Driver,NULL);
	DeviceRegister(&f_STM32F_FLASH_Driver,NULL);
	DeviceRegister(&f_KeyBoard_Driver,NULL,GetKeyValue);
	DeviceRegister(&f_STM32F_ADC_Driver,NULL);
	DeviceRegister(&f_STM32F_PWM_Driver,NULL,SoftPwmIo);


	DeviceControl(PWMCTL_FREQ_DUTY,SOFT_PWM_CH01,100,80);//10k for dlp led
	LoadSetting();
	Effect_Usr_Cfg();
	DeviceControl(ADCCTL_CONVERT,ADC_CH5,&Value);
	QueueTimerCreate(1000,TRUE,TRUE,TRUE,LedFlicker);
	QueueTimerCreate(1000,TRUE,TRUE,TRUE,SaveSetting);//for eeprom save
    QueueTimerCreate(400,TRUE,TRUE,FALSE,USBCableCheck);//for USB Cable Check
    UsbRevTimerIndex=QueueTimerCreate(0,TRUE,FALSE,FALSE,UsbRevTimerCallBack);//for USB TimeOut
    USB_Init();                          /* USB Initialization */
    USB_Connect(0);                      /* USB DisConnect */

	
	
	USB_Configuration=0;
	EnableGlobalInterrupt();
	// �����CPU�տ�ʼ����ʱ���м���������Ե�������ֹ����ֱ�������ɿ�
	DeviceControl(KEYCTL_CLEAN);

	printf("Wellcome STM32 System\n");
	timer2_init();
    irmp_init();                                                            // initialize irmp

	OLED_Init();  //OLED��ʼ��
	OLED_Clear();
	
	light->init(light);
	printf("Light Init\n");
	wall->light_obj = light;
	printf("Switch  get Obj\n");
	wall->set_switch(wall);
	printf("Switch  set switch\n");
	printf("Alpha Test Here\n");

		
		OLED_ShowString(30,2,"OLED TEST");// OLED TEST
	//	OLED_ShowCHinese(16,0,0);// ��
	//	OLED_ShowCHinese(32,0,1);// ��
	//	OLED_ShowCHinese(48,0,2);// ��
	//	OLED_ShowCHinese(64,0,3);// ��
	//	OLED_ShowCHinese(80,0,4);// ��
	//	OLED_ShowCHinese(96,0,5);// ��
	//OLED_Clear();

        while (1) 
        {    

			
               if (irmp_get_data (&irmp_data))
               {					
                 //printf("R: Code: %s",irmp_protocol_names[irmp_data.protocol]);          
                 printf(" Address: 0x%.2X\n",irmp_data.address);
                 printf(" Command: 0x%.2X\n",irmp_data.command);
                 printf(" Flags: 0x%.2X\r\n",irmp_data.flags );										
               }

			   
                 /* Loop forever */
      		ExeUsbCMD();
      		ExeUsrCMD();
      		ExeSysCMD();
      		IdleCallBack();
        }											   
} 
