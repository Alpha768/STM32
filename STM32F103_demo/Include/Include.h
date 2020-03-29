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
#include "KEY.h"
#include "Remoter.h"
#include "Device.h"
#include "AES.h"
#include "lw_oopc.h"
//#include "./Lw_oopc/Light.h"
//#include "./Lw_oopc/Switch.h"
#include "oled.h"

#define USE_UART1
//#define USE_UART2


#endif
