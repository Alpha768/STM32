#include "Include.h"

static U8 IRCodeMapToKeyTbl[]=
{
    IRKEY_POWER  , IR_KEY_POWER,
    IRKEY_RIGHT  , IR_KEY_PLUS,    //Dell Request
    IRKEY_LEFT   , IR_KEY_MINUS,
	IRKEY_UP	 , IR_KEY_PLUS,
	IRKEY_DOWN	 , IR_KEY_MINUS,
    IRKEY_SELECT , IR_KEY_MENU,    //kojona140514
    IRKEY_EXIT   , IR_KEY_TICK,	//kojona140514
    0xFF ,
};

static U8 GetIRCodeMapToKey(U8 u8inputkey )
{
    U8 i, u8temp;

    i = 0;
    u8temp = IR_KEY_NOTHING;
	
    while(IRCodeMapToKeyTbl[i] != 0xFF)
    {
        if(IRCodeMapToKeyTbl[i] == u8inputkey)
        {
            u8temp = IRCodeMapToKeyTbl[i+1]; 
            break;
        }
        i+=2;
    }

    return u8temp;
}

static U32 IR_GetIRKeypadStatus(U32 cmd,va_list arg_ptr)
{	
	IRMP_DATA irmp_data;
	U8 u8Temp=IR_KEY_NOTHING;
	U8 *Value=va_arg(arg_ptr, U8*);

	_PARAMETER_UNUSED_(cmd);

    if (irmp_get_data (&irmp_data))
    {					
      printf(" Address: 0x%.2X\n",irmp_data.address);
      printf(" Command: 0x%.2X\n",irmp_data.command);
      printf(" Flags: 0x%.2X\r\n",irmp_data.flags );										
    }

	if(irmp_data.address==IR_HEADER_CODE)
	{

		u8Temp=GetIRCodeMapToKey(irmp_data.command);
		irmp_data.address=0x00;
		irmp_data.command=0x00;
		irmp_data.flags=0x00;
	}

	*Value=~u8Temp;
	
	return TRUE;
}


static void timer2_init (void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 					// enable clock for TIM1

	printf("stm32_GetSYSCLK:%d\n",stm32_GetSYSCLK());
    TIM2->PSC = ((stm32_GetSYSCLK()/ F_INTERRUPTS)/8) - 1;// set prescaler  72MHz/(71+1)=1MHz
    TIM2->ARR = 7;// set auto-reload	(255+1)=256us
    TIM2->RCR = 0; // set repetition counter
    
    TIM2->CR1 = 0; // reset command register 1
    TIM2->CR2 = 0; // reset command register 2
    
    TIM2->DIER = TIM_IT_Update;							// enable interrupt
    NVIC->ISER[0]  = (1 << (TIM2_IRQChannel & 0x1F));  // enable interrupt
    TIM2->CR1 |= TIMX_CR1_CEN;							  // enable timer

	
}

void TIM2_IRQHandler(void)                                                  // Timer2 Interrupt Handler
{
 // TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
 
   TIM2->SR = (uint16_t)~TIM_IT_Update;
  (void) irmp_ISR();                                                        // call irmp ISR
  // call other timer interrupt routines...
}

static U32 InitIR(void)
{
	timer2_init();
    irmp_init();                                                            // initialize irmp
	return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
     printf("STM32F IR Online !!!\n");
	 InitIR();
	return TRUE;
}



static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE, CheckOnline},
	{IRCTL_IRKEYPADSTATUS,IR_GetIRKeypadStatus},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_IR_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_IR_Driver =
{   
	NULL,
	IR_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_IR_PatchTable
};

//===========================================================



