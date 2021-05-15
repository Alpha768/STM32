/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbhw.c
 * Purpose: USB Hardware Layer Module for ST STM32F10x
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Added USB_ClearEPBuf
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

/* Double Buffering is not yet supported */

#include <stm32f10x_lib.h>                           /* stm32f10x definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbreg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"

#include "STM32_Reg.h"                                  // missing bit definitions
#include "IO.h"


#define EP_BUF_ADDR (sizeof(EP_BUF_DSCR)*USB_EP_NUM) /* Endpoint Buffer Start Address */

EP_BUF_DSCR *pBUF_DSCR = (EP_BUF_DSCR *)USB_PMA_ADDR;/* Pointer to Endpoint Buffer Descriptors */

U16 FreeBufAddr;                                     /* Endpoint Free Buffer Address */


/*
 *  Reset Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void EP_Reset (U32 EPNum) {
  U32 num, val;

  num = EPNum & 0x0F;
  val = EPxREG(num);
  if (EPNum & 0x80) {                       /* IN Endpoint */
    EPxREG(num) = val & (EP_MASK | EP_DTOG_TX);
  } else {                                  /* OUT Endpoint */
    EPxREG(num) = val & (EP_MASK | EP_DTOG_RX);
  }
}


/*
 *  Set Endpoint Status
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     stat: New Status
 *    Return Value:    None
 */

void EP_Status (U32 EPNum, U32 stat) {
  U32 num, val;

  num = EPNum & 0x0F;
  val = EPxREG(num);
  if (EPNum & 0x80) {                       /* IN Endpoint */
    EPxREG(num) = (val ^ (stat & EP_STAT_TX)) & (EP_MASK | EP_STAT_TX);
  } else {                                  /* OUT Endpoint */
    EPxREG(num) = (val ^ (stat & EP_STAT_RX)) & (EP_MASK | EP_STAT_RX);
  }
}


/*
 *  USB Interrupt enable
 *   Called by USB_Init to enable the USB Interrupt
 *    Return Value:    None
 */

static void USB_IntrEna (void) {
  NVIC->ISER[0] |= (1 << (USB_LP_CAN_RX0_IRQChannel & 0x1F));
}


/*
 *  USB Initialize Function
 *   Called by the User to initialize USB
 *    Return Value:    None
 */

void USB_Init (void) {

  /* Control USB connecting via SW */
#if 1
  //RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable clock for GPIOC
	//SetPortBit(PORTC,7);//set PC7
	//GPIOC->CRL&=0x0FFFFFFF;//
	//GPIOC->CRL|=0x30000000;//
	//InitPortBit(PORTA,11,AF_PUSH_PULL,0);
#else
	//RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                   // enable clock for GPIOC
  //GPIOC->BRR |=  (1<<13);                    /* reset PC13 */
  //GPIOC->CRH &= ~0x00F00000;                /* clear port PC13 */
  //GPIOC->CRH |=  0x00300000;                /* PC13 General purpose output push-pull, max speed 50 MHz */
#endif
 /// SetPortBit(PORTC,7);//Set PC7

  RCC->APB1ENR |= (1 << 23);                /* enable clock for USB */

  USB_IntrEna ();                           /* Enable USB Interrupts */
}


/*
 *  USB Connect Function
 *   Called by the User to Connect/Disconnect USB
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USB_Connect (BOOL con) {

#if 1
  if (con) {
    ClrPortBit(PORTC,13);//Clr PC7
    CNTR = CNTR_FRES;                             /* Force USB Reset */
    CNTR = 0;
    ISTR = 0;                                     /* Clear Interrupt Status */
    CNTR = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM; /* USB Interrupt Mask */
  } else {
    CNTR = CNTR_FRES | CNTR_PDWN;                 /* Switch Off USB Device */
    SetPortBit(PORTC,13);//Set PC7
  }
#else
	  if (con) {
    //GPIOC->BRR |= 1<<13;                          /* reset PC13 */
    CNTR = CNTR_FRES;                             /* Force USB Reset */
    CNTR = 0;
    ISTR = 0;                                     /* Clear Interrupt Status */
    CNTR = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM; /* USB Interrupt Mask */
  } else {
    CNTR = CNTR_FRES | CNTR_PDWN;                 /* Switch Off USB Device */
		//GPIOC->BSRR |= 1<<13;                         /* set PC13 */
  }
#endif
}


/*
 *  USB Reset Function
 *   Called automatically on USB Reset
 *    Return Value:    None
 */

