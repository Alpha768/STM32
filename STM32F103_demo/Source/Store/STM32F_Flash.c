 
#include "Include.h"

//----------------------------------------------------

#define  STM32F_IFLASH_SIZE 0x20000//128Kbytes
#define  STM32F_IFLASH_PAGE_SIZE 1024//1Kbytes
#define  STM32F_IFLASH  0x8000000

#define EEPROM_FLASH_SIZE  (STM32F_IFLASH_PAGE_SIZE<<1)//2048=2Kbytes
#define EEPROM_FLASH_START (STM32F_IFLASH + STM32F_IFLASH_SIZE -EEPROM_FLASH_SIZE)
#define EEPROM_PAGE_SIZE 	STM32F_IFLASH_PAGE_SIZE

//#define FLASH_KEY1 0x45670123
//#define FLASH_KEY2 0xcdef89ab


void _nop_(void)
{
	unsigned char i;
	i--;
}

void flash_unlock(void) { // unlock the flash program erase controller
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

void flash_wait(void) {
	while (FLASH->SR & 0x1/*FLASH_FLAG_BSY*/)
		;
	FLASH->SR = 0x35;
}

void flash_erase_page(U32 addr) {
	//DisableGlobalInterrupt();
	FLASH->CR |= 0x02;
	FLASH->AR = addr;
	FLASH->CR |= 0x40;
	flash_wait();
	FLASH->CR &= ~0x02;
	//RestoreGlobalInterrupt();
}

int flash_write_halfword(U32 addr, U16 word) {
	flash_wait();
	FLASH->CR |= 0x01;
	_nop_();
	*((volatile U16 *)addr) = word;
	flash_wait();
	FLASH->CR &= ~0x01;
	return *((volatile U16 *)addr) == word;
}

int flash_write_word(U32 addr, U32 word) {
	flash_wait();
	FLASH->CR |= 0x01;
	//asm volatile ("nop");
	_nop_();
	*((volatile U16 *)addr) = word & 0xffff;
	flash_wait();
	*((volatile U16 *) (addr + 2)) = word >> 16;
	flash_wait();
	FLASH->CR &= ~0x01;
	return *((volatile U32 *)addr) == word;
}
//FlashPageWrite
BOOLEAN write_flash_page(U32 Address,U16 len,U8 *buf)
{
 U16 RamSource;
 U16 i;
 U8 *p=(U8*)&RamSource;
   flash_unlock();
   if((Address%STM32F_IFLASH_PAGE_SIZE)==0)
	  flash_erase_page(Address);
   
   len=(len+1)/2;
   
   for(i=0; i<len; i++)
   {
      //memcpy((U8*)&RamSource, buf, 2);
		 p[0]=buf[0];
		 p[1]=buf[1];
	  flash_write_halfword( Address, RamSource);
      
      if ( *(U16*)Address != RamSource )
         return FALSE;
      
      buf+=2;
      Address+=2;
   }
   
   return TRUE;
}

void FlashReadProtect(BOOLEAN bProtect)
{
	if(bProtect)
	{
		if ((FLASH->OBR & RDPRT_Mask) != 0)
			return;
	}
	else
	{
		if ((FLASH->OBR & RDPRT_Mask) == 0)
			return;
	}
	//-----------------------------------------
	flash_unlock();
	//-----------------------------------------
	flash_wait();
	FLASH->OPTKEYR = FLASH_KEY1;
  FLASH->OPTKEYR = FLASH_KEY2;
  FLASH->CR |= CR_OPTER_Set;
  FLASH->CR |= CR_STRT_Set;
	flash_wait();
	/* if the erase operation is completed, disable the OPTER Bit */
  FLASH->CR &= CR_OPTER_Reset;
  /* Enable the Option Bytes Programming operation */
  FLASH->CR |= CR_OPTPG_Set; 
	if(bProtect)
  {
     OB->RDP = 0x00;
  }
  else
  {
     OB->RDP = RDP_Key;  
  }
	flash_wait();
	/* if the program operation is completed, disable the OPTPG Bit */
  FLASH->CR &= CR_OPTPG_Reset;
}
//========================================================================

static BOOLEAN EEPROM_Read_Byte(U16 Addr,U8 *ucVal)
{
	U8 *p=(U8*)EEPROM_FLASH_START;
	*ucVal=p[Addr];
	return TRUE;
}
/*
static BOOLEAN  EEPROM_Write_Byte(U16 Addr,U8 ucVal)
{
	BOOLEAN ucRet;
	U32 PageAddr;
	U16 i;
	U8 *p=(U8*)EEPROM_FLASH_START;
	U8 buffer[EEPROM_PAGE_SIZE];
	if(Addr > (EEPROM_FLASH_SIZE-1))
		return FALSE;
	if(p[Addr]==ucVal)
		return TRUE;
	PageAddr=EEPROM_FLASH_START+Addr-((EEPROM_FLASH_START+Addr)%EEPROM_PAGE_SIZE);
	for(i=0;i<EEPROM_PAGE_SIZE;i++)
	buffer[i]=(*((U8*)(PageAddr+i)));
	buffer[(EEPROM_FLASH_START+Addr)%EEPROM_PAGE_SIZE]=ucVal;
	ucRet=write_flash_page(PageAddr,EEPROM_PAGE_SIZE,buffer);
	return ucRet;
}
*/
static BOOLEAN EEPROM_Write_Block(U16 Addr,U16 Size, U8 *Buffer)
{
	BOOLEAN ucRet=TRUE;
	U16 i;
	//-------------------------------------------------------------
	U8 buffer[EEPROM_PAGE_SIZE];
	U32 PageAddr=EEPROM_FLASH_START+Addr;
	U16 Offset=0;
	U16 Length=0;
	if((Addr+Size) > EEPROM_FLASH_SIZE)
		return FALSE;
	if((EEPROM_FLASH_START+Addr)%EEPROM_PAGE_SIZE)
	{
			PageAddr=EEPROM_FLASH_START+Addr-((EEPROM_FLASH_START+Addr)%EEPROM_PAGE_SIZE);
		  Offset=(EEPROM_FLASH_START+Addr)%EEPROM_PAGE_SIZE;
		  Length=EEPROM_PAGE_SIZE-Offset;
			for(i=0;i<EEPROM_PAGE_SIZE;i++)
				buffer[i]=(*((U8*)(PageAddr+i)));
		  if(Length>Size)
				Length=Size;
			for(i=0;i<Length;i++)
				buffer[Offset+i]=Buffer[i];
		  write_flash_page(PageAddr,EEPROM_PAGE_SIZE,buffer);
		  PageAddr=PageAddr+EEPROM_PAGE_SIZE;
		  Offset=Offset+Length;
	}
	//-------------------------------------------------------------
	while(Length!=Size)
	{
		if((Size-Length)>=EEPROM_PAGE_SIZE)
		{
			write_flash_page(PageAddr,EEPROM_PAGE_SIZE,&Buffer[Offset]);
			PageAddr=PageAddr+EEPROM_PAGE_SIZE;
			Offset=Offset+EEPROM_PAGE_SIZE;
			Length=Length+EEPROM_PAGE_SIZE;
		}
		else
		{
			write_flash_page(PageAddr,Size-Length,&Buffer[Offset]);
			break;
		}
	}
	return ucRet;
}
static BOOLEAN EEPROM_Read_Block(U16 Addr,U16 Size,U8 *Buffer)
{
	U16 i;
	for(i=0;i<Size;i++)
	{
		if(EEPROM_Read_Byte(Addr+i,&Buffer[i])==FALSE)
			return FALSE;
	}
	return FALSE;
}
//========================================================================

static U32 Store_Write(U32 cmd,va_list arg_ptr)
{
    U16 Addr=va_arg(arg_ptr, VA_U16);
    U16 Size=va_arg(arg_ptr, VA_U16);
    U8 *Buffer=va_arg(arg_ptr, U8*);
    
		EEPROM_Write_Block(Addr,Size,Buffer);
    return TRUE;
}
 
static U32 Store_Read(U32 cmd,va_list arg_ptr)
{
    U16 Addr=va_arg(arg_ptr, VA_U16);
    U16 Size=va_arg(arg_ptr, VA_U16);
    U8 *Buffer=va_arg(arg_ptr, U8*);
    EEPROM_Read_Block(Addr,Size,Buffer);
    return TRUE;
}

static U32 Store_Protect(U32 cmd,va_list arg_ptr)
{
	  U8 flag=va_arg(arg_ptr, VA_U8);
	  if(flag)
			FlashReadProtect(TRUE);
		else
			FlashReadProtect(FALSE);
		return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
  _PARAMETER_UNUSED_(arg_ptr);
  //PRINTF("STM32F FLASH Online !!!\n",0);
	return TRUE;
}

static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
  {STORECTL_WRITE,Store_Write},
  {STORECTL_READ,Store_Read},
	{STORECTL_PROTECT,Store_Protect},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_FLASH_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_FLASH_Driver =
{   
	NULL,
	STORE_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_FLASH_PatchTable
};

