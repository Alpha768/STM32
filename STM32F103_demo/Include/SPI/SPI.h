#ifndef __SPI_H__
#define __SPI_H__

#define ChipDisable()         GPIOA->BSRR = GPIO_Pin_4 //hight
#define ChipEnable()         GPIOA->BRR  = GPIO_Pin_4 //low

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define SPI_DATA_8B	0
#define SPI_DATA_16B 1

#define SPI_FIRST_MSB 0
#define SPI_FIRST_LSB 1

#define SPI_CLK_36M			SPI_BaudRatePrescaler_2
#define SPI_CLK_18M			SPI_BaudRatePrescaler_4        
#define SPI_CLK_9M			SPI_BaudRatePrescaler_8        
#define SPI_CLK_4500K		SPI_BaudRatePrescaler_16       
#define SPI_CLK_2250K		SPI_BaudRatePrescaler_32       
#define SPI_CLK_1125K		SPI_BaudRatePrescaler_64       
#define SPI_CLK_562P5K		SPI_BaudRatePrescaler_128       
#define SPI_CLK_218P25K		SPI_BaudRatePrescaler_256 

#endif
