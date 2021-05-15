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

/*----------------------------------------------------------------------------
  Main Program
 *------------------------------------------------------------------------------*/


static unsigned char GetKeyValue(void)
{
	U8 u8KeypadStatus;

	DeviceControl(KEYPADCTL_KEYPADSTATUS,&u8KeypadStatus);
	u8KeypadStatus=(u8KeypadStatus^KEYPAD_MASK)&KEYPAD_MASK;
	
	if(!u8KeypadStatus)
	{
	   DeviceControl(IRCTL_IRKEYPADSTATUS,&u8KeypadStatus);
       u8KeypadStatus=(u8KeypadStatus^KEYPAD_MASK)&KEYPAD_MASK;
	}
	
	if(u8KeypadStatus)
	printf("GetKeyValue=0x%x\n",u8KeypadStatus);
	
	return u8KeypadStatus;
}

#if 1
void DisplayShowTestPage(void)
{
	//drvSSD1306_UpdateScreen();
	//WriteDisplayString("OLED Test",DISPLAY_BLUE_COLOR,30,2);
	//drvSSD1306_DrawString

#if 1
		PageContainer_t container;
		container.header = "TestHeader";
		container.line_one = "Line One";
		container.line_two = "Line Two";
		container.line_three = "Line Three";
		DrawPage(&container);
#endif	
//drvSSD1306_DrawString(0,0,"IIC_OLED");		// 从(64,0)开始显示 	// 因为，【Project=】一共8个字符，横向上占用【64】个点
//DrawFilledCircle();

}
#endif

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
	
	g_SystemMode = MODE_NORMAL;

	//m_BaseTick=0;
	
	g_Current=0;
  f_USBCableNew=FALSE;
  f_USBCableOld=FALSE;
	f_USB_Error=FALSE;
}


//SEGGER_RTT_printf(0, "cnt = %d\n",cnt); 	 //RTT打印



int main (void) 
{
	U16 Value;
	
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
	DeviceRegister(&f_STM32F_IR_Driver,NULL);
	DeviceRegister(&f_STM32F_Keypad_Driver,NULL);
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
	// 如果在CPU刚开始工作时就有键盘命令，忽略掉，并禁止键盘直到按键松开
	DeviceControl(KEYCTL_CLEAN);

	
    #ifdef SEGGER_RTT
    /* 配置通道0，上行配置（STM32->RTT Viewer软件） */
    SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    
    /* 配置通道0，下行配置（RTT Viewer软件->STM32） */	 
    SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    #endif
	

	printf("Wellcome STM32 System\n");
	printf("ADC Value=0x%x\n",Value);
	//SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

	//OLED_Init();  //OLED初始化
	//OLED_Clear();
	
	// InitDisplay
	//InitDisplay();
	
    // UpdateDisplay
	//DisplayShowTestPage();

		
	//	OLED_ShowString(30,2,"OLED TEST");// OLED TEST
	//	OLED_ShowCHinese(16,0,0);// 技
	//	OLED_ShowCHinese(32,0,1);// 新
	//	OLED_ShowCHinese(48,0,2);// 电
	//	OLED_ShowCHinese(64,0,3);// 子
	//	OLED_ShowCHinese(80,0,4);// 科
	//	OLED_ShowCHinese(96,0,5);// 技
	//OLED_Clear();

        while (1) 
        {    
        
            /* Loop forever */
      		ExeUsbCMD();
      		ExeUsrCMD();
      		ExeSysCMD();
      		IdleCallBack();
			
        }											   
} 
