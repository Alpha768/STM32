#ifndef __SSD1306_DRV_H__
#define __SSD1306_DRV_H__

extern void drvSSD1306_Init(void);

/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR     0x78//0x3C //    0x78
//#define SSD1306_I2C_ADDR       0x7A
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/*definition--------------------------------------------*/
#define SSD1306_CMD  0	//写命令
#define SSD1306_DATA 1	//写数据

#define 	SIZE 		0//16		//显示字符的大小
#define 	Max_Column	128		//最大列数
#define		Max_Row		64		//最大行数
#define		X_WIDTH 	128		//X轴的宽度
#define		Y_WIDTH 	64	    //Y轴的宽度
#define		IIC_ACK		0		//应答
#define		IIC_NO_ACK	1		//不应答

#define SSD1306_SETLOWERCOLUMNSTARTADDR 0x00
#define SSD1306_SETHIGHERCOLUMNSTARTADDR 0x10


#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SETCONTRAST_MAX 0xFF

#define SSD1306_CHARGEPUMPSETTING 0x8D
#define SSD1306_CHARGEPUMPSETTING_DEF 0x14

#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIRATIO 0xA8
#define SSD1306_SETMULTIRATIO_DUTY 0x3F

#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYOFFSET_NOT_OFFSET 0x00

#define SSD1306_SETPRECHARGEPERIOD 0xD9
#define SSD1306_SETPRECHARGEPERIOD_DEF 0x22


#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETCOMPINS_DEF 0x12

#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETVCOMDETECT_DEF 0x20

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETDISPLAYCLOCKDIV_RATIO 0xF0

#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_SETPAGESTARTADDR   0xB0

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEG0REMAP 0xA0
#define SSD1306_SEG1REMAP 0xA1

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;

typedef struct
{
	unsigned char u8Reg;
	unsigned char u8Cmd;
}SSD1306_Reg;

void drvSSD1306_DrawString(unsigned char x,unsigned char y,unsigned char *chr);
void drvSSD1306_Fill(SSD1306_COLOR_t color) ;
void drvSSD1306_UpdateScreen(void);

void drvSSD1306_GotoXY(uint16_t x, uint16_t y);

char drvSSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) ;
void drvSSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

#endif