void USB_Reset (void) {
/* Double Buffering is not yet supported */

  ISTR = 0;                                 /* Clear Interrupt Status */

  CNTR = CNTR_CTRM | CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM |
         (USB_ERROR_EVENT   ? CNTR_ERRM    : 0) |
         (USB_ERROR_EVENT   ? CNTR_PMAOVRM : 0) |
         (USB_SOF_EVENT     ? CNTR_SOFM    : 0) |
         (USB_SOF_EVENT     ? CNTR_ESOFM   : 0);

  FreeBufAddr = EP_BUF_ADDR;

  BTABLE = 0x00;                            /* set BTABLE Address */

  /* Setup Control Endpoint 0 */
  pBUF_DSCR->ADDR_TX = FreeBufAddr;
  FreeBufAddr += USB_MAX_PACKET0;
  pBUF_DSCR->ADDR_RX = FreeBufAddr;
  FreeBufAddr += USB_MAX_PACKET0;
  if (USB_MAX_PACKET0 > 62) {
    pBUF_DSCR->COUNT_RX = ((USB_MAX_PACKET0 << 5) - 1) | 0x8000;
  } else {
    pBUF_DSCR->COUNT_RX =   USB_MAX_PACKET0 << 9;
  }
  EPxREG(0) = EP_CONTROL | EP_RX_VALID;

  DADDR = DADDR_EF | 0;                     /* Enable USB Default Address */
}


/*
 *  USB Suspend Function
 *   Called automatically on USB Suspend
 *    Return Value:    None
 */

void USB_Suspend (void) {
  CNTR |= CNTR_FSUSP;                       /* Force Suspend */
  CNTR |= CNTR_LPMODE;                      /* Low Power Mode */
}


/*
 *  USB Resume Function
 *   Called automatically on USB Resume
 *    Return Value:    None
 */

void USB_Resume (void) {
  /* Performed by Hardware */
}


/*
 *  USB Remote Wakeup Function
 *   Called automatically on USB Remote Wakeup
 *    Return Value:    None
 */

void USB_WakeUp (void) {
  CNTR &= ~CNTR_FSUSP;                      /* Clear Suspend */
}


/*
 *  USB Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Enable/Disable
 *    Return Value:    None
 */

void USB_WakeUpCfg (BOOL cfg) {
  /* Not needed */
}


/*
 *  USB Set Address Function
 *    Parameters:      adr:   USB Address
 *    Return Value:    None
 */

void USB_SetAddress (U32 adr) {
  DADDR = DADDR_EF | adr;
}


/*
 *  USB Configure Function
 *    Parameters:      cfg:   Configure/Deconfigure
 *    Return Value:    None
 */

void USB_Configure (BOOL cfg) {
  if (cfg == __FALSE) {
      FreeBufAddr  = EP_BUF_ADDR;
      FreeBufAddr += 2*USB_MAX_PACKET0;         /* reset Buffer address */
  }
}


/*
 *  Configure USB Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Endpoint Descriptor
 *    Return Value:    None
 */

void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR *pEPD) {
/* Double Buffering is not yet supported */
  U32 num, val;

  num = pEPD->bEndpointAddress & 0x0F;

  val = pEPD->wMaxPacketSize;
  if (pEPD->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
    (pBUF_DSCR + num)->ADDR_TX = FreeBufAddr;
    val = (val + 1) & ~1;
  } else {
    (pBUF_DSCR + num)->ADDR_RX = FreeBufAddr;
    if (val > 62) {
      val = (val + 31) & ~31;
      (pBUF_DSCR + num)->COUNT_RX = ((val << 5) - 1) | 0x8000;
    } else {
      val = (val + 1)  & ~1;
      (pBUF_DSCR + num)->COUNT_RX =   val << 9;
    }
  }
  FreeBufAddr += val;

  switch (pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
    case USB_ENDPOINT_TYPE_CONTROL:
      val = EP_CONTROL;
      break;
    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
      val = EP_ISOCHRONOUS;
      break;
    case USB_ENDPOINT_TYPE_BULK:
      val = EP_BULK;
      if (USB_DBL_BUF_EP & (1 << num)) {
        val |= EP_KIND;
      }
      break;
    case USB_ENDPOINT_TYPE_INTERRUPT:
      val = EP_INTERRUPT;
      break;
  }
  val |= num;
  EPxREG(num) = val;
}


/*
 *  Set Direction for USB Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USB_DirCtrlEP (U32 dir) {
  /* Not needed */
}


