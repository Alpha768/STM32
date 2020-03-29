#include "Include.h"
#ifdef USE_SPI
//################################################################################
/* SPI SPE mask */
#define CR1_SPE_Set          ((U16)0x0040)
#define CR1_SPE_Reset        ((U16)0xFFBF)

/* I2S I2SE mask */
#define I2SCFGR_I2SE_Set     ((U16)0x0400)
#define I2SCFGR_I2SE_Reset   ((U16)0xFBFF)

/* SPI CRCNext mask */
#define CR1_CRCNext_Set      ((U16)0x1000)

/* SPI CRCEN mask */
#define CR1_CRCEN_Set        ((U16)0x2000)
#define CR1_CRCEN_Reset      ((U16)0xDFFF)

/* SPI SSOE mask */
#define CR2_SSOE_Set         ((U16)0x0004)
#define CR2_SSOE_Reset       ((U16)0xFFFB)

/* SPI registers Masks */
#define CR1_CLEAR_Mask       ((U16)0x3040)
#define I2SCFGR_CLEAR_Mask   ((U16)0xF040)

/* SPI or I2S mode selection masks */
#define SPI_Mode_Select      ((U16)0xF7FF)
#define I2S_Mode_Select      ((U16)0x0800) 

/* I2S clock source selection masks */
#define I2S2_CLOCK_SRC       ((U32)(0x00020000))
#define I2S3_CLOCK_SRC       ((U32)(0x00040000))
#define I2S_MUL_MASK         ((U32)(0x0000F000))
#define I2S_DIV_MASK         ((U32)(0x000000F0))

/**
  * @brief  Checks whether the specified SPI/I2S flag is set or not.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  SPI_I2S_FLAG: specifies the SPI/I2S flag to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_I2S_FLAG_TXE: Transmit buffer empty flag.
  *     @arg SPI_I2S_FLAG_RXNE: Receive buffer not empty flag.
  *     @arg SPI_I2S_FLAG_BSY: Busy flag.
  *     @arg SPI_I2S_FLAG_OVR: Overrun flag.
  *     @arg SPI_FLAG_MODF: Mode Fault flag.
  *     @arg SPI_FLAG_CRCERR: CRC Error flag.
  *     @arg I2S_FLAG_UDR: Underrun Error flag.
  *     @arg I2S_FLAG_CHSIDE: Channel Side flag.
  * @retval The new state of SPI_I2S_FLAG (SET or RESET).
  */
static U8 SPI_GetFlagStatus(U16 flag)
{
  U8 bitstatus = RESET;

  /* Check the status of the specified SPI/I2S flag */
  if ((SPI1->SR & flag) != 0)
  {
    /* SPI_I2S_FLAG is set */
    bitstatus = 1;
  }
  else
  {
    /* SPI_I2S_FLAG is reset */
    bitstatus = 0;
  }
  /* Return the SPI_I2S_FLAG status */
  return  bitstatus;
}

static void InitSpiMaster(U8 Mode,U8 BaudRatePrescaler,U8 DataSize,U8 FirstBit)
{
	U16 RegVal = 0;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                   // enable clock for GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;                // enable clock for SPI1
  //--------------
    GPIOA->CRL   &= ~(0xFFFFUL  << 16);                      // Clear PA4, PA5,PA6, PA7
    GPIOA->CRL   |=  (0x03UL  << 16);                      // PA4 Speed_50MHz,Mode_Out_PP
    GPIOA->CRL   |=  (0x0BUL  << 20);                     // PA5 Speed_50MHz,Mode_AF_PP
	GPIOA->CRL   |=  (0x0BUL  << 24);                      // PA6 Speed_50MHz,Mode_AF_PP
    GPIOA->CRL   |=  (0x0BUL  << 28);                     // PA7 Speed_50MHz,Mode_AF_PP

	GPIOA->BSRR = GPIO_Pin_4;
  // Enable SPI1 reset state 
  RCC->APB2RSTR |= RCC_APB2ENR_SPI1;
  // Release SPI1 from reset state
  RCC->APB2RSTR &= ~RCC_APB2ENR_SPI1;
//---------------------------------------------------------------------------------------
/*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  RegVal = SPI1->CR1;
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  RegVal &= CR1_CLEAR_Mask;
  /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
     master/salve mode, CPOL and CPHA */
  /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
  /* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
  /* Set LSBFirst bit according to SPI_FirstBit value */
  /* Set BR bits according to SPI_BaudRatePrescaler value */
  /* Set CPOL bit according to SPI_CPOL value */
  /* Set CPHA bit according to SPI_CPHA value */

    //RegVal |= (U16)((U32)SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master |SPI_DataSize_8b |
   	//			  SPI_CPOL_High |SPI_CPHA_2Edge | SPI_NSS_Soft | SPI_BaudRatePrescaler_8 | SPI_FirstBit_MSB);

   RegVal |= (U16)((U32)SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_NSS_Soft | BaudRatePrescaler);
   //---------------------------------------------------
   if(Mode==SPI_MODE0)
   	  RegVal |= (SPI_CPOL_High |SPI_CPHA_1Edge);
   else if(Mode==SPI_MODE1)
   	  RegVal |= (SPI_CPOL_Low |SPI_CPHA_2Edge);
   else  if(Mode==SPI_MODE2)
      RegVal |= (SPI_CPOL_Low |SPI_CPHA_1Edge);
   else if(Mode==SPI_MODE3)
   	  RegVal |= (SPI_CPOL_High |SPI_CPHA_2Edge);
   //----------------------------------------------------
   if(DataSize==SPI_DATA_8B)
	  RegVal |= SPI_DataSize_8b;
   else if(DataSize==SPI_DATA_16B)
	   RegVal |= SPI_DataSize_16b;
   //----------------------------------------------------
   if(FirstBit==SPI_FIRST_MSB)
	   RegVal |= SPI_FirstBit_MSB;
   else
       RegVal |= SPI_FirstBit_LSB;
   //----------------------------------------------------
  /* Write to SPIx CR1 */
  SPI1->CR1 = RegVal;
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= SPI_Mode_Select;		

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  SPI1->CRCPR = 7;
//---------------------------------------------------------------------------------------
 /* Enable SPI1  */
    SPI1->CR1 |= CR1_SPE_Set;  
}

