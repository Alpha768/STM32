#ifndef __I2C_H__
#define __I2C_H__
#include "type.h"

#define NACK 0
#define ACK  1
#define I2C_BUS_ERROR 1
#define I2C_BUS_OK    0

#define I2C_BUS_MAX   2

#define IIC_EEPROM_ID 0xA0

#define IIC_START  				1
#define IIC_STOP   				2
#define IIC_SEND_BYTE			3
#define IIC_RECEIVE_LAST_ACK	4
#define IIC_RECEIVE_LAST_NACK	5

typedef enum i2c_direction
{
    I2C_WRITE, I2C_READ
}
I2C_Direction;


extern void IicSetSpeed(U16 kHz);
extern void I2CInit(void);
extern U8 IicStart(void);
extern U8 IicStop( void );
extern U8 IicSendByte( U8 chr );
extern U8 IicSendBusrtByte(U8 *pbuf, U16 len);

extern U8 IicReceiveByte(U8 ack);
extern void IicWriteByte( U8 SlaveAddr, U8 RegAddr, U8 RegData );
extern U8 IicReadByte( U8 SlaveAddr, U8 RegAddr );
extern U8 IicMasterStart( I2C_Direction direct, U8 addr );

//extern void IicPageWrite( U8 SlaveAddr, U8 RegAddr, U8 *buf ,U16 count);
//extern void IicPageRead( U8 SlaveAddr, U8 RegAddr,U8 *buf,U16 count );
//--------------------------------------------------------------------
//extern void  AT24C32_Write_Block(U8 SlaveAddr, U16 RegAddr,U8 Size, U8 *Buffer);
//extern void	AT24C32_Read_Block(U8 SlaveAddr, U16 RegAddr,U8 Size,U8 *Buffer);
//extern void AT24C32_WriteBytes(U32 Addr,U32 Size, U8 *Buffer);
//extern void AT24C32_ReadBytes(U32 Addr,U32 Size, U8 *Buffer);
//#define EEPROM_WriteBlock    AT24C32_Write_Block
//#define EEPROM_ReadBlock   AT24C32_Read_Block
//#define EEPROM_WriteBlock(Addr,Size, Buffer)  IicPageWrite(0xA0,Addr,Buffer,Size)
//#define EEPROM_ReadBlock(Addr,Size, Buffer)   IicPageRead(0xA0,Addr,Buffer,Size)
//#define EEPROM_WriteBytes AT24C32_WriteBytes
//#define EEPROM_ReadBytes  AT24C32_ReadBytes
extern void I2CSoftInit(void);
#endif
