#include "Include.h"

#define hwSSD1306_I2C_SCL_Pin               // GPIO04
#define hwSSD1306_I2C_SDA_Pin               // GPIO05
#define Init_hwSSD1306_I2C_SCL_Pin()        InitPortBit(PORTB,6,OUTPUT_OPEN_DRAIN,0)
#define Init_hwSSD1306_I2C_SDA_Pin()        InitPortBit(PORTB,7,OUTPUT_OPEN_DRAIN,0)

#define SetSSD1306_i2c_SDA()               SetPortBit(PORTB,7)
#define ClrSSD1306_i2c_SDA()               ClrPortBit(PORTB,7)
#define i2cSSD1306_SDAHi()                 GetPortBit(PORTB,7)
#define i2cSSD1306_SDALo()                 (!i2cSSD1306_SDAHi())
#define SetSSD1306_i2c_SCL()               SetPortBit(PORTB,6)
#define ClrSSD1306_i2c_SCL()               ClrPortBit(PORTB,6)
#define i2cSSD1306_SCLHi()                 GetPortBit(PORTB,6)
#define i2cSSD1306_SCLLo()                 (!i2cSSD1306_SCLHi())


void SSD1306_Delay4us( void )
{
	Delay1Us(4);

}


// Setup i2c Start condition
bool SSD1306_i2c_Start( void )
{
    SetSSD1306_i2c_SDA();
    SetSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    if( i2cSSD1306_SDALo() || i2cSSD1306_SCLLo() )
        return FALSE;
    ClrSSD1306_i2c_SDA();
    SSD1306_Delay4us();
    ClrSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void SSD1306_i2c_Stop( void )
{
    // SCL=L, SDA=L, Stop condition.
    ClrSSD1306_i2c_SCL();
    ClrSSD1306_i2c_SDA();
    SSD1306_Delay4us();
    SetSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    SetSSD1306_i2c_SDA();
    SSD1306_Delay4us();
}
//============================================
bool SSD1306_Wait_i2C_Ack( void )
{
    BYTE i;
    for( i = 0; i < 5; i++ )
    {    	
        SSD1306_Delay4us();
        if( i2cSSD1306_SDALo() )
            return TRUE;
    }

    if( i2cSSD1306_SDALo() )
        return TRUE;
    else
        return FALSE;

}
//============================================
bool SSD1306_i2c_SendByte( BYTE value )
{
    BYTE i;
    bool result;

    for( i = 0; i < 8; i++ ) // Send data via i2c pin
    {
        if( value & BIT7 )
            SetSSD1306_i2c_SDA();
        else
            ClrSSD1306_i2c_SDA();
        SSD1306_Delay4us();
        SetSSD1306_i2c_SCL();
        SSD1306_Delay4us();
        value <<= 1;
        ClrSSD1306_i2c_SCL();
    }
    SetSSD1306_i2c_SDA();
    result = SSD1306_Wait_i2C_Ack();
    SetSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    ClrSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    ClrSSD1306_i2c_SDA();

    return result;
}

//============================================
BYTE SSD1306_i2c_ReceiveByte(bool ack )
{
    BYTE i;
    BYTE value = 0;

    for( i = 0; i < 8; i++ )
    {
        value <<= 1;
        SetSSD1306_i2c_SDA();
        SSD1306_Delay4us();
        SetSSD1306_i2c_SCL();
        SSD1306_Delay4us();
        if( i2cSSD1306_SDAHi() )
            value |= BIT0;
        ClrSSD1306_i2c_SCL();
    }
    if( ack )
    {
        ClrSSD1306_i2c_SDA();
    }
    else
    {
        SetSSD1306_i2c_SDA();
    }
    SSD1306_Delay4us();
    SetSSD1306_i2c_SCL();
    SSD1306_Delay4us();
    ClrSSD1306_i2c_SCL();

    return value;
}

bool SSD1306_i2c_BurstWrite( BYTE count, BYTE *buffer )
{
    while( count-- )
    {
        if( SSD1306_i2c_SendByte( *( buffer++ ) ) == FALSE )
            return FALSE;
    }
    return TRUE;
}
bool SSD1306_i2c_BurstRead( BYTE count, BYTE * buffer )
{
    BYTE i;

    for( i = 0; i < count - 1; i++ )
        *( buffer + i ) = SSD1306_i2c_ReceiveByte( TRUE );
    *( buffer + i ) = SSD1306_i2c_ReceiveByte( FALSE );
	
    SSD1306_i2c_Stop();
    return TRUE;
}

bool SSD1306_i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    BYTE retry = 3;

    if( direct == I2C_READ ) // Set I2C direction bit.
        addr |= BIT0;
    else
        addr &= ~BIT0;

    while( retry-- )
    {
        if( SSD1306_i2c_Start() == FALSE )
        {
            SSD1306_i2c_Stop();
            continue;
        }

        if( SSD1306_i2c_SendByte( addr ) == TRUE ) // send address success
            return TRUE;
        SSD1306_i2c_Stop();
        Delay1Ms( 2 );
    }
    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SSD1306_I2C_Init()
{
	// i2cm_init(&hi2c1 , 100000);
	Init_hwSSD1306_I2C_SCL_Pin();
	Init_hwSSD1306_I2C_SDA_Pin();
	SetSSD1306_i2c_SDA();
	SetSSD1306_i2c_SCL();
}




