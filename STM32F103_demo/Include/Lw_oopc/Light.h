#ifndef LIGHT_H
#define LIGHT_H

#include "Include.h"

typedef enum
{
  LED1,// PC6 Red
  LED2,// PC7 Green
  LED3,// PC8 Orange
  LED4,// PC9 Blue
  
  LED_MAX,
}LedType;



CLASS(Light)
{
	S16 led1_state;
	S16 led2_state;
	S16 led3_state;
	S16 led4_state;
	void (*init)(void *);
	S16 (*get_state)(void*,U8 u8LedType);
	void (*set_light)(void*,S16 flag,U8 u8LedType);	
};

#endif



