
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
// ϵͳ
static BOOLEAN	f_RepeatCommand	;
static BOOLEAN	f_ReleaseCommand ;
static BOOLEAN  f_FirstCommand;
// ����ʶ��ģ��
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
		m_CurrKey = NO_COMMAND;
	else
		m_CurrKey = GetKeyValue() ;
	//--------------------------------------------
	if(m_CurrKey & 0xff00)// for spec key
	{
		m_CurrKey = m_CurrKey & 0xff;
		if(m_CurrKey != NO_COMMAND)
		{
			g_KeyCommand = m_CurrKey;		
			f_KeyCommand = 1 ;				
			f_KeyRepeat = 0 ;				
			f_KeyDisableRepeat = 0 ;		
			g_KeyPressTimer = 0 ;			
			g_KeyReleaseTimer=0;
			m_KeyCounter = 0 ;							
			m_PrevKey = NO_COMMAND ;
			f_KeyRelease = 0 ;
			return;
		}
	}
	//--------------------------------------------
		
	if( m_CurrKey == NO_COMMAND )
	{
		f_DisableKey = 0 ;						// ���CPU��λ��İ�����ֹ
		//if(m_KeyCounter >= 100/BASE_TIME)
		//	f_KeyRelease=1;
		
		if( g_KeyReleaseTimer < 0xffffffff)
			g_KeyReleaseTimer++;
	}
	else if( m_CurrKey == m_PrevKey )		 	// ����һ�ζ����ļ�����ͬ
	{
		if( f_DisableKey == FALSE)					// ��鰴���Ƿ񱻽�ֹ
		{
			if( ++m_KeyCounter == 100/BASE_TIME )		// ���������ȶ�100ms����Ч
			{
				g_KeyCommand = m_CurrKey ;		// ��������������g_KeyCommand
				f_KeyCommand = 1 ;				// ����������Ч��־
				f_KeyRepeat = 0 ;				// �����ظ�����
				f_KeyDisableRepeat = 0 ;		// �������ظ�
				g_KeyPressTimer = 0 ;			// �ոհ���
			}
			else if( m_KeyCounter == 500/BASE_TIME )	// ���������ȶ�500ms����Ϊ���ظ���
			{
				if(  ! f_KeyDisableRepeat )		// �ж��ظ����Ƿ񱻽�ֹ(�μ�ң�ؽ���)
					f_KeyRepeat = 1 ;			// �����ظ�����
				m_KeyCounter = (500-100)/BASE_TIME ;	// �ظ������ظ����ʱ�䣺100ms
				if( g_KeyPressTimer < 0xffffffff)		// ������ʱ
					g_KeyPressTimer ++ ;
			}
			g_KeyReleaseTimer=0;
		}
		return;
	}
	if(m_KeyCounter >= 100/BASE_TIME)
		f_KeyRelease=1;
	// ��⵽��ͬ�İ������ް���
	m_KeyCounter = 0 ;							// ��������0
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
	
	if( f_KeyCommand )
	{
		g_Command = g_KeyCommand ;
		f_FirstCommand = 1;
		f_RepeatCommand = 0 ;
		f_KeyCommand = 0 ;
		//PRINTF("Fir g_Command=%c\n", &g_Command);
	}
	else if( f_KeyRepeat )
	{
		g_Command = g_KeyCommand ;
		f_RepeatCommand = 1 ;
		f_KeyRepeat = 0 ;
		//PRINTF("Rep g_Command=%c\n", &g_Command);
	}
	else if( f_KeyRelease )
	{
		g_Command = g_KeyCommand ;
		f_ReleaseCommand=1;
		f_KeyRelease=0;
	}
	else
		return TRUE;
	
	if( g_Command == NO_COMMAND )
		return TRUE;
	
	Index = -1 ;
	while( 1 )
	{
		Index ++ ;
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
		if( ( CommandTable[Index].ModeMask & g_SystemMode ) == 0 )
			continue ;
		CommandTable[Index].DoCommand() ;
	}
	f_FirstCommand=0;
	f_RepeatCommand=0;
	f_ReleaseCommand=0;
	g_Command = NO_COMMAND ;
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
