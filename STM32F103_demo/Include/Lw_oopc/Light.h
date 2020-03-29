#ifndef LIGHT_H
#define LIGHT_H

#include "Include.h"

CLASS(Light)
{
	S16 state;
	void (*init)(void *);
	S16 (*get_state)(void*);
	void (*set_light)(void*,S16 flag);
};

#endif



