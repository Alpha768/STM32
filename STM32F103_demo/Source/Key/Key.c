
#include "Include.h"


//=================================================================================
typedef U16 (*_FN_KEYVALUE_)(void);
_FN_KEYVALUE_  GetKeyValue=NULL;


static COMMAND_TABLE	MeCommandTable[] = 
{
	{ 0, 0, 0,0 } 
} ;

static COMMAND_TABLE *	CommandTable=MeCommandTable;



//#define		BASE_TIME				10


static U8	g_Command		;
// 系统
static BOOLEAN	f_RepeatCommand	;
static BOOLEAN	f_ReleaseCommand ;
static BOOLEAN  f_FirstCommand;
// 按键识别模块
static BOOLEAN	f_DisableKey	;
static BOOLEAN	f_KeyCommand	;
static BOOLEAN	f_KeyRepeat		;
static BOOLEAN	f_KeyDisableRepeat	;
static BOOLEAN  f_KeyRelease    ;
static U8	g_KeyCommand	;
static U8	m_KeyCounter	;
static U16	m_PrevKey		;
static U16	m_CurrKey		;
static U32	g_KeyPressTimer	;
static U32  g_KeyReleaseTimer;


static U32 InitKey(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(arg_ptr);
	_PARAMETER_UNUSED_(cmd);
	f_RepeatCommand=0;
	f_ReleaseCommand=0;
	f_FirstCommand=0;
	//g_SystemMode = 1;
	f_DisableKey = 1 ; 
	f_KeyCommand = 0 ;
	f_KeyRepeat = 0 ;
	f_KeyRelease=0;
	return TRUE;
}

	
static void KeyScan(void)
{
	if(GetKeyValue==NULL)
		m_CurrKey = KEY_NOTHING;
	else
		m_CurrKey = GetKeyValue();	
	
	//--------------------------------------------
	if(m_CurrKey)// for spec key
	{
		printf(" m_CurrKey=0x%x\n",m_CurrKey);
		m_CurrKey = m_CurrKey & 0xff;
		if(m_CurrKey != KEY_NOTHING)
		{
		
			g_KeyCommand = m_CurrKey;		
			f_KeyCommand = 1 ;				
			f_KeyRepeat = 0 ;				
			f_KeyDisableRepeat = 0 ;		
			g_KeyPressTimer = 0 ;			
			g_KeyReleaseTimer=0;
			m_KeyCounter = 0 ;							
			m_PrevKey = KEY_NOTHING ;
			f_KeyRelease = 0 ;
			
			printf("@@@ m_CurrKey=0x%x\n",m_CurrKey);
			return;
		}
	}
	//--------------------------------------------
		
	if( m_CurrKey == KEY_NOTHING )
	{
		f_DisableKey = 0 ;						// 解除CPU复位后的按键禁止
		//if(m_KeyCounter >= 100/BASE_TIME)
		//	f_KeyRelease=1;
		
		if( g_KeyReleaseTimer < 0xffffffff)
			g_KeyReleaseTimer++;
	}
	else if( m_CurrKey == m_PrevKey )		 	// 与上一次读到的键码相同
	{
	
    	printf(" f_DisableKey=0x%x\n",f_DisableKey);
		if( f_DisableKey == FALSE)					// 检查按键是否被禁止
		{
			if( ++m_KeyCounter == 100/BASE_TIME )		// 按键至少稳定100ms才有效
			{
				g_KeyCommand = m_CurrKey ;		// 将键码给命令变量g_KeyCommand
				f_KeyCommand = 1 ;				// 设置命令有效标志
				f_KeyRepeat = 0 ;				// 不是重复命令
				f_KeyDisableRepeat = 0 ;		// 允许按键重复
				g_KeyPressTimer = 0 ;			// 刚刚按键
			}
			else if( m_KeyCounter == 500/BASE_TIME )	// 按键至少稳定500ms才认为是重复键
			{
				if(  ! f_KeyDisableRepeat )		// 判断重复键是否被禁止(参见遥控解码)
					f_KeyRepeat = 1 ;			// 设置重复命令
				m_KeyCounter = (500-100)/BASE_TIME ;	// 重复键的重复间隔时间：100ms
				if( g_KeyPressTimer < 0xffffffff)		// 按键计时
					g_KeyPressTimer ++ ;
			}
			g_KeyReleaseTimer=0;
		}
		return;
	}

	
	if(m_KeyCounter >= 100/BASE_TIME)
		f_KeyRelease=1;
	
	// 检测到不同的按键或无按键
	m_KeyCounter = 0 ;							// 计数器清0
	m_PrevKey = m_CurrKey ;
	return;
}

