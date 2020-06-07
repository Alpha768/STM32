#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <stm32f10x_lib.h>                  /* stm32f10x definitions */
#include "STM32_Reg.h"                                  // missing bit definitions

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "type.h"
#include "config.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"
#include "STM32_Init.h"                     /* stm32 initialisation */

#include "MCU.h"
#include "usbApp.h"
#include "IAP.h"
#include "Queue.h"
#include "serial.h"

#include "CRC32.h"
#include "CRC16.h"
#include "Time.h"
#include "SPI.h"
#include "PWM.h"
#include "command.h"
#include "Store.h"
#include "IO.h"
#include "ADC.h"
#include "Ram.h"
#include "KeyDef.h"
#include "Key.h"
#include "Keypad.h"
#include "Remoter.h"
#include "Panel.h"
#include "Device.h"
#include "AES.h"
#include "lw_oopc.h"
//#include "./Lw_oopc/Light.h"
//#include "./Lw_oopc/Switch.h"
//#include "oled.h"
#include "irmp.h"
#include "IR.h"
//#include "I2C.h"
//#include "ssd1306.h"
#include "fonts.h"
#include "display.h"
#include "ssd1306_i2c.h"
#include "ssd1306_drv.h"

#define USE_UART1
//#define USE_UART2

#define ENABLE_IR


typedef enum
{
    eBOOT_STATUS_NONE,  // Not valid
    eBOOT_STATUS_ACON,  // AC
    eBOOT_STATUS_DCOFF, // AC -> enter DC off
    eBOOT_STATUS_DCON,  // DC wakeup
    eBOOT_STATUS_DPMSON,// DPMS wakeup
}eBOOT_STATUS_TYPE;


#endif
