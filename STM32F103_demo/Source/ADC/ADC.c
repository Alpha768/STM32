#include "include.h"


static void ADCInit(void)
{
    RCC -> APB2ENR |= 1<<9;        //����ADC1ʱ��
    RCC -> APB2RSTR |= 1<<9;       //��λADC1
    RCC -> APB2RSTR &= ~(1<<9);    //ADC1��λ����
 
    RCC -> CFGR &= ~(3<<14);       //��Ƶ��������
    RCC -> CFGR |= 2<<14;          //�趨��Ƶ����Ϊ2,PCLK2 6��Ƶ����ΪADCʱ��
 
    ADC1 -> CR1 &= 0xF0FFFF;     //����ģʽ����
    ADC1 ->  CR1 |= 0<<16;         //�趨Ϊ����ģʽ
    ADC1 -> CR1 &= ~(1<<8);            //��ɨ�蹤��ģʽ
    ADC1 -> CR2 &= ~(1<<1);            //�ر�����ת��
 
    ADC1 -> CR2 &= ~(7<<17);       //�������ͨ�������¼�
    ADC1 -> CR2 |= 7<<17;          //�趨����ͨ�������¼�Ϊ���������SWSTART��
 
    ADC1 -> CR2 |= 1<<20;          //ʹ���ⲿ�¼����� SWSTART
    ADC1 -> CR2 &= ~(1<<11);       //���ö���ģʽΪ�Ҷ���
 
    ADC1 -> SQR1 &= ~(0xF<<20);        //����������е�����
    ADC1 -> SQR1 |= 15<<20;            //���ù������е�����Ϊ16
	  //ADC1->SQR1&=0<<20;     //1��ת���ڹ��������� Ҳ����ֻת����������1 
 
    ADC1 -> SMPR2 &= 0x00000000; //����ͨ������ʱ��
    ADC1 -> SMPR1 &= 0xFF000000;
 
    //if(ADC_CH_x <= 9 ){
    //    ADC1 -> SMPR2 |= 7<<(ADC_CH_x*3);          //����ͨ��x����ʱ�䣬��߲���ʱ�������߲�������
    //}
 
    //if(ADC_CH_x > 9 ){
    //    ADC1 -> SMPR1 |= 7<<((ADC_CH_x-10)*3);    
    //}
		ADC1 -> SMPR2=0x3fffffff;
		ADC1 -> SMPR1 |=0x00ffffff;
		//ADC1 -> SMPR2 |= 7<<(5*3);
		
    ADC1 -> CR2 |= 1<<0;           //����ADת��
    ADC1 -> CR2 |= 1<<3;           //ʹ�ܸ�λУ׼����Ӳ������

    while((ADC1 -> CR2)& (1<<3));  //�ȴ�У׼����

    ADC1 -> CR2 |= 1<<2;           //����ADУ׼����Ӳ������

    while((ADC1 -> CR2)& (1<<2));  //�ȴ�У׼����
}

static U16 ADCValue_(U8 ADCIndex)// ADCIndex=0~16
{
    U16 data = 0;
    ADC1 -> SQR3 &= 0xFFFFFFE0;          //���ͨ��ѡ��
    ADC1 -> SQR3 |= ADCIndex;                //ѡ��ͨ��
    ADC1 -> CR2  |= 1<<22;             //����ADת��
    while(!(ADC1 -> SR & 1<<1));           //�ȴ�ת������
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
        uwAdcRslt = ADCValue_(ADCIndex);           //�ɼ�AD
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
    //ȥ����Сֵ�����ֵ��Ȼ����ƽ��
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

