#include "Light.h"

//LED1 PC6
//LED2 PC7
//LED3 PC8
//LED4 PC9

#define LED_PIN PORTC
#define LED1_BIT 6
#define LED2_BIT 7
#define LED3_BIT 8
#define LED4_BIT 9

static void init(void *t)
{
	Light *cthis = (Light*)t;
	
	printf("Light init\n");
	
	cthis->led1_state = 0;
	cthis->led2_state = 0;
	cthis->led3_state = 0;
	cthis->led4_state = 0;

}

static S16 get_state(void *t,U8 u8LedType)
{
	Light *cthis = (Light *)t;
	
	if(u8LedType==LED1)
    	return cthis->led1_state;
	else if(u8LedType==LED2)
	    return cthis->led2_state;
	else if(u8LedType==LED3)
	    return cthis->led3_state;
	else if(u8LedType==LED4)
	    return cthis->led4_state;

	printf("Light get_state :%d\n",u8LedType);
	
	return 0;
}



static void set_light(void *t, S16 flag,U8 u8LedType)
{
	Light * cthis = (Light*)t;
	if(u8LedType==LED1)
	{
    	cthis->led1_state = flag;
    	printf("Light set_led1_light\n");	
    	if(cthis->led1_state)
    		SetPortBit(LED_PIN,LED1_BIT);//set PC7 green led
    	else
    		ClrPortBit(LED_PIN,LED1_BIT);//set PC7 green led
	}
	else if(u8LedType==LED2)
	{
    	cthis->led2_state = flag;
    	printf("Light set_led2_light\n");	
    	if(cthis->led2_state)
    		SetPortBit(LED_PIN,LED2_BIT);//set PC7 green led
    	else
    		ClrPortBit(LED_PIN,LED2_BIT);//set PC7 green led
	}
	else if(u8LedType==LED3)
	{
    	cthis->led3_state = flag;
    	printf("Light set_led3_light\n");	
    	if(cthis->led3_state)
    		SetPortBit(LED_PIN,LED3_BIT);//set PC7 green led
    	else
    		ClrPortBit(LED_PIN,LED3_BIT);//set PC7 green led
	}
	else if(u8LedType==LED4)
	{
    	cthis->led4_state = flag;
    	printf("Light set_led4_light\n");	
    	if(cthis->led4_state)
    		SetPortBit(LED_PIN,LED4_BIT);//set PC7 green led
    	else
    		ClrPortBit(LED_PIN,LED4_BIT);//set PC7 green led
	}
}

CTOR(Light)
FUNCTION_SETTING(init,init);
FUNCTION_SETTING(get_state,get_state);
FUNCTION_SETTING(set_light,set_light);
END_CTOR





