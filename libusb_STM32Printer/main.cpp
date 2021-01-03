#include <stdio.h>
#include "lusb0_usb.h"

#define U8 unsigned char 
#define U16 unsigned short int
#define U32 unsigned int
#define S8 signed char
#define S16 signed short int
#define S32 signed int
#define Bool unsigned char
#define U64  unsigned __int64
#define S64  signed __int64

#define BOOLEAN  unsigned char

#define FALSE 0
#define TRUE 1
#define NULL 0
#define ACT  2


#define MY_VID 0xC251
#define MY_PID 0x1706

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint 2
#define EP_IN 0x81
#define EP_OUT 0x01

// Device of bytes to transfer.
#define BUF_SIZE 64


struct usb_device *dev;
struct usb_dev_handle *udev;
struct usb_bus *bus;



//指定USB设备的配置和接口
void my_init_usbdev()
{
    //libusb规定下面这两个函数必须要被调用
    if (usb_set_configuration(udev, MY_CONFIG) < 0) {
        printf("error setting config #%d: %s", MY_CONFIG, usb_strerror());
        exit(1);
    }
    if (usb_claim_interface(udev, MY_INTF) < 0) {
        printf("error claiming interface #%d:\n%s", MY_INTF, usb_strerror());
        exit(1);
    }
}

void my_usb_get_device_list(void)
{

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            /* for specified device
            if (dev->descriptor.idVendor == MY_VID
                        && dev->descriptor.idProduct == MY_PID)
            {
                return usb_open(dev);
            }
            */

            printf("VID:VID = %x:%x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);

        }
    }

}


usb_dev_handle *open_dev(void)
{
    struct usb_bus *bus;

    for(bus = usb_get_busses(); bus; bus = bus->next) {
        for(dev = bus->devices; dev; dev = dev->next) {
            if((dev->descriptor.idVendor == MY_VID) && (dev->descriptor.idProduct == MY_PID)) {
                    return usb_open(dev);
            }
        }
    }
    return 0;
}

void printf_device_descriptor(usb_device_descriptor *desc)
{
        printf("bLength:             %u\n",     desc->bLength);
        printf("bDescriptorType:     %02Xh\n",  desc->bDescriptorType);
        printf("bcdUSB:              %04Xh\n",  desc->bcdUSB);
        printf("bDeviceClass:        %02Xh\n",  desc->bDeviceClass);
        printf("bDeviceSubClass:     %02Xh\n",  desc->bDeviceSubClass);
        printf("bDeviceProtocol:     %02Xh\n",  desc->bDeviceProtocol);
        printf("bMaxPacketSize0:     %02Xh\n",  desc->bMaxPacketSize0);
        printf("idVendor:            %04Xh\n",  desc->idVendor);
        printf("idProduct:           %04Xh\n",  desc->idProduct);
        printf("bcdDevice:           %04Xh\n",  desc->bcdDevice);
        printf("iManufacturer:       %u\n",     desc->iManufacturer);
        printf("iProduct:            %u\n",     desc->iProduct);
        printf("iSerialNumber:       %u\n",     desc->iSerialNumber);
        printf("bNumConfigurations:  %u\n",     desc->bNumConfigurations);
}

unsigned short CRC16Table[256] = {
	0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022, 
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041, 
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082, 
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1, 
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, 
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2, 
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, 
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101, 
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321, 
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2, 
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, 
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381, 
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2, 
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, 
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261, 
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202,
};

unsigned short CRC16( const unsigned char* ptr, unsigned long len )
{
	unsigned short crc;
	unsigned long i;
	crc = 0;
	for(i = 0; i < len; i++)
	{
	crc = (crc << 8) ^ CRC16Table[((crc >> 8) ^ ptr[i]) & 0xff];
	}
	return crc;
}

void UsbPackageTransmit(U8 *buffer,U16 len)
{
	U8 buf[516];
	U16 i,crc16;
	int ret;

	if(512<len || buffer==NULL)
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

//	WriteInterFace(INTERFACE0,buf,len+4);	

	ret = usb_bulk_write(udev, EP_OUT, (char *)buf, len+4, 50000);
    if (ret < 0) {
            printf("error writing: %s", usb_strerror());
      //      exit(1);
    }
}


int main(int argc, char **argv)
{
      int ret,i;
      char readdata[BUF_SIZE];
      U8 cmdTable[]={0x04,0x00};

      usb_set_debug(250);
      usb_init(); /* initialize the library */
      usb_find_busses(); /* find all busses */
      usb_find_devices(); /* find all connected devices */
 
    if (!(udev = open_dev())) {
        printf("error opening device: %s\n", usb_strerror());
    } else
        printf("open success: device %04X:%04X opened\n", MY_VID, MY_PID);

    printf_device_descriptor(&dev->descriptor);

    my_init_usbdev();

	UsbPackageTransmit(cmdTable,sizeof(cmdTable));

    //批量读(同步)
    ret = usb_bulk_read(udev, EP_IN, readdata, sizeof(readdata), 5000);
	printf("ret=%d\n",ret);
    if (ret < 0) {
            printf("error reading:%s", usb_strerror());
           // exit(1);
    }
    readdata[ret] = '\0';

	for(i=0;i<BUF_SIZE;i++)
		printf("readdata[%d]=0x%x\n",i,readdata[i]);

    usb_close(udev);

  //    my_usb_get_device_list();


	return 0;
}