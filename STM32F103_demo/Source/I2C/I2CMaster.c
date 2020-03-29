#include <stm32f10x_lib.h>                  /* stm32f10x definitions */
#include "type.h"
#include "STM32_Reg.h"                                  // missing bit definitions
#include "Time.h"
#include "I2C.H"
#include "STM32_Init.h"                     /* stm32 initialisation */
/* I2C SPE mask */
#define CR1_PE_Set              ((U16)0x0001)
#define CR1_PE_Reset            ((U16)0xFFFE)

/* I2C START mask */
#define CR1_START_Set           ((U16)0x0100)
#define CR1_START_Reset         ((U16)0xFEFF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((U16)0x0200)
#define CR1_STOP_Reset          ((U16)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((U16)0x0400)
#define CR1_ACK_Reset           ((U16)0xFBFF)

/* I2C ENGC mask */
#define CR1_ENGC_Set            ((U16)0x0040)
#define CR1_ENGC_Reset          ((U16)0xFFBF)

/* I2C SWRST mask */
#define CR1_SWRST_Set           ((U16)0x8000)
#define CR1_SWRST_Reset         ((U16)0x7FFF)

/* I2C PEC mask */
#define CR1_PEC_Set             ((U16)0x1000)
#define CR1_PEC_Reset           ((U16)0xEFFF)

/* I2C ENPEC mask */
#define CR1_ENPEC_Set           ((U16)0x0020)
#define CR1_ENPEC_Reset         ((U16)0xFFDF)

/* I2C ENARP mask */
#define CR1_ENARP_Set           ((U16)0x0010)
#define CR1_ENARP_Reset         ((U16)0xFFEF)

/* I2C NOSTRETCH mask */
#define CR1_NOSTRETCH_Set       ((U16)0x0080)
#define CR1_NOSTRETCH_Reset     ((U16)0xFF7F)

/* I2C registers Masks */
#define CR1_CLEAR_Mask          ((U16)0xFBF5)

/* I2C DMAEN mask */
#define CR2_DMAEN_Set           ((U16)0x0800)
#define CR2_DMAEN_Reset         ((U16)0xF7FF)

/* I2C LAST mask */
#define CR2_LAST_Set            ((U16)0x1000)
#define CR2_LAST_Reset          ((U16)0xEFFF)

/* I2C FREQ mask */
#define CR2_FREQ_Reset          ((U16)0xFFC0)

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((U16)0x0001)
#define OAR1_ADD0_Reset         ((U16)0xFFFE)

/* I2C ENDUAL mask */
#define OAR2_ENDUAL_Set         ((U16)0x0001)
#define OAR2_ENDUAL_Reset       ((U16)0xFFFE)

/* I2C ADD2 mask */
#define OAR2_ADD2_Reset         ((U16)0xFF01)

/* I2C F/S mask */
#define CCR_FS_Set              ((U16)0x8000)

/* I2C CCR mask */
#define CCR_CCR_Set             ((U16)0x0FFF)

/* I2C FLAG mask */
#define FLAG_Mask               ((U32)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask               ((U32)0x07000000)

//***************************************************************
/* --EV5 */
//#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
/* --EV6 */
//#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
//#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */

/* Master RECEIVER mode -----------------------------*/ 
/* --EV7 */
//#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
//#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* --EV8_2 */
//#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */


#define _TIMEOUT_CNT_  255
U32 I2C_LastEvent(void)
{
	U32 lastevent = 0;
	U32 flag1 = 0, flag2 = 0;

	/* Read the I2Cx status register */
	flag1 = I2C1->SR1;
	flag2 = I2C1->SR2;
	flag2 = flag2 << 16;
	
	/* Get the last event value from I2C status register */
	lastevent = (flag1 | flag2) & FLAG_Mask;
	return lastevent;
}


U8 IicStart(void)
{
	U16 Timeout = _TIMEOUT_CNT_;
	/* Generate a START condition */
    I2C1->CR1 |= CR1_START_Set; 
  while((I2C_LastEvent()& I2C_EVENT_MASTER_MODE_SELECT)!=I2C_EVENT_MASTER_MODE_SELECT)
  {
	  if (Timeout-- == 0)
           return I2C_BUS_ERROR;
	  Delay1Us(1);
  }
  return I2C_BUS_OK;
}
U8 IicStop( void )
{
	U16 Timeout = _TIMEOUT_CNT_;
	//U32 flag2 = 0;
	/* Generate a STOP condition */
    I2C1->CR1 |= CR1_STOP_Set;
	//flag2 = I2C1->SR2;
	 while((I2C_LastEvent()& (I2C_FLAG_BUSY<<16))!=(I2C_FLAG_BUSY<<16))
  	{
	  if (Timeout-- == 0)
           return I2C_BUS_ERROR;
	  Delay1Us(1);
  	} 
	return I2C_BUS_OK;
}

