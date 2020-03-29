#ifndef __PWM_H__
#define __PWM_H__

#define PWM_CH01 0x21//PA0---TIM2_CH1
#define PWM_CH02 0x22//PA1---TIM2_CH2
#define PWM_CH03 0x24//PA2---TIM2_CH3
#define PWM_CH04 0x28//PA3---TIM2_CH4
//-------------------------------------
#define PWM_CH05 0x41//PA6---TIM3_CH1
#define PWM_CH06 0x42//PA7---TIM3_CH2
#define PWM_CH07 0x44//PB0---TIM3_CH3
#define PWM_CH08 0x48//PB1---TIM3_CH4
//-------------------------------------
#define PWM_CH09 0x81//PB6---TIM4_CH1
#define PWM_CH10 0x82//PB7---TIM4_CH2
#define PWM_CH11 0x84//PB8---TIM4_CH3
#define PWM_CH12 0x88//PB9---TIM4_CH4
//==========================================
#define SOFT_PWM_CH01 0x00 //soft pwm freq <= 1000
#define SOFT_PWM_CH02 0x01//soft pwm freq <= 1000
#define SOFT_PWM_CH03 0x02//soft pwm freq <= 1000
#define SOFT_PWM_CH04 0x03//soft pwm freq <= 1000
#define SOFT_PWM_CH05 0x04//soft pwm freq <= 1000

typedef struct tagPwmIo{
	U8 Port ;
        U8 Bit;
}PwmIo;


#endif
