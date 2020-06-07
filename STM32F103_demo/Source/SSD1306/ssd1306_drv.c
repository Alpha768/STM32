#include "Include.h"
#include "oledfont.h" 

/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

/* Private variable */
static SSD1306_t SSD1306;



void drvSSD1306_Write_Command(unsigned char ssd1306cmd)
{
   SSD1306_i2c_Start();
   SSD1306_i2c_SendByte(0x78);//д��ӻ���ַ��SD0 = 0
   SSD1306_i2c_SendByte(0x00);//д������
   SSD1306_i2c_SendByte(ssd1306cmd);//����
   SSD1306_i2c_Stop();  //����ֹͣ�ź�
}

void drvSSD1306_Write_Data(unsigned char ssd1306Data)
{
   SSD1306_i2c_Start();
   SSD1306_i2c_SendByte(0x78);	//д��ӻ���ַ��SD0 = 0
   SSD1306_i2c_SendByte(0x40);	//д������
   SSD1306_i2c_SendByte(ssd1306Data);//����
   SSD1306_i2c_Stop();		//����ֹͣ�ź�
}

void drvSSD1306_Write_BurstData( BYTE count, BYTE * buffer )
{
   SSD1306_i2c_Start();
   SSD1306_i2c_SendByte(0x78);	//д��ӻ���ַ��SD0 = 0
   SSD1306_i2c_SendByte(0x40);	//д������
   SSD1306_i2c_BurstWrite(count,buffer);//����
   SSD1306_i2c_Stop();		//����ֹͣ�ź�
}

void drvSSD1306_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       drvSSD1306_Write_Data(dat); //д������
	  }
	else {
       drvSSD1306_Write_Command(dat); //д������
	}
}

void drvSSD1306_Fill(SSD1306_COLOR_t color) {
	/* Set memory */
	memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

void drvSSD1306_UpdateScreen(void)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
	{
        drvSSD1306_Write_Command(0xB0 + i);
        drvSSD1306_Write_Command(0x00);
        drvSSD1306_Write_Command(0x10);
        drvSSD1306_Write_BurstData(SSD1306_WIDTH,&SSD1306_Buffer[SSD1306_WIDTH*i]);
    }
}

void SSD1306_ToggleInvert(void) {
	uint16_t i;

	/* Toggle invert */
	SSD1306.Inverted = !SSD1306.Inverted;

	/* Do memory toggle */
	for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
		SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
	}
}

void drvSSD1306_Clear(void)  
{  
	unsigned char i,n;		    //�������
	for(i=0;i<8;i++)  
	{  
		drvSSD1306_WR_Byte (0xb0+i,SSD1306_CMD);    //��0~7ҳ����д��
		drvSSD1306_WR_Byte (0x00,SSD1306_CMD);      //�е͵�ַ
		drvSSD1306_WR_Byte (0x10,SSD1306_CMD);      //�иߵ�ַ  
		for(n=0;n<130;n++)
			drvSSD1306_WR_Byte(0,SSD1306_DATA); //д�� 0 ����
	}
}

void drvSSD1306_Set_Pos(unsigned char x, unsigned char y) 
{ 
	drvSSD1306_WR_Byte(0xb0+y,SSD1306_CMD);//д��ҳ��ַ
	drvSSD1306_WR_Byte(((x+2)&0x0f),SSD1306_CMD);  //д���еĵ�ַ  �Ͱ��ֽ�
	drvSSD1306_WR_Byte((((x+2)&0xf0)>>4)|0x10,SSD1306_CMD);//д���еĵ�ַ �߰��ֽ�
}   

void drvSSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR_t)!color;
	}

	/* Set color */
	if (color == SSD1306_COLOR_WHITE) {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void drvSSD1306_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

char drvSSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
		SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				drvSSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
			} else {
				drvSSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
			}
		}
	}

	/* Increase pointer */
	SSD1306.CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
}

char drvSSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (drvSSD1306_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}

void drvSSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			drvSSD1306_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			drvSSD1306_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		drvSSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void drvSSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	/* Draw 4 lines */
	drvSSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	drvSSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	drvSSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	drvSSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void drvSSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	uint8_t i;

	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		drvSSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void drvSSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	/* Draw lines */
	drvSSD1306_DrawLine(x1, y1, x2, y2, color);
	drvSSD1306_DrawLine(x2, y2, x3, y3, color);
	drvSSD1306_DrawLine(x3, y3, x1, y1, color);
}

void drvSSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		drvSSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
void drvSSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    drvSSD1306_DrawPixel(x0, y0 + r, c);
    drvSSD1306_DrawPixel(x0, y0 - r, c);
    drvSSD1306_DrawPixel(x0 + r, y0, c);
    drvSSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drvSSD1306_DrawPixel(x0 + x, y0 + y, c);
        drvSSD1306_DrawPixel(x0 - x, y0 + y, c);
        drvSSD1306_DrawPixel(x0 + x, y0 - y, c);
        drvSSD1306_DrawPixel(x0 - x, y0 - y, c);

        drvSSD1306_DrawPixel(x0 + y, y0 + x, c);
        drvSSD1306_DrawPixel(x0 - y, y0 + x, c);
        drvSSD1306_DrawPixel(x0 + y, y0 - x, c);
        drvSSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}

void drvSSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    drvSSD1306_DrawPixel(x0, y0 + r, c);
    drvSSD1306_DrawPixel(x0, y0 - r, c);
    drvSSD1306_DrawPixel(x0 + r, y0, c);
    drvSSD1306_DrawPixel(x0 - r, y0, c);
    drvSSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drvSSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        drvSSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        drvSSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        drvSSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

void drvSSD1306_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{     
	unsigned char c=0,i=0;	
		c=chr-' '; //��ȡ�ַ���ƫ����	
		if(x>Max_Column-1){x=0;y=y+2;} //������������˷�Χ���ʹ���2ҳ�ĵ�0�п�ʼ

		if(SIZE ==16) //�ַ���С���Ϊ 16 = 8*16
			{
				drvSSD1306_Set_Pos(x,y);	//��x y ��ʼ����
				for(i=0;i<8;i++)  //ѭ��8�� ռ8��
				drvSSD1306_WR_Byte(F8X16[c*16+i],SSD1306_DATA); //�ҳ��ַ� c ������λ�ã����ڵ�һҳ���л���
				drvSSD1306_Set_Pos(x,y+1); //ҳ����1
				for(i=0;i<8;i++)  //ѭ��8��
				drvSSD1306_WR_Byte(F8X16[c*16+i+8],SSD1306_DATA); //�ѵڶ�ҳ����������
			}
		else 	//�ַ���СΪ 6 = 6*8
			{	
				drvSSD1306_Set_Pos(x,y+1); //һҳ�Ϳ��Ի���
				for(i=0;i<6;i++) //ѭ��6�� ��ռ6��
				drvSSD1306_WR_Byte(F6x8[c][i],SSD1306_DATA); //���ַ�����
			}
}

/*
	@brief			����m^n
	@param			m�������һ����
					n���������Ĵη�
	@retval			result��һ������n�η�
 */
unsigned int drvSSD1306_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			��ָ����λ�ã���ʾһ��ָ�����ȴ�С����
	@param			x����ʼ��
					y����ʼҳ
					num������
					len�����ֵĳ���
					size����ʾ���ֵĴ�С
	@retval			��
 */		  
void drvSSD1306_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;  //�������
	unsigned char enshow=0;		//�������

	for(t=0;t<len;t++)
	{
		temp=(num/drvSSD1306_pow(10,len-t-1))%10;//ȡ����������ÿ��λ���ɸߵ���
		if(enshow==0&&t<(len-1)) //enshow���Ƿ�Ϊ��һ������t<(len-1)���ж��Ƿ�Ϊ���һ����
		{
			if(temp==0) //�������Ϊ0 
			{
				drvSSD1306_ShowChar(x+(size/2)*t,y,' ');//��ʾ 0 ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
				continue; //����ʣ����䣬�����ظ�ѭ���������ظ���ʾ��
			}else enshow=1; 
		}
	 	drvSSD1306_ShowChar(x+(size/2)*t,y,temp+'0'); //��ʾһ��λ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
	}
} 
/*
	@brief			��ʾ�ַ���
	@param			x����ʼ��
					y����ʼҳ
					*chr����һ���ַ��׵�ַ
	@retval			��
 */