U8 IicSendByte( U8 chr )
{
  	U16 Timeout = _TIMEOUT_CNT_;
	U32 events;
  /* Write in the DR register the data to be sent */
	I2C1->DR = chr;
	while(Timeout--)
	{
		events=I2C_LastEvent();
		if((events&I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)==I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)
			return I2C_BUS_OK;
		if((events&I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)
			return I2C_BUS_OK;
		if((events&I2C_EVENT_MASTER_BYTE_TRANSMITTED)==I2C_EVENT_MASTER_BYTE_TRANSMITTED)
			return I2C_BUS_OK;
		Delay1Us(1);
	}
	return I2C_BUS_ERROR;
}
U8 IicReceiveByte(U8 ack)
{
   
	U8 ucVal;
	U16 Timeout= _TIMEOUT_CNT_;
	if(ack)
		// Enable the acknowledgement
     I2C1->CR1 |= CR1_ACK_Set; 
	else
		// Disable the acknowledgement
       I2C1->CR1 &= CR1_ACK_Reset;
	while((I2C_LastEvent()&I2C_EVENT_MASTER_BYTE_RECEIVED)!=I2C_EVENT_MASTER_BYTE_RECEIVED)
	{
		if (Timeout-- == 0)
			break;
		Delay1Us(1);
	}
	//The data in the DR register
		ucVal = I2C1->DR;
	return ucVal;
}
void I2C_Config(U32 Speed,U32 DutyCycle)
{
	U16 tmpreg = 0, freqrange = 0;
	U16 result = 0x04;
	U32 pclk1 = 72000000/2;	
	//unsigned int DutyCycle=I2C_DutyCycle_2;
	pclk1 = stm32_GetPCLK1();
	/*---------------------------- I2Cx CR2 Configuration ------------------------*/
	/* Get the I2Cx CR2 value */
	tmpreg = I2C1->CR2;
	/* Clear frequency FREQ[5:0] bits */
	tmpreg &= CR2_FREQ_Reset;
	/* Set frequency bits depending on pclk1 value */
	freqrange = (U16)(pclk1 / 1000000);
	tmpreg |= freqrange;
	/* Write to I2Cx CR2 */
	I2C1->CR2 = tmpreg;
	
	/*---------------------------- I2Cx CCR Configuration ------------------------*/
	/* Disable the selected I2C peripheral to configure TRISE */
	I2C1->CR1 &= CR1_PE_Reset;
	/* Reset tmpreg value */
	/* Clear F/S, DUTY and CCR[11:0] bits */
	tmpreg = 0;
	
	/* Configure speed in standard mode */
	if (Speed <= 100000)
	{
		/* Standard mode speed calculate */
		result = (U16)(pclk1 / (Speed << 1));
		/* Test if CCR value is under 0x4*/
		if (result < 0x04)
		{
			/* Set minimum allowed value */
			result = 0x04;  
		}
		/* Set speed value for standard mode */
		tmpreg |= result;	  
		/* Set Maximum Rise Time for standard mode */
		I2C1->TRISE = freqrange + 1; 
	}
	/* Configure speed in fast mode */
	else /*(I2C_InitStruct->I2C_ClockSpeed <= 400000)*/
	{
		if (DutyCycle == I2C_DutyCycle_2)
		{
			// Fast mode speed calculate: Tlow/Thigh = 2
			result = (U16)(pclk1 / (Speed * 3));
		}
		else //I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_16_9
		{
			// Fast mode speed calculate: Tlow/Thigh = 16/9
			result = (U16)(pclk1 / (Speed * 25));
			/* Set DUTY bit */
			result |= I2C_DutyCycle_16_9;
		}
		
		/* Test if CCR value is under 0x1*/
		if ((result & CCR_CCR_Set) == 0)
		{
			/* Set minimum allowed value */
			result |= (U16)0x0001;  
		}
		/* Set speed value and set F/S bit for fast mode */
		tmpreg |= (U16)(result | CCR_FS_Set);
		/* Set Maximum Rise Time for fast mode */
		I2C1->TRISE = (U16)(((freqrange * (U16)300) / (U16)1000) + (U16)1);  
	}
	
	/* Write to I2Cx CCR */
	I2C1->CCR = tmpreg;
	/* Enable the selected I2C peripheral */
	I2C1->CR1 |= CR1_PE_Set;
	
	/*---------------------------- I2Cx CR1 Configuration ------------------------*/
	/* Get the I2Cx CR1 value */
	tmpreg = I2C1->CR1;
	/* Clear ACK, SMBTYPE and  SMBUS bits */
	tmpreg &= CR1_CLEAR_Mask;
	/* Configure I2Cx: mode and acknowledgement */
	/* Set SMBTYPE and SMBUS bits according to I2C_Mode value */
	/* Set ACK bit according to I2C_Ack value */
	tmpreg |= (U16)((U32)I2C_Mode_I2C | I2C_Ack_Enable);
	/* Write to I2Cx CR1 */
	I2C1->CR1 = tmpreg;
	
	/*---------------------------- I2Cx OAR1 Configuration -----------------------*/
	/* Set I2Cx Own Address1 and acknowledged address */
  I2C1->OAR1 = (I2C_AcknowledgedAddress_7bit | 0xff);//Own Address1 =0xff
}
void I2CInit(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;                   // enable clock for GPIOB
  RCC->APB1ENR |= RCC_APB1ENR_I2C1;                // enable clock for I2C1
  //--------------
  GPIOB->CRL   &= ~(0xFFUL  << 24);                      // Clear PB6, PB7
  GPIOB->CRL   |=  (0x0FUL  << 24);                      // PB6 Speed_50MHz,Mode_AF_OD
  GPIOB->CRL   |=  (0x0FUL  << 28);                     // PB7 Speed_50MHz,Mode_AF_OD
  //---------------
  // Enable I2C1 reset state 
  RCC->APB1RSTR |= RCC_APB1ENR_I2C1;
  // Release I2C1 from reset state
  RCC->APB1RSTR &= ~RCC_APB1ENR_I2C1;
  I2C_Config(200000,I2C_DutyCycle_2);
}

void IicSetSpeed(U16 kHz)
{
	
}
