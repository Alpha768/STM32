#include "include.h"


static void ADCInit(void)
{
    RCC -> APB2ENR |= 1<<9;        //开启ADC1时钟
    RCC -> APB2RSTR |= 1<<9;       //复位ADC1
    RCC -> APB2RSTR &= ~(1<<9);    //ADC1复位结束
 
    RCC -> CFGR &= ~(3<<14);       //分频因子清零
    RCC -> CFGR |= 2<<14;          //设定分频因数为2,PCLK2 6分频后作为ADC时钟
 
    ADC1 -> CR1 &= 0xF0FFFF;     //工作模式清零
    ADC1 ->  CR1 |= 0<<16;         //设定为独立模式
    ADC1 -> CR1 &= ~(1<<8);            //非扫描工作模式
    ADC1 -> CR2 &= ~(1<<1);            //关闭连续转换
 
    ADC1 -> CR2 &= ~(7<<17);       //清除规则通道启动事件
    ADC1 -> CR2 |= 7<<17;          //设定规则通道启动事件为软件启动（SWSTART）
 
    ADC1 -> CR2 |= 1<<20;          //使用外部事件触发 SWSTART
    ADC1 -> CR2 &= ~(1<<11);       //设置对齐模式为右对齐
 
    ADC1 -> SQR1 &= ~(0xF<<20);        //清零规则序列的数量
    ADC1 -> SQR1 |= 15<<20;            //设置规则序列的数量为16
	  //ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 
 
    ADC1 -> SMPR2 &= 0x00000000; //清零通道采样时间
    ADC1 -> SMPR1 &= 0xFF000000;
 
    //if(ADC_CH_x <= 9 ){
    //    ADC1 -> SMPR2 |= 7<<(ADC_CH_x*3);          //设置通道x采样时间，提高采样时间可以提高采样精度
    //}
 
    //if(ADC_CH_x > 9 ){
    //    ADC1 -> SMPR1 |= 7<<((ADC_CH_x-10)*3);    
    //}
		ADC1 -> SMPR2=0x3fffffff;
		ADC1 -> SMPR1 |=0x00ffffff;
		//ADC1 -> SMPR2 |= 7<<(5*3);
		
    ADC1 -> CR2 |= 1<<0;           //开启AD转换
    ADC1 -> CR2 |= 1<<3;           //使能复位校准，由硬件清零

    while((ADC1 -> CR2)& (1<<3));  //等待校准结束

    ADC1 -> CR2 |= 1<<2;           //开启AD校准，由硬件清零

    while((ADC1 -> CR2)& (1<<2));  //等待校准结束
}

static U16 ADCValue_(U8 ADCIndex)// ADCIndex=0~16
{
    U16 data = 0;
    ADC1 -> SQR3 &= 0xFFFFFFE0;          //清除通道选择
    ADC1 -> SQR3 |= ADCIndex;                //选择通道
    ADC1 -> CR2  |= 1<<22;             //开启AD转换
    while(!(ADC1 -> SR & 1<<1));           //等待转换结束
    data = ADC1->DR;
    return data;
}

static U16 ADCValue(U8 ADCIndex)// ADCIndex=0~16
{
		U8 ucCnt = 0;
		U16 uwAdcBtmDrop = 9999;
		U16 uwAdcTopDrop = 0;
		U16 unAdcTotal = 0;
	  U16 uwAdcRslt;
    for(ucCnt = 0; ucCnt < 10; ucCnt++)
    {
        uwAdcRslt = ADCValue_(ADCIndex);           //采集AD
        if(uwAdcBtmDrop > uwAdcRslt)
        {
            uwAdcBtmDrop = uwAdcRslt;
        }
        if(uwAdcTopDrop < uwAdcRslt)
        {
            uwAdcTopDrop = uwAdcRslt;
        }
        unAdcTotal += uwAdcRslt;
    }
    //去掉最小值、最大值，然后求平均
    unAdcTotal -= uwAdcBtmDrop;
    unAdcTotal -= uwAdcTopDrop;
    unAdcTotal /= 8;
		return unAdcTotal;
}

//===========================================================================================

static U32 ADC_Convert(U32 cmd,va_list arg_ptr)
{
	U8 Index=va_arg(arg_ptr, VA_U8);
	U16 *Value=va_arg(arg_ptr, U16*);
	_PARAMETER_UNUSED_(cmd);
	*Value=ADCValue(Index);
	return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
  _PARAMETER_UNUSED_(arg_ptr);
	ADCInit();
  //PRINTF("STM32F ADC Online !!!\n",0);
	return TRUE;
}

//static U32 adc_init(U32 cmd,va_list arg_ptr)
//{
//  _PARAMETER_UNUSED_(cmd);
//	_PARAMETER_UNUSED_(arg_ptr);
//	ADCInit();
//	return TRUE;
//}


static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	//{ADCCTL_INIT,		adc_init},
	{ADCCTL_CONVERT,	ADC_Convert},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_ADC_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_ADC_Driver =
{   
	NULL,
	ADC_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_ADC_PatchTable
};