/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_EnableEP (U32 EPNum) {
  EP_Status(EPNum, EP_TX_NAK | EP_RX_VALID);   /* EP is able to receive */
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DisableEP (U32 EPNum) {
  EP_Status(EPNum, EP_TX_DIS | EP_RX_DIS);
}


/*
 *  Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ResetEP (U32 EPNum) {
  EP_Reset(EPNum);
}


/*
 *  Set Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_SetStallEP (U32 EPNum) {
  EP_Status(EPNum, EP_TX_STALL | EP_RX_STALL);
}


/*
 *  Clear Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClrStallEP (U32 EPNum) {
  EP_Reset(EPNum);                              /* reset DTog Bits */
  EP_Status(EPNum, EP_TX_VALID | EP_RX_VALID);
}


/*
 *  Clear USB Endpoint Buffer
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClearEPBuf (U32 EPNum) {
  ;
}


/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

U32 USB_ReadEP (U32 EPNum, U8 *pData) {
/* Double Buffering is not yet supported */
  U32 num, cnt, *pv, n;

  num = EPNum & 0x0F;

  pv  = (U32 *)(USB_PMA_ADDR + 2*((pBUF_DSCR + num)->ADDR_RX));
  cnt = (pBUF_DSCR + num)->COUNT_RX & EP_COUNT_MASK;
  for (n = 0; n < (cnt + 1) / 2; n++) {
    *((__packed U16 *)pData) = *pv++;
    pData += 2;
  }
  EP_Status(EPNum, EP_RX_VALID);

  return (cnt);
}


/*
 *  Write USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

U32 USB_WriteEP (U32 EPNum, U8 *pData, U32 cnt) {
/* Double Buffering is not yet supported */
  U32 num, *pv, n;
  U16 statusEP;

  num = EPNum & 0x0F;

  pv  = (U32 *)(USB_PMA_ADDR + 2*((pBUF_DSCR + num)->ADDR_TX));
  for (n = 0; n < (cnt + 1) / 2; n++) {
    *pv++ = *((__packed U16 *)pData);
    pData += 2;
  }
  (pBUF_DSCR + num)->COUNT_TX = cnt;

  statusEP = EPxREG(num);
  if ((statusEP & EP_STAT_TX) != EP_TX_STALL) {
    EP_Status(EPNum, EP_TX_VALID);                /* do not make EP valid if it is stalled previously */
	
  }
  return (cnt);
}


/*
 *  Get USB Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

U32 USB_GetFrame (void) {
  return (FNR & FNR_FN);
}


/*
 *  USB Interrupt Service Routine
 */

void USB_LP_CAN_RX0_IRQHandler (void) {
  U32 istr, num, val;

  istr = ISTR;

  /* USB Reset Request */
  if (istr & ISTR_RESET) {
    USB_Reset();
#if USB_RESET_EVENT
    USB_Reset_Event();
#endif
    ISTR = ~ISTR_RESET;
  }

  /* USB Suspend Request */
  if (istr & ISTR_SUSP) {
    USB_Suspend();
#if USB_SUSPEND_EVENT
    USB_Suspend_Event();
#endif
    ISTR = ~ISTR_SUSP;
  }

  /* USB Wakeup */
  if (istr & ISTR_WKUP) {
    USB_WakeUp();
#if USB_RESUME_EVENT
    USB_Resume_Event();
#endif
    ISTR = ~ISTR_WKUP;
  }

  /* Start of Frame */
  if (istr & ISTR_SOF) {
#if USB_SOF_EVENT
    USB_SOF_Event();
#endif
    ISTR = ~ISTR_SOF;
  }

#if USB_ERROR_EVENT

  /* PMA Over/underrun */
  if (istr & ISTR_PMAOVR) {
    USB_Error_Event(1);
    ISTR = ~ISTR_PMAOVR;
  }

  /* Error: No Answer, CRC Error, Bit Stuff Error, Frame Format Error */
  if (istr & ISTR_ERR) {
    USB_Error_Event(0);
    ISTR = ~ISTR_ERR;
  }

#endif

  /* Endpoint Interrupts */
  while ((istr = ISTR) & ISTR_CTR) {
    ISTR = ~ISTR_CTR;

    num = istr & ISTR_EP_ID;

    val = EPxREG(num);
    if (val & EP_CTR_RX) {
      EPxREG(num) = val & ~EP_CTR_RX & EP_MASK;
      if (USB_P_EP[num]) {
        if (val & EP_SETUP) {
          USB_P_EP[num](USB_EVT_SETUP);
        } else {
          USB_P_EP[num](USB_EVT_OUT);
        }
      }
    }
    if (val & EP_CTR_TX) {
      EPxREG(num) = val & ~EP_CTR_TX & EP_MASK;
      if (USB_P_EP[num]) {
        USB_P_EP[num](USB_EVT_IN);
      }
    }
  }

}
