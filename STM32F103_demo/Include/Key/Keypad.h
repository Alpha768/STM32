#ifndef __KEYPAD_H__
#define __KEYPAD_H__

typedef struct
{
	U8 u8SarCH;
	U8 u8KeypadData;
	U8 u8KeypadDataTol;
	U8 u8KeypadMask;
}KeypadDataType;

//SAR SELECT
#define KEYPAD_ADC_A        ADC_CH10
#define KEYPAD_ADC_B        ADC_CH11

//SAR_KEY_VALUE
#define ADCKEY_A0_VALUE   0x5C
#define ADCKEY_A1_VALUE   0x34

#define ADCKEY_B0_VALUE   0x5C
#define ADCKEY_B1_VALUE   0x34

#define ADCKEY_A0   KEY_PLUS
#define ADCKEY_A1   KEY_TICK
#define ADCKEY_B0   KEY_MINUS
#define ADCKEY_B1   KEY_MENU

#define KEY_TOL_0   4
#define KEY_TOL_1   4


#endif