static void SpiSendByte(U8 ucData)
{   
    while(SPI_GetFlagStatus(SPI_I2S_FLAG_TXE) == 0);
	SPI1->DR = ucData;
}

static U8 SpiReceiveByte(void)
{
	U8 ucData;
    while((SPI_GetFlagStatus(SPI_I2S_FLAG_RXNE) == 0));
	ucData=SPI1->DR;
    return ucData;
}

static void SpiSendWord(U16 u16Data)
{   
    while(SPI_GetFlagStatus(SPI_I2S_FLAG_TXE) == 0);
	SPI1->DR = u16Data;
	while(SPI_GetFlagStatus(SPI_I2S_FLAG_TXE) == 0);
}

static U16 SpiReceiveWord(void)
{
	U16 u16Data;
    while((SPI_GetFlagStatus(SPI_I2S_FLAG_RXNE) == 0));
	u16Data=SPI1->DR;
    return u16Data;
}
//====================================================================================================

static U32 SPI_Data(U32 cmd,va_list arg_ptr)
{
	U8 u8Data;
	U16 u16Data;
	U8 *p8Data;
	U16 *p16Data;
	if(cmd == SPICTL_TRANSMIT_BYTE)
	{
		u8Data=va_arg(arg_ptr, VA_U8);
		SpiSendByte(u8Data);
	}
	if(cmd == SPICTL_TRANSMIT_WORD)
	{
		u16Data=va_arg(arg_ptr, VA_U16);
		SpiSendWord(u16Data);
	}
	if(cmd == SPICTL_RECEIVE_BYTE)
	{
		p8Data=va_arg(arg_ptr, U8*);
		*p8Data=SpiReceiveByte();
	}
	if(cmd == SPICTL_RECEIVE_WORD)
	{
		p16Data=va_arg(arg_ptr, U16*);
		*p16Data=SpiReceiveWord();
	}
	return TRUE;
}

static U32 SPI_init(U32 cmd,va_list arg_ptr)
{
	U8 Mode=va_arg(arg_ptr, VA_U8);
	U8 BaudRatePrescaler=va_arg(arg_ptr, VA_U8);
	U8 DataSize=va_arg(arg_ptr, VA_U8);
	U8 FirstBit=va_arg(arg_ptr, VA_U8);
	InitSpiMaster(Mode,BaudRatePrescaler,DataSize,FirstBit);
	return TRUE;
}

static U32 CheckOnline(U32 cmd,va_list arg_ptr)
{
	_PARAMETER_UNUSED_(cmd);
  _PARAMETER_UNUSED_(arg_ptr);
  //PRINTF("STM32F SPI Online !!!\n",0);
	return TRUE;
}

static CTRL_TAB ControlTab[]=
{
	{SYSCTL_CHECK_ONLINE,   CheckOnline},
	{SPICTL_INIT,		SPI_init},
	{SPICTL_TRANSMIT_BYTE,	SPI_Data},
	{SPICTL_RECEIVE_BYTE,SPI_Data},
	{SPICTL_TRANSMIT_WORD,SPI_Data},
	{SPICTL_RECEIVE_WORD,SPI_Data},
	{INVALID_CMDID,NULL}
};

static PATCH_TABLE STM32F_SPI_PatchTable[1]={{INVALID_CMDID,NULL,NULL,NULL}};


DEVICE f_STM32F_SPI_Driver =
{   
	NULL,
	SPI_CLASS,
	STATUS_OFFLINE,
	ControlTab,
	NULL,
	STM32F_SPI_PatchTable
};
//############################################################################################
#endif
