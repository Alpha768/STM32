#include "Include.h"

static U8  DataBuffer[USB_BUFFER_SIZE*2];

BOOLEAN ExeUsbFilterCMD(U8 type,U8 *buffer,U8 len)
{
	type=type;
	if(QueueDataCount(&USBReceiveBuffer)>0 || UsbDataCnt>0)
		return FALSE;
	if(buffer==NULL)
		return FALSE;
	if(len>=5 && buffer[0]==5 && buffer[1]==0)
	{
		if(buffer[2]==USB_CMD_STATUS)
		{
			if(CRC16(buffer,5))
			return FALSE;
			UsbPackageTransmit(&ExeUsbCmdStatus,1);
			if(ExeUsbCmdStatus!=STATUS_BUSY)
				ExeUsbCmdStatus = STATUS_IDLE;
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}


BOOLEAN UsbPackageReceive(U8** p,U16 *len)
{
	U16 Length;
	U16 Cnt;
	Cnt=QueueDataCount(&USBReceiveBuffer);
	
	if(Cnt)
		QueueTimerInterval(UsbRevTimerIndex,500);
	else
		return FALSE;
	
	if((Cnt+UsbDataCnt)<5)
	{
		QueueRead(&USBReceiveBuffer,&DataBuffer[UsbDataCnt],Cnt);
		UsbDataCnt=UsbDataCnt+Cnt;
		return FALSE;
	}
		
	if(UsbDataCnt==0)
	{
		QueueRead(&USBReceiveBuffer,DataBuffer,5);
		UsbDataCnt=5;
		Cnt=Cnt-5;
	}
		
	Length=DataBuffer[0] | (DataBuffer[1]<<8);
	if(Length>sizeof(DataBuffer))//To do
	{
		QueueRead(&USBReceiveBuffer,DataBuffer,Cnt);
		UsbDataCnt=0;
		return FALSE;
	}
			
	if((Cnt+UsbDataCnt)>=Length)
	{
		QueueRead(&USBReceiveBuffer,&DataBuffer[UsbDataCnt],Length-UsbDataCnt);
		UsbDataCnt=0;
		if(CRC16(DataBuffer,Length)==0)
		{
			*p=&DataBuffer[2];
			*len=Length-4;
			return TRUE;
		}
		else
			return FALSE;
	}
	QueueRead(&USBReceiveBuffer,&DataBuffer[UsbDataCnt],Cnt);
	UsbDataCnt=UsbDataCnt+Cnt;
	return FALSE;	
}

void UsbPackageTransmit(U8 *buffer,U16 len)
{
	U8 buf[516];
	U16 i,crc16,numBytes;
	if(512<len || buffer==NULL)
		return ;
	for(i=0;i<500;i++)
  {
		numBytes=QueueFreeCount(&USBTransmitBuffer);
		if(numBytes>=(len+4))
			break;
		Delay1Ms(1);
	}
	
	if(i==500)
		return ;
	
	buf[0]=(len+4)&0xff;
	buf[1]=((len+4)>>8)&0xff;
	for(i=0;i<len;i++)
	{
		buf[2+i]=buffer[i];
	}
	crc16=CRC16(buf,len+2);
	buf[len+2]=(crc16>>8)&0xff;
	buf[len+3]= crc16&0xff;
	
	numBytes=QueueDataCount(&USBTransmitBuffer);
	if(numBytes==0)
	{
		if(len<=60)
		USB_WriteEP (0x81, buf, len+4);
		else
		{
			QueueWrite(&USBTransmitBuffer,&buf[64],len+4-64);
			USB_WriteEP (0x81, buf, 64);
		}
	}
	else
		QueueWrite(&USBTransmitBuffer,buf,len+4);
}

void USBCableCheck(void)
{
		 f_USBCableNew=IO_USB_CABLE;
     if(f_USBCableNew != f_USBCableOld)
		 {
		  	if(f_USBCableNew==FALSE)
				{
						USB_Connect(0);
						USB_Configuration=0;
				}
				else
				{
						USB_Connect(1);
				}	
				f_USBCableOld=f_USBCableNew;
		 }
		 if(f_USB_Error)
		 {
       	USB_Connect(0); //Disable USB
				USB_Configuration=0;
        f_USB_Error=0;
		 }
}