void drvSSD1306_DrawString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //�������

	while (chr[j]!='\0') //����������һ���ַ�
	{		
		drvSSD1306_ShowChar(x,y,chr[j]); //��ʾ�ַ�
		x+=8; //������8 ��һ���ַ�������ռ8
		if(x>=128){x=0;y+=2;} //���x���ڵ���128���л�ҳ���Ӹ�ҳ�ĵ�һ����ʾ
		j++; //��һ���ַ�
	}
}

/*
	@brief			��ʾ����
	@param			x����ʼ�У�һ������ռ16��
					y����ʼҳ��һ������ռ��ҳ
					no����������
	@retval			��
 */
void drvSSD1306_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0; //�������

	drvSSD1306_Set_Pos(x,y);	//�� x y ��ʼ���㣬�Ȼ���һҳ
    for(t=0;t<16;t++) //ѭ��16�Σ�����һҳ��16��
		{
			drvSSD1306_WR_Byte(Hzk[2*no][t],SSD1306_CMD);//��no������λ�õĵ�һҳ16�еĵ�
			adder+=1; //�����ַ��1
     	}	
		drvSSD1306_Set_Pos(x,y+1); //���ڶ�ҳ
    for(t=0;t<16;t++)//ѭ��16�Σ����ڶ�ҳ��16��
		{	
			drvSSD1306_WR_Byte(Hzk[2*no+1][t],SSD1306_CMD);//��no������λ�õĵڶ�ҳ16�еĵ�
			adder+=1;//�����ַ��1
        }					
}
/*
	@brief			��ʾͼƬ
	@param			x0����ʼ�е�ַ
					y0����ʼҳ��ַ
					x1����ֹ�е�ַ
					y1����ֹҳ��ַ
					BMP[]�����ͼƬ���������
	@retval			��
 */
void drvSSD1306_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 	unsigned int j=0; //�������
 	unsigned char x,y; //�������
  
 	if(y1%8==0) y=y1/8;   //�ж���ֹҳ�Ƿ�Ϊ8��������
 	 else y=y1/8+1;

		for(y=y0;y<y1;y++) //����ʼҳ��ʼ��������ֹҳ
		{
			drvSSD1306_Set_Pos(x0,y); //��ҳ����ʼ�п�ʼ��
   			for(x=x0;x<x1;x++) //��x1 - x0 ��
	    		{
	    			drvSSD1306_WR_Byte(BMP[j++],SSD1306_CMD);	//��ͼƬ�ĵ�    	
	    		}
		}
} 

/* Write command */
#define SSD1306_WRITECOMMAND(command)      drvSSD1306_WR_Byte(command,SSD1306_CMD)

void drvSSD1306_ON(void) {
	SSD1306_WRITECOMMAND(0x8D);
	SSD1306_WRITECOMMAND(0x14);
	SSD1306_WRITECOMMAND(0xAF);
}
void drvSSD1306_OFF(void) {
	SSD1306_WRITECOMMAND(0x8D);
	SSD1306_WRITECOMMAND(0x10);
	SSD1306_WRITECOMMAND(0xAE);
}

