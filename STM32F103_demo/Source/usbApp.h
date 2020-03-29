#ifndef __USB_APP_H__
#define __USB_APP_H__

BOOLEAN ExeUsbFilterCMD(U8 type,U8 *buffer,U8 len);
BOOLEAN UsbPackageReceive(U8** p,U16 *len);
void UsbPackageTransmit(U8 *buffer,U16 len);
void USBCableCheck(void);

#endif
