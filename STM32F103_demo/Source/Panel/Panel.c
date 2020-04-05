#include "Include.h"


static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
     printf("STM32F PANEL Online !!!\n");
	 
	return TRUE;
}



static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_PANEL_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_PANEL_Driver =
{   
	NULL,
	PANEL_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_PANEL_PatchTable
};

//===========================================================