static SSD1306_Reg tSSD1306RegTbl[]=
{
	{SSD1306_DISPLAYOFF,SSD1306_CMD}, //0xAE
	{SSD1306_MEMORYMODE,SSD1306_CMD}, //0x20
	{SSD1306_SETHIGHCOLUMN,SSD1306_CMD},//0x10
	{SSD1306_SETPAGESTARTADDR,SSD1306_CMD},  //0xB0
	{SSD1306_COMSCANDEC,SSD1306_CMD},//0xC8
	{SSD1306_SETLOWERCOLUMNSTARTADDR,SSD1306_CMD},//0x00
	{SSD1306_SETHIGHERCOLUMNSTARTADDR,SSD1306_CMD},//0x10
	{SSD1306_SETSTARTLINE,SSD1306_CMD},//0x40
	{SSD1306_SETCONTRAST,SSD1306_CMD},//0x81
	{SSD1306_SETCONTRAST_MAX,SSD1306_CMD},//0xFF
	
	{SSD1306_SEG1REMAP,SSD1306_CMD},//0xA1
	{SSD1306_NORMALDISPLAY,SSD1306_CMD},//0xA6
	{SSD1306_SETMULTIRATIO,SSD1306_CMD},//0xA8
	{SSD1306_SETMULTIRATIO_DUTY,SSD1306_CMD},//0x3F
	
	{SSD1306_DISPLAYALLON_RESUME,SSD1306_CMD},//0xA4
	
	{SSD1306_SETDISPLAYOFFSET,SSD1306_CMD},//0xD3
	{SSD1306_SETDISPLAYOFFSET_NOT_OFFSET,SSD1306_CMD},//0x00
	
	{SSD1306_SETDISPLAYCLOCKDIV,SSD1306_CMD},//0xD5
	{SSD1306_SETDISPLAYCLOCKDIV_RATIO,SSD1306_CMD},//0xF0
	
	{SSD1306_SETPRECHARGEPERIOD,SSD1306_CMD},//0xD9
	{SSD1306_SETPRECHARGEPERIOD_DEF,SSD1306_CMD},//0x22
	
	{SSD1306_SETCOMPINS,SSD1306_CMD},//0xDA
	{SSD1306_SETCOMPINS_DEF,SSD1306_CMD},//0x12

	{SSD1306_SETVCOMDETECT,SSD1306_CMD},//0xDB
	{SSD1306_SETVCOMDETECT_DEF,SSD1306_CMD},//0x20

	{SSD1306_CHARGEPUMPSETTING,SSD1306_CMD},//0x8D
	{SSD1306_CHARGEPUMPSETTING_DEF,SSD1306_CMD},//0x14

};

