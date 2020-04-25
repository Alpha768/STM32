#include "Include.h"

#define HW_POWER_KEY

static U32 Key_GetKeypadStatus(U32 cmd,va_list arg_ptr)
{	
	U8 *Value=va_arg(arg_ptr, U8*);
	U8 u8Keypad = 0xFF;
	U8 u8Temp = 0;
	U8 u8Temp1 = 0;

	_PARAMETER_UNUSED_(cmd);
	_PARAMETER_UNUSED_(u8Temp);
	_PARAMETER_UNUSED_(u8Temp1);

	  #if 0
	  DeviceControl(ADCCTL_CONVERT,KEYPAD_ADC_A,&u8Temp);
	  DeviceControl(ADCCTL_CONVERT,KEYPAD_ADC_B,&u8Temp1);

	  u8Temp = u8Temp  & SARADC_MASK;
	  u8Temp1 = u8Temp1  & SARADC_MASK;
	
      printf(" u8Temp: 0x%.2X\n",u8Temp);
      printf(" u8Temp1: 0x%.2X\n",u8Temp1);
	
	  if (abs(u8Temp - ADCKEY_A0_VALUE)  <= KEY_TOL_0)
		  u8Keypad &= ~ADCKEY_A0;
	
	  if (abs(u8Temp - ADCKEY_A1_VALUE)  <= KEY_TOL_1)
		  u8Keypad &= ~ADCKEY_A1;
		
	  if (abs(u8Temp1 - ADCKEY_B0_VALUE) <= KEY_TOL_0)
		  u8Keypad &= ~ADCKEY_B0;
	
	  if (abs(u8Temp1 - ADCKEY_B1_VALUE) <= KEY_TOL_1)
		  u8Keypad &= ~ADCKEY_B1;

    // if (HW_POWER_KEY == 0)
     //     u8Keypad &= ~KEY_POWER;

	#endif
	*Value=u8Keypad;
	
	return TRUE;
}



static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
     printf("STM32F Keypad Online !!!\n");
	return TRUE;
}



static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{KEYPADCTL_KEYPADSTATUS,Key_GetKeypadStatus},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_Keypad_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_Keypad_Driver =
{   
	NULL,
	KEYPAD_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_Keypad_PatchTable
};

//===========================================================



