#ifndef __KEY_DEF_H__
#define __KEY_DEF_H__

typedef struct {
	U8	CommandCode ;
	U8	StatusMask;
	U16	ModeMask;
	void ( *DoCommand)( void ) ;
} COMMAND_TABLE ;

typedef enum
{
  KEY_POWER     =BIT0, // power
  KEY_MENU      =BIT1, // menu
  KEY_PLUS      =BIT2, // right/up
  KEY_MINUS     =BIT3, // left/down
  KEY_EXIT      =BIT4, // tick
  KEY_TICK      =KEY_EXIT,
  KEY_NOTHING   =0
}
KeypadMaskType;


#define KEYPAD_MASK (KEY_POWER|KEY_PLUS|KEY_MINUS|KEY_MENU|KEY_TICK)

#ifndef SARADC_MASK
#define SARADC_MASK 0xFF//0x3F    //for new IC setting
#endif

//Key status
#define STATUS_NULL     0x00
#define STATUS_FIRST    0x01
#define STATUS_REPEAT   0x02
#define STATUS_RELEASE  0x04
#define STATUS_ALL      (STATUS_FIRST|STATUS_REPEAT|STATUS_RELEASE)

#define TYPE_NORMAL_SWITCH 0x100
#define TYPE_ROTARY_SWITCH 0x200

#endif

