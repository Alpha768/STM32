#ifndef __COMMAND_H__
#define __COMMAND_H__

//command id
#define USB_CMD_STATUS				0x00
#define USB_CMD_CURRENT				0x01
#define USB_CMD_MAX_CURRENT   0x02
#define USB_CMD_MIN_CURRENT   0x03

//command iap id
#define USB_CMD_RUN_BOOT      0xf9
#define USB_CMD_BUFFER_SIZE		0xfa
#define USB_CMD_WRITE         0xfb
#define USB_CMD_READ          0xfc
#define USB_CMD_RUN_APP       0xfd
#define USB_CMD_MAGIC         0xfe


void ExeUartCMD(void);
void ExeUsbCMD(void);
void IdleCallBack(void);
void ExeUsrCMD( void );
void ExeSysCMD( void );
BOOLEAN ExeUsbFilterCMD(U8 type,U8 *buffer,U8 len);
void Effect_Usr_Cfg(void);
void UsbRevTimerCallBack(void);

#endif
