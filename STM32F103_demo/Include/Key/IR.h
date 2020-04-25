#ifndef __IR_H__
#define __IR_H__

// IR Header code define

#define IR_HEADER_CODE0     0xFF    // Custom 0
#define IR_HEADER_CODE1     0x00    // Custom 1
#define IR_HEADER_CODE      0xFF00    // Custom 1

typedef enum _IrCommandType
{
    IRKEY_UP                = 0x18,
    IRKEY_POWER             = 0x0D,
    IRKEY_EXIT              = 0x16,
    IRKEY_DOWN              = 0x52,
    IRKEY_LEFT              = 0x08,
    IRKEY_SELECT            = 0x1C, // IRKEY OK
    IRKEY_RIGHT             = 0x5A,

    IRKEY_NUM_0             = 0x19,
    IRKEY_NUM_1             = 0x45,
    IRKEY_NUM_2             = 0x46,
    IRKEY_NUM_3             = 0x47,
    IRKEY_NUM_4             = 0x44,
    IRKEY_NUM_5             = 0x40,
    IRKEY_NUM_6             = 0x43,
    IRKEY_NUM_7             = 0x07,
    IRKEY_NUM_8             = 0x15,
    IRKEY_NUM_9             = 0x09,
    
}IrCommandType;

typedef enum
{
  IR_KEY_POWER     =BIT0, // power
  IR_KEY_MENU      =BIT1, // menu
  IR_KEY_PLUS      =BIT2, // right/up
  IR_KEY_MINUS     =BIT3, // left/down
  IR_KEY_TICK      =BIT4, // tick
  IR_KEY_NOTHING   =0
} IRKeypadType;


#endif
