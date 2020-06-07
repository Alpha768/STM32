#ifndef __SSD1306_I2C_H__
#define __SSD1306_I2C_H__

#include "type.h"

typedef enum i2c_direction
{
    I2C_WRITE, I2C_READ
}
I2C_Direction;

extern bool SSD1306_i2c_Start( void );
extern void SSD1306_i2c_Stop( void );
extern bool SSD1306_Wait_i2C_Ack( void );
extern bool SSD1306_i2c_SendByte( BYTE value );
extern BYTE SSD1306_i2c_ReceiveByte(bool ack );
extern bool SSD1306_i2c_BurstWrite( BYTE count, BYTE *buffer );
extern bool SSD1306_i2c_BurstRead( BYTE count, BYTE * buffer );
extern bool SSD1306_i2c_MasterStart( I2C_Direction direct, BYTE addr );
extern void SSD1306_I2C_Init(void);

#endif

