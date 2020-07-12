#ifndef __IO_H__
#define __IO_H__

#define PORTA		0
#define PORTB		1
#define PORTC		2
#define PORTD		3
#define PORTE		4
#define PORTF   5
#define PORTG		6

#define OUTPUT_PUSH_PULL  		0
#define OUTPUT_OPEN_DRAIN 		1
#define AF_PUSH_PULL 					2 //Alternate  Function 
#define AF_OPEN_DRAIN  				3 //Alternate  Function 
#define INPUT_ANALOG					4
#define INPUT_FLOATING				5
#define INPUT_PULL_DOWN				6
#define INPUT_PULL_UP					7


BOOLEAN SetGpioValue(U8 Port,U8 Bit,BOOLEAN BitVal);
BOOLEAN SetPortBit(U8 Port,U8 Bit);
BOOLEAN ClrPortBit(U8 Port,U8 Bit);
BOOLEAN GetPortBit(U8 Port,U8 Bit);
BOOLEAN InitPortBit(U8 Port,U8 Bit,U8 Config,BOOLEAN BitVal);
void InitGPIO(void);
void LedFlicker(void);
void Power(unsigned char OnOff);

#define IO_USB_CABLE  1//GetPortBit(PORTC,6)

#endif
