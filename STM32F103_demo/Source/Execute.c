
#include "Include.h"
#include "Light.h"
#include "Switch.h"

#define MAX_CURRENT  Target.UserSetting.max_current//40//50
#define MIN_CURRENT  Target.UserSetting.min_current//16
#define ADJ_CURRENT  (MAX_CURRENT-MIN_CURRENT)

Light *light;// = (Light*)Light_new();
Switch *wall;// = (Switch*)Switch_new();

void SetCurrent(U8 Percent)
{
	U8 Value=(Percent*ADJ_CURRENT)/100 + MIN_CURRENT;
	 if(Percent)
		DeviceControl(PWMCTL_DUTY,PWM_CH03,Value);
	 else
		 DeviceControl(PWMCTL_DUTY,PWM_CH03,0);
}

void Effect_Usr_Cfg(void)
{
	//SetCurrent(usr_EEPROM.current);

	light = (Light*)Light_new();
	wall = (Switch*)Switch_new();
	
	light->init(light);
	printf("Light Init\n");
	wall->light_obj = light;
	printf("Switch  get Obj\n");
	printf("Switch  set switch\n");
	printf("Alpha Test Here\n");
}

void PowerSystem(void)
{
	if(g_SystemMode==MODE_NORMAL)
	{
		printf("PowerOff !!\n" );
		g_SystemMode = MODE_STANBY;

	}
	else
	{
		printf("PowerOn !!\n" );
		g_SystemMode = MODE_NORMAL;

	}

}

void PowerOn(void)
{
	printf("PowerOn !!\n" );

	g_SystemMode = MODE_NORMAL;
}

void PowerOff(void)
{
	printf("PowerOff !!\n" );

	g_SystemMode = MODE_STANBY;
}

void ExecSelect(void)
{
	printf("ExecSelect !!\n" );
	//wall->set_switch(wall,LED1);

}

void ExecExit(void)
{
	printf("ExecExit !!\n" );
	wall->set_switch(wall,LED2);

}

void ExecLeft(void)
{
	printf("ExecLeft !!\n" );
	wall->set_switch(wall,LED3);

}

void ExecRight(void)
{
	printf("ExecRight !!\n" );
	wall->set_switch(wall,LED4);

}


COMMAND_TABLE	CommandTable[] = 
{
	{ KEY_POWER,     STATUS_FIRST,MODE_NORMAL,PowerSystem},
	{ KEY_MINUS,	     STATUS_FIRST,MODE_NORMAL,ExecLeft},
	{ KEY_PLUS,	 STATUS_FIRST,MODE_NORMAL,ExecRight},
	{ KEY_MENU,	     STATUS_FIRST,MODE_NORMAL,ExecSelect},
	{ KEY_EXIT,	     STATUS_FIRST,MODE_NORMAL,ExecExit},
	{ 0, 0, 0,0 } 
} ;


void ExeUsrCMD( void )
{
	DeviceControl(KEYCTL_PROCESS,CommandTable);	
}
//-----------------------------------------------------------------------------------

void ExeSysCMD( void )
{
	QueueTimerPolling();
}

//*****************************************************************
void UsbRevTimerCallBack(void)
{
	UsbDataCnt=0;
}


void Uart1ReceiveCallBack(RingBuffer_t *ReceiveBuffer,RingBuffer_t *TransmitBuffer)
{
		//U8 cmd;
	U32 Cnt;
	U8 buffer[60];
	Cnt=QueueDataCount(ReceiveBuffer);
	if(Cnt==0)
			return;
	//QueueLook(ReceiveBuffer,&cmd,1);
	
	QueueRead(ReceiveBuffer,buffer,Cnt);
}

__COMM_CALLBACK__ Uart1CallBackTable[]={Uart1ReceiveCallBack,0};

CommCallBack Uart1CallBack={Uart1CallBackTable,NULL,10,TRUE};

/*
void ExeUartCMD(void)
{
	//U8 cmd;
	U32 Cnt;
	U8 buffer[60];
	DeviceControl(UARTCTL_REVCNT,UART_CH1,&Cnt);
	if(Cnt==0)
			return;
	//DeviceControl(UARTCTL_REVLOOK,UART_CH1,&cmd,1);
		
	DeviceControl(UARTCTL_RECEIVE,UART_CH1,buffer,Cnt);
}
*/

void ExeUsbCMD(void)
{
	U8 command;
	U16 Len;
	U8*p;
	U8 i;

   if(UsbPackageReceive(&p,&Len)==FALSE)
		 return ;   
	 
	 ExeUsbCmdStatus=STATUS_BUSY;
	 command=*p;
	 if(command>=0xf9)
	 {
		 IapCmd(p);
		 ExeUsbCmdStatus=STATUS_IDLE;
		 return ;
	 }
	 switch(command)
	 {
		 case USB_CMD_STATUS:
			 ExeUsbCmdStatus=STATUS_IDLE;
		   UsbPackageTransmit(&ExeUsbCmdStatus,1);
			 break;
//================================================================
		case USB_CMD_CURRENT:
			 if(p[1])
			 {
				 UsbPackageTransmit(&Target.UserSetting.current,1);
				 break;
			 }
			 if(p[2]>100)
				 break ;
			 SetCurrent(p[2]);
			 Target.UserSetting.current=p[2];
			 break;
		case USB_CMD_MAX_CURRENT:
			 if(p[1])
			 {
				 UsbPackageTransmit(&Target.UserSetting.max_current,1);
				 break;
			 }
			 if(p[2]>100)
				 break ;
			 Target.UserSetting.max_current=p[2];
			 SetCurrent(Target.UserSetting.current);
			 break;
		case USB_CMD_MIN_CURRENT:
			 if(p[1])
			 {
				 UsbPackageTransmit(&Target.UserSetting.min_current,1);
				 break;
			 }
			 if(p[2]>100)
				 break ;
			 Target.UserSetting.min_current=p[2];
			 SetCurrent(Target.UserSetting.current);
			 break;
		 case USB_CMD_SET_LED:
		 	if(p[1]==0x01)
			{
				SetPortBit(PORTC,7);
				printf("Set Led\n");
		 	}
			else
			{
				ClrPortBit(PORTC,7);
				printf("Clr Led\n");
			}
			i=0xAA;
			UsbPackageTransmit(&i,1);		
		 	break;
		 default:
			 break;
	 }
	ExeUsbCmdStatus=STATUS_IDLE;
}

void IdleCallBack(void)
{

}


