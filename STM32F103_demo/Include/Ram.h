#ifndef __RAM_H__
#define __RAM_H__

#ifdef	MAP_VARS_HERE
	#define EXTERN
#else
	#define EXTERN	extern
#endif

#define USB_BUFFER_SIZE 512
 
EXTERN U8 USBRevRingBuffer[USB_BUFFER_SIZE+13];
EXTERN U8 USBTranRingBuffer[USB_BUFFER_SIZE+13];
EXTERN RingBuffer_t USBReceiveBuffer;
EXTERN RingBuffer_t USBTransmitBuffer;

EXTERN U8  UsbRevTimerIndex;
EXTERN U16 UsbDataCnt;
EXTERN BOOLEAN ExeUsbCmdStatus;
EXTERN BOOLEAN ExeUartCmdBusy;
#define STATUS_IDLE		0
#define STATUS_BUSY		1
#define STATUS_ERROR	2
#define STATUS_SUCCESS	3

//EXTERN U32 UsrDataCRC32;

#define BASE_TIME 10

EXTERN U8	g_SystemMode	;
	#define	MODE_NORMAL	  0x0001
	#define	MODE_STANBY		0x0002
	#define MODE_ALL  (MODE_NORMAL|MODE_STANBY)


//EXTERN U32 m_BaseTick;
//EXTERN BOOLEAN f_1000_ms;
//EXTERN BOOLEAN f_200_ms;

EXTERN U16 g_Current;

//EXTERN BOOLEAN f_LedOnOff;
EXTERN BOOLEAN f_USBCableNew;
EXTERN BOOLEAN f_USBCableOld;
EXTERN BOOLEAN f_PatternOnOff;


EXTERN BOOLEAN f_USB_Error;
#endif
