#ifndef __KEY_H__
#define __KEY_H__

typedef struct {
	U8	CommandCode ;
	U8	StatusMask;
	U16	ModeMask;
	void ( *DoCommand)( void ) ;
} COMMAND_TABLE ;

enum {
	//----------------------------------
	KEY_LEFT,
	KEY_RIGHT,
	KEY_POWER,
	SW_POWER_ON,
	SW_POWER_OFF,
	NO_COMMAND		= 0xFF
} ;

//Key status
#define STATUS_NULL     0x00
#define STATUS_FIRST    0x01
#define STATUS_REPEAT   0x02
#define STATUS_RELEASE  0x04
#define STATUS_ALL      (STATUS_FIRST|STATUS_REPEAT|STATUS_RELEASE)

#define TYPE_NORMAL_SWITCH 0x100
#define TYPE_ROTARY_SWITCH 0x200

#endif
