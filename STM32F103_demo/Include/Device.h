#ifndef _DEVICE_H_
#define _DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define  READ_ARGS_FLAG 0x00800000
#define  INVALID_CMDID  0xffffffff

#define  VA_ARG_PTR  0x00400000
#define  REGISTER_FLAG  0x01000000
#define  UNREGISTER_FLAG  0x02000000

//Device Index  ---->0~0x3f
//command Index ---->0~0xff
//Device Class  ---->0~0x3f

//#define DEV_MAGIC (0xF5<<24)
#define DEV_MAGIC (0xF4<<24)
#define DEV_W_R    (0xC0<<8)
#define MakeCmdIdEx(device_cmd,device_index)  ((device_index<<8)|device_cmd|DEV_MAGIC|DEV_W_R)
#define GetDeviceClass(cmd) ((cmd>>16)&0x3f)
#define GetCmdIdEx(cmd) ((cmd)&(0x3f00ff|READ_ARGS_FLAG))
#define GetCmdId(cmd) ((cmd)&0x3f00ff)
#define MakeCmdId(device_cmd)  (device_cmd|DEV_MAGIC|DEV_W_R)
#define GetDeviceIndex(cmd) ((cmd>>8)&0x3f)
//---------------------------------------------
//mask
#define USR_CTL_MASK			0x000000
#define ADC_CTL_MASK			0x010000
#define PWM_CTL_MASK			0x020000
#define KEY_CTL_MASK			0x030000
#define IR_CTL_MASK		        0x040000
#define KEYPAD_CTL_MASK		    0x050000
#define SPI_CTL_MASK			0x060000
#define UART_CTL_MASK			0x070000
#define STORE_CTL_MASK		    0x080000
#define PANEL_CTL_MASK		    0x090000

	
#define SYS_CTL_MASK			0x3F0000
//*********************************************
//class
#define  USR_CLASS		    (USR_CTL_MASK>>16)
#define  ADC_CLASS     		(ADC_CTL_MASK>>16)
#define  PWM_CLASS          (PWM_CTL_MASK>>16)
#define  KEY_CLASS	        (KEY_CTL_MASK>>16)
#define  IR_CLASS		    (IR_CTL_MASK>>16)
#define  KEYPAD_CLASS		(KEYPAD_CTL_MASK>>16)
#define  SPI_CLASS			(SPI_CTL_MASK>>16)
#define  UART_CLASS			(UART_CTL_MASK>>16)
#define  STORE_CLASS		(STORE_CTL_MASK>>16)
#define  PANEL_CLASS		(PANEL_CTL_MASK>>16)


#define  MAX_CLASS      (PANEL_CLASS+1)
//*********************************************
//sys common command
#define SYSCTL_POWER        (SYS_CTL_MASK|0x00)
#define CTL_AC_ON                    0x10
#define CTL_DC_ON                    0x20
 
#define SYSCTL_STANDBY        (SYS_CTL_MASK|0x01)
#define CTL_AC_STANDBY                   0x10
#define CTL_DC_STANDBY                 0x20
 
#define SYSCTL_POLLING        (SYS_CTL_MASK|0x02)
#define SYSCTL_CHECK_ONLINE (SYS_CTL_MASK|0x03)
 
#define SYSCTL_INIT         (SYS_CTL_MASK|0x04)
//*********************************************
//ADC command
//#define ADCCTL_POWER           (ADC_CTL_MASK|0x00)
//#define CTL_AC_ON                    0x10
//#define CTL_DC_ON                    0x20       

#define ADCCTL_INIT         		(ADC_CTL_MASK|0x01)
#define ADCCTL_CONVERT          (ADC_CTL_MASK|0x02)
	