void drvSSD1306_Init(void)
{
	unsigned char i;
	//OLED_GPIO_Init();	//GPIO�ڳ�ʼ��
 
    SSD1306_I2C_Init();
	
	Delay1Ms(200);	//�ӳ٣����ڵ�Ƭ���ϵ��ʼ����OLED�죬���Ա�������ӳ٣��ȴ�OLED�ϸ�λ���
#if 1
    /* Init LCD */
	for(i=0;i<sizeof(tSSD1306RegTbl)/sizeof(SSD1306_Reg);i++)
		drvSSD1306_WR_Byte(tSSD1306RegTbl[i].u8Reg,tSSD1306RegTbl[i].u8Cmd);
#if 0
    SSD1306_WRITECOMMAND(0xAE); //display off
    SSD1306_WRITECOMMAND(0x20); //Set Memory Addressing Mode
    SSD1306_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    SSD1306_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
    SSD1306_WRITECOMMAND(0x00); //---set low column address
    SSD1306_WRITECOMMAND(0x10); //---set high column address
    SSD1306_WRITECOMMAND(0x40); //--set start line address
    SSD1306_WRITECOMMAND(0x81); //--set contrast control register
    SSD1306_WRITECOMMAND(0xFF);
	
    SSD1306_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
    SSD1306_WRITECOMMAND(0xA6); //--set normal display
    SSD1306_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
    SSD1306_WRITECOMMAND(0x3F); //
    
    SSD1306_WRITECOMMAND(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    SSD1306_WRITECOMMAND(0xD3); //-set display offset
    SSD1306_WRITECOMMAND(0x00); //-not offset
    SSD1306_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
    SSD1306_WRITECOMMAND(0xF0); //--set divide ratio
    SSD1306_WRITECOMMAND(0xD9); //--set pre-charge period
    SSD1306_WRITECOMMAND(0x22); //
    
    SSD1306_WRITECOMMAND(0xDA); //--set com pins hardware configuration
    SSD1306_WRITECOMMAND(0x12);
	
    SSD1306_WRITECOMMAND(0xDB); //--set vcomh
    SSD1306_WRITECOMMAND(0x20); //0x20,0.77xVcc
    
    SSD1306_WRITECOMMAND(0x8D); //--set DC-DC enable
    SSD1306_WRITECOMMAND(0x14); //
#endif
	/* Clear screen */
	drvSSD1306_Fill(SSD1306_COLOR_BLACK);

	/* Update screen */
	drvSSD1306_UpdateScreen();

	/* Set default values */
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;

	/* Initialized OK */
	SSD1306.Initialized = 1;
	
    SSD1306_WRITECOMMAND(0xAF); //--turn on SSD1306 panel
#endif
	
#if 0
	drvSSD1306_WR_Byte(0xAE,SSD1306_CMD);	//�ر���ʾ

	drvSSD1306_WR_Byte(0x00,SSD1306_CMD);	//���õ��е�ַ
	drvSSD1306_WR_Byte(0x10,SSD1306_CMD);	//���ø��е�ַ
	drvSSD1306_WR_Byte(0x40,SSD1306_CMD);	//������ʼ�е�ַ
	drvSSD1306_WR_Byte(0xB0,SSD1306_CMD);	//����ҳ��ַ

	drvSSD1306_WR_Byte(0x81,SSD1306_CMD); 	// �Աȶ����ã�����������
	drvSSD1306_WR_Byte(0xFF,SSD1306_CMD);	//  265  

	drvSSD1306_WR_Byte(0xA1,SSD1306_CMD);	//���öΣ�SEG������ʼӳ���ַ��column��127��ַ��SEG0�ĵ�ַ
	drvSSD1306_WR_Byte(0xA6,SSD1306_CMD);	//������ʾ��0xa7����ʾ

	drvSSD1306_WR_Byte(0xA8,SSD1306_CMD);	//��������·����16~64��
	drvSSD1306_WR_Byte(0x3F,SSD1306_CMD);	//64duty
	
	drvSSD1306_WR_Byte(0xC8,SSD1306_CMD);	//��ӳ��ģʽ��COM[N-1]~COM0ɨ��

	drvSSD1306_WR_Byte(0xD3,SSD1306_CMD);	//������ʾƫ��
	drvSSD1306_WR_Byte(0x00,SSD1306_CMD);	//��ƫ��
	
	drvSSD1306_WR_Byte(0xD5,SSD1306_CMD);	//����������Ƶ
	drvSSD1306_WR_Byte(0x80,SSD1306_CMD);	//ʹ��Ĭ��ֵ
	
	drvSSD1306_WR_Byte(0xD9,SSD1306_CMD);	//���� Pre-Charge Period
	drvSSD1306_WR_Byte(0xF1,SSD1306_CMD);	//ʹ�ùٷ��Ƽ�ֵ
	
	drvSSD1306_WR_Byte(0xDA,SSD1306_CMD);	//���� com pin configuartion
	drvSSD1306_WR_Byte(0x12,SSD1306_CMD);	//ʹ��Ĭ��ֵ
	
	drvSSD1306_WR_Byte(0xDB,SSD1306_CMD);	//���� Vcomh���ɵ������ȣ�Ĭ�ϣ�
	drvSSD1306_WR_Byte(0x40,SSD1306_CMD);	////ʹ�ùٷ��Ƽ�ֵ
	
	drvSSD1306_WR_Byte(0x8D,SSD1306_CMD);	//����OLED��ɱ�
	drvSSD1306_WR_Byte(0x14,SSD1306_CMD);	//����ʾ
	drvSSD1306_Clear();        //����
	
	drvSSD1306_WR_Byte(0xAF,SSD1306_CMD);	//����OLED�����ʾ
	drvSSD1306_Set_Pos(0,0); 	 //��������д�����ʼ�С���
#endif


	//drvSSD1306_DrawFilledCircle(5,5,5,SSD1306_COLOR_WHITE);
	
	//drvSSD1306_UpdateScreen();
	printf("drvSSD1306_Init\n");
}  


