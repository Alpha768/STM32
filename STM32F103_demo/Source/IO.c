
#include "Include.h"

GPIO_TypeDef * PortMap[]={GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG};

BOOLEAN SetGpioValue(U8 Port,U8 Bit,BOOLEAN BitVal) 
{
	GPIO_TypeDef * p;
	if(Port>=COUNTOF(PortMap))
		return FALSE;
	p=PortMap[Port];
	if(BitVal)
		p->BSRR |= 1<<Bit;
	else
		p->BRR |= 1<<Bit;
	return TRUE;
}

BOOLEAN SetPortBit(U8 Port,U8 Bit) 
{
	GPIO_TypeDef * p;
	if(Port>=COUNTOF(PortMap))
		return FALSE;
	p=PortMap[Port];
	p->BSRR |= 1<<Bit;
	return TRUE;
}
BOOLEAN ClrPortBit(U8 Port,U8 Bit)
{
	GPIO_TypeDef * p;
	if(Port>=COUNTOF(PortMap))
		return FALSE;
	p=PortMap[Port];
	p->BRR |= 1<<Bit;
	return TRUE;
}
BOOLEAN GetPortBit(U8 Port,U8 Bit) 
{
	U16 Value;
	GPIO_TypeDef * p;
	if(Port>=COUNTOF(PortMap))
		return FALSE;
	p=PortMap[Port];
	Value=p->IDR;
	Value=Value&(1<<Bit);
	if(Value)
		return TRUE;
	return FALSE;
}

#define SPEED_10MHz 0x01
#define SPEED_2MHz  0x02
#define SPEED_50MHz 0x03

BOOLEAN InitPortBit(U8 Port,U8 Bit,U8 Config,BOOLEAN BitVal)
{
	U8 Offset;
	U32 Val=0;
	U32 Cfg;
	GPIO_TypeDef * p;
	if(Bit>15)
		return FALSE;
	if(Port>=COUNTOF(PortMap))
		return FALSE;
	p=PortMap[Port];
	
	if(Port==PORTA)
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable clock for GPIOA
	if(Port==PORTB)
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // enable clock for GPIOB
	if(Port==PORTC)
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable clock for GPIOC
	if(Port==PORTD)
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN; // enable clock for GPIOD
	if(Port==PORTE)
		RCC->APB2ENR |= RCC_APB2ENR_IOPEEN; // enable clock for GPIOE
	if(Port==PORTF)
		RCC->APB2ENR |= RCC_APB2ENR_IOPFEN; // enable clock for GPIOF
	if(Port==PORTG)
		RCC->APB2ENR |= RCC_APB2ENR_IOPGEN; // enable clock for GPIOG
	
	if(Config == AF_PUSH_PULL || Config == AF_OPEN_DRAIN)
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	if(Config == OUTPUT_PUSH_PULL)
		Val=0x00|SPEED_50MHz;
	if(Config == OUTPUT_OPEN_DRAIN)
	  Val=0x04|SPEED_50MHz;
	if(Config == AF_PUSH_PULL)
		Val=0x08|SPEED_50MHz;
	if(Config == AF_OPEN_DRAIN)
		Val=0x0C|SPEED_50MHz;
	//----------------------------
	if(Config == INPUT_ANALOG)
		Val=0x00;
	if(Config == INPUT_FLOATING)
		Val=0x04;
	if(Config == INPUT_PULL_DOWN || Config == INPUT_PULL_UP)
		Val=0x08;
	
	if(Bit<8)
	{
		Cfg=p->CRL;
		Offset=Bit;
		Cfg&=(~(0xF<<(Offset<<2)));//Cfg&=(~(0xF<<(Offset*4)));
		Cfg|=(Val<<(Offset<<2));//Cfg|=(Val<<(Offset*4));
		p->CRL=Cfg;
	}
	else
	{
		Cfg=p->CRH;
		Offset=Bit-8;
		Cfg&=(~(0xF<<(Offset<<2)));//Cfg&=(~(0xF<<(Offset*4)));
		Cfg|=(Val<<(Offset<<2));//Cfg|=(Val<<(Offset*4));
		p->CRH=Cfg;
	}
	
	Val=p->ODR;
	Val=Val&(~(1<<Bit));
	if(BitVal)
		Val|=(1<<Bit);
	p->ODR=Val;
	
	return TRUE;
}

//######################################################################
void InitGPIO(void)
{
	//-------------------------------
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable clock for GPIOA
	
	//GPIOA->CRL&=0xFF0FFFFF;// set PA5 for ADC input
	//InitPortBit(PORTA,5,INPUT_ANALOG,1);// set PA5 for ADC input
	
	//RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // enable clock for GPIOB
	//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	//AFIO->MAPR &= 0xfaffffff;//----------------重映射JTAG的JTDO,JNTRST,解决PB3.PB4被JTAG占用的问题
	//AFIO->MAPR |= 0x02000000;
	//DBGMCU->CR &= 0xFFFFFFDF;
	
	//RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable clock for GPIOC
	//for USB Cable Check
	//GPIOC->CRL&=0xF0FFFFFF;
	//GPIOC->CRL|=0x08000000;

	
	InitPortBit(PORTC,6,OUTPUT_PUSH_PULL,0);
	InitPortBit(PORTC,7,OUTPUT_PUSH_PULL,0);
	InitPortBit(PORTC,8,OUTPUT_PUSH_PULL,0);	
	InitPortBit(PORTC,9,OUTPUT_PUSH_PULL,0);
}

static unsigned char f_Flicker=0;


void LedFlicker(void)
{

	
	//printf("LedFlicker 0 :%d",f_Flicker);
	f_Flicker=~f_Flicker;
	//printf("LedFlicker  :%d\n",f_Flicker);
	if(f_Flicker)	
	{
		SetPortBit(PORTC,9);//set PC7 green led
		//printf("LedFlicker SetPortBit\n");
	}
	else
	{
		ClrPortBit(PORTC,9);//Clr PC7 green led
		//printf("LedFlicker ClrPortBit\n");
	}
	
}