U32 ProcessKey( U32 cmd,va_list arg_ptr )
{
	int Index ;
    _PARAMETER_UNUSED_(cmd);
	CommandTable=va_arg(arg_ptr, COMMAND_TABLE *);

	f_ReleaseCommand = 0 ;
	f_FirstCommand = 0;
	
	if(f_KeyCommand)
	printf("$$ f_KeyCommand=0x%x\n", f_KeyCommand);

	if(f_KeyRepeat)
	printf("## f_KeyCommand=0x%x\n", f_KeyRepeat);

	if(f_KeyRelease)
	printf("!! f_KeyCommand=0x%x\n", f_KeyRelease);

	
	if( f_KeyCommand )
	{
		g_Command = g_KeyCommand ;
		f_FirstCommand = 1;
		f_RepeatCommand = 0 ;
		f_KeyCommand = 0 ;
		if(g_Command)
		printf("Fir g_Command=0x%x\n", g_Command);
	}
	else if( f_KeyRepeat )
	{
		g_Command = g_KeyCommand ;
		f_RepeatCommand = 1 ;
		f_KeyRepeat = 0 ;
		if(g_Command)
		printf("Rep g_Command=0x%x\n", g_Command);
	}
	else if( f_KeyRelease )
	{
	
    	if(g_Command)
    	printf("Rel g_Command=0x%x\n", g_Command);
		g_Command = g_KeyCommand ;
		f_ReleaseCommand=1;
		f_KeyRelease=0;
	}
	else
		return TRUE;
	
	if( g_Command == KEY_NOTHING )
		return TRUE;
	
	Index = -1 ;
	
	while( 1 )
	{
		Index ++ ;	
		
		printf("Fir ModeMask=0x%x\n", CommandTable[Index].ModeMask & g_SystemMode);
		if( CommandTable[Index].DoCommand == 0 )
			break ;
		if( CommandTable[Index].CommandCode != g_Command )
			continue ;
		if( ( (CommandTable[Index].StatusMask & STATUS_FIRST) == 0 ) && f_FirstCommand )
			continue ;
		else if( ( (CommandTable[Index].StatusMask & STATUS_REPEAT) == 0 ) && f_RepeatCommand )
			continue ;
		else if( ( (CommandTable[Index].StatusMask & STATUS_RELEASE) == 0 ) && f_ReleaseCommand )
			continue ;
		//if( ( CommandTable[Index].ModeMask & g_SystemMode ) == 0 )
		//	continue ;
		
		CommandTable[Index].DoCommand() ;
	}
	f_FirstCommand=0;
	f_RepeatCommand=0;
	f_ReleaseCommand=0;
	g_Command = KEY_NOTHING ;
	return TRUE;
}

U32 KeyPressTime(U32 cmd,va_list arg_ptr)
{
	U16 * pTime=va_arg(arg_ptr, U16 *);
	*pTime=g_KeyPressTimer;
	_PARAMETER_UNUSED_(cmd);
	return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
	f_RepeatCommand=0;
	f_ReleaseCommand=0;
	f_FirstCommand=0;
	f_DisableKey = 0 ; 
	f_KeyCommand = 0 ;
	f_KeyRepeat = 0 ;
	f_KeyRelease=0;
	GetKeyValue=va_arg(arg_ptr, _FN_KEYVALUE_);
	QueueTimerCreate(10,TRUE,TRUE,FALSE,KeyScan);
	
	printf("STM32F Key Online !!!\n");
	return TRUE;
}

//===========================================================================

static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{KEYCTL_CLEAN,			InitKey},
	{KEYCTL_PROCESS,		ProcessKey},
	{KEYCTL_TIME,           KeyPressTime},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE KeyBoard_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};

DEVICE f_KeyBoard_Driver =
{   
	NULL,
	KEY_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	KeyBoard_PatchTable
};