#define CTL_ON 0x01
#define CTL_OFF 0x00
//*********************************************
//PWM command
#define PWMCTL_FREQ_DUTY    		 (PWM_CTL_MASK|0x00)
#define PWMCTL_FREQ       			 (PWM_CTL_MASK|0x01)
#define PWMCTL_DUTY    				   (PWM_CTL_MASK|0x02)	
//*********************************************
//Key command
//#define KEYCTL_POWER       (KEY_CTL_MASK|0x00)
#define KEYCTL_PROCESS       (KEY_CTL_MASK|0x01)
#define KEYCTL_TIME          (KEY_CTL_MASK|0x02)
#define KEYCTL_CLEAN         (KEY_CTL_MASK|0x03)
//*********************************************	
//SPI command
#define SPICTL_INIT							 (SPI_CTL_MASK|0x00)
#define SPICTL_TRANSMIT_BYTE     (SPI_CTL_MASK|0x01)
#define SPICTL_RECEIVE_BYTE			 (SPI_CTL_MASK|0x02)
#define SPICTL_TRANSMIT_WORD		 (SPI_CTL_MASK|0x03)
#define SPICTL_RECEIVE_WORD      (SPI_CTL_MASK|0x04)
//*******************************************************
//UART command
#define UARTCTL_INIT							 (UART_CTL_MASK|0x00)
#define UARTCTL_TRANSMIT           (UART_CTL_MASK|0x01)
#define UARTCTL_RECEIVE			       (UART_CTL_MASK|0x02)
#define UARTCTL_REVCNT		       	 (UART_CTL_MASK|0x03)
#define UARTCTL_REVLOOK            (UART_CTL_MASK|0x04)
//*****************************************************************
//store command
#define STORECTL_WRITE               (STORE_CTL_MASK|0x01)
#define STORECTL_READ                (STORE_CTL_MASK|0x02)
#define STORECTL_PROTECT			 (STORE_CTL_MASK|0x03)
//*****************************************************************
//panel command
#define PANELCTL_WRITE               (PANEL_CTL_MASK|0x01)
#define PANELCTL_READ                (PANEL_CTL_MASK|0x02)
#define PANELCTL_PROTECT			 (PANEL_CTL_MASK|0x03)
//*****************************************************************
//IR command
#define IRCTL_PROCESS         (IR_CTL_MASK|0x01)
#define IRCTL_IRKEYPADSTATUS  (IR_CTL_MASK|0x02)
#define IRCTL_CLEAN           (IR_CTL_MASK|0x03)
//*********************************************	
//Keypad command
#define KEYPADCTL_PROCESS       (KEYPAD_CTL_MASK|0x01)
#define KEYPADCTL_KEYPADSTATUS  (KEYPAD_CTL_MASK|0x02)
#define KEYPADCTL_CLEAN         (KEYPAD_CTL_MASK|0x03)
//*********************************************	

#define PATCH_BEFORE              0x010000
#define PATCH_AFTER               0x020000
#define PATCH_FULL                0x030000

#ifndef _WIN32
#define VA_U8   U32
#define VA_U16  U32
#define VA_U32  U32
	
#define VA_S8   S32
#define VA_S16  S32
#define VA_S32  S32
#else
#define VA_U8   U8
#define VA_U16  U16
#define VA_U32  U32
	
#define VA_S8   S8
#define VA_S16  S16
#define VA_S32  S32
#endif

#define DRIVER_MODE_REAL 1
#define DRIVER_MODE_BUFFER  2

#define STATUS_OFFLINE 0x00
#define STATUS_ONLINE  0x01

typedef U32 (*_DEV_CONTROL_)(U32 cmd, ...);


typedef struct {
    void ( *Attach) ( void *) ;
	U8 *Name ;
	U8 ( *BusRead)(U8 *AddrBuffer,U16 AddrCnt,U8 *DataBuffer,U16 DataCnt) ;
	U8 ( *BusWrite)(U8 *AddrBuffer,U16 AddrCnt,U8 *DataBuffer,U16 DataCnt) ;
	U8 ( *RangeCheck)( U16 *Address,U16 *Data ) ;
	U8 Flag ;
} BUS_DEVICE ;

/*
typedef struct {
    void ( *Attach) ( void *) ;
	U8 *Name ;
	void ( *BusRead)( void ) ;
	void ( *BusWrite)( void ) ;
	U8 ( *RangeCheck)( void ) ;
	U8 Flag ;
} BUS_DEVICE ;
*/

typedef BUS_DEVICE * LPBUS_DEVICE;

typedef U32 (*_DO_COMMAND_)(U32 cmd,va_list arg_ptr);

typedef struct tagPATCH_TABLE{
	U32 CmdId;
    _DO_COMMAND_ BeforePatch;
    _DO_COMMAND_ AfterPatch;
    _DO_COMMAND_ FullPatch;
}PATCH_TABLE;

typedef PATCH_TABLE * LPATCH_TABLE;


typedef struct tagCTRL_TAB{
	U32 CmdId;
    _DO_COMMAND_ DoCommand;
}CTRL_TAB;

typedef struct tagDEVICE
{
	void ( *PatchDriverMode) ( U8) ;
	U8  DeviceClass;
	U32 status;
	CTRL_TAB *   lpControlTab;
	LPBUS_DEVICE lpBusDevice;
	LPATCH_TABLE lpPatchTable;
}DEVICE;

typedef DEVICE * LPDEVICE;

extern U32 DeviceControl(U32 cmd,...);
#define DeviceRegister(pDevice,pPatchTable,...) DeviceControl(SYSCTL_CHECK_ONLINE|REGISTER_FLAG,pDevice,pPatchTable,##__VA_ARGS__)
#define DeviceUnRegister(command,pDevice,...) DeviceControl(command|UNREGISTER_FLAG,pDevice,##__VA_ARGS__)

extern DEVICE f_STM32F_ADC_Driver;
extern DEVICE f_STM32F_PWM_Driver;
extern DEVICE f_KeyBoard_Driver;
extern DEVICE f_STM32F_IR_Driver;
extern DEVICE f_STM32F_Keypad_Driver;
extern DEVICE f_STM32F_SPI_Driver;
extern DEVICE f_STM32F_UART_Driver;
extern DEVICE f_STM32F_FLASH_Driver;
extern DEVICE f_STM32F_PANEL_Driver;

extern void InitDevice(void);

#ifdef __cplusplus
};
#endif

#endif
