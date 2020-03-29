#include "Light.h"


static void init(void *t)
{
	Light *cthis = (Light*)t;
	
	printf("Light init\n");
	
	cthis->state = 0;

}

static S16 get_state(void *t)
{
	Light *cthis = (Light *)t;
	
	printf("Light get_state\n");
	
	return cthis->state;
}



static void set_light(void *t, S16 flag)
{
	Light * cthis = (Light*)t;
	cthis->state = flag;
	
	printf("Light set_light\n");

	
	if(cthis->state)
		SetPortBit(PORTC,8);//set PC7 green led
	else
		ClrPortBit(PORTC,8);//set PC7 green led
}

CTOR(Light)
FUNCTION_SETTING(init,init);
FUNCTION_SETTING(get_state,get_state);
FUNCTION_SETTING(set_light,set_light);
END_CTOR





