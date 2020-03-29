#ifndef __I2C_H__
#define __I2C_H__

#define NACK 0
#define ACK  1
#define I2C_BUS_ERROR 1
#define I2C_BUS_OK    0

#define I2C_BUS_MAX   2
extern void I2CInit(void);
extern U8 IicStart(void);
extern U8 IicStop( void );
extern U8 IicSendByte( U8 chr );
extern U8 IicReceiveByte(U8 ack);
#endif
