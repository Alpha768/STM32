#ifndef SWITCH_H
#define SWITCH_H

#include "Include.h"
#include "Light.h"

CLASS(Switch)
{
	S16 state;
	Light* light_obj;
	void (*init)(void*);
	S16 (*get_state)(void*);
	void (*set_switch)(void*);

};

#endif

