#include "Switch.h"

static void init(void *t)
{
	Switch * cthis = (Switch *)t;
	
	printf("Switch init\n");
	
	cthis->state = 0;

}

static S16 get_state(void *t)
{
	Switch * cthis  = (Switch *)t;
	
	printf("Switch get_state\n");
	return cthis->state;
}

static void set_switch(void *t,U8 u8LedType)
{
	S16 st;
	Light *light;
	Switch * cthis = (Switch*)t;
	
	printf("Switch set_switch\n");
	cthis->state = !(cthis->state);
	light = cthis->light_obj;
	st = light->get_state(light,u8LedType);

	if(st == 1)
		light->set_light(light,0,u8LedType);
	else
		light->set_light(light,1,u8LedType);

}

CTOR(Switch)
FUNCTION_SETTING(init,init);
FUNCTION_SETTING(get_state,get_state);
FUNCTION_SETTING(set_switch,set_switch);
END_CTOR





