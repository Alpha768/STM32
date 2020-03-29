/********************************************************
  * @file        	oled.c
  * @author      	LGG
  * @version     	V1.0.0
  * @data        	2017年06月6号
  * @brief       	OELD文件
  *
********************************************************/


/*includes----------------------------------------------*/
#include "Include.h"
#include "oledfont.h" 

/*definition--------------------------------------------*/


/*
	@brief			延迟1us
	@param			无
	@retval			无
 */
static void delay(unsigned char num)
{
	unsigned char i = 10;
	while(num--)
	{
		while(i--);
	}
}


/*
	@brief			ms延迟函数
	@param			无
	@retval			无
*/
static void delay_ms(unsigned int ms)//延迟函数，MS级别
{
	unsigned int x,y;
	for(x = ms;x>0;x--)
 {
		for(y = 12000;y>0;y--);
 }
}


/*
	@brief			初始化OLED与单片机的IO接口
	@param			无
	@retval			无
 */
static void OLED_GPIO_Init(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;	//定义一个GPIO_InitTypeDef类型的结构体
	
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);	//打开GPIOC的外设时钟
	
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;	//选择控制的引脚
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//设置为通用开漏输出
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置输出速率为50MHz
	//GPIO_Init(GPIOA,&GPIO_InitStructure);	//调用库函数初始化GPIOA

	/* Configure I2C1 pins: SCL and SDA */
	
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 				  // enable clock for GPIOB
	//--------------
	//GPIOA->CRL	 &= ~(0xFFUL  << 24);					   // Clear PA5, PA7
	//GPIOA->CRL	 |=  (0x07UL  << 24);					   // PA5 Speed_50MHz,Mode_AF_OD
	//GPIOA->CRL	 |=  (0x07UL  << 28);					  // PA7 Speed_50MHz,Mode_AF_OD
	InitPortBit(PORTA,5,OUTPUT_OPEN_DRAIN,0);
	InitPortBit(PORTA,7,OUTPUT_OPEN_DRAIN,0);
	
	OLED_SCLK_Set();	//设PA5（SCL）为高电平
	OLED_SDIN_Set();	//设PA7（SDA）为高电平
}


/*
	@brief			模拟IIC起始信号
	@param			无
	@retval			无
 */
static void OLED_IIC_Start(void)
{

	OLED_SCLK_Set();	//时钟线置高
	OLED_SDIN_Set();	//信号线置高
	delay(1);	//延迟1us
	OLED_SDIN_Clr();	//信号线置低
	delay(1);	//延迟1us
	OLED_SCLK_Clr();	//时钟线置低
	delay(1);	//延迟1us
}


/*
	@brief			模拟IIC停止信号
	@param			无
	@retval			无
 */
static void OLED_IIC_Stop(void)
{
	OLED_SDIN_Clr();	//信号线置低
	delay(1);	//延迟1us
	OLED_SCLK_Set();	//时钟线置高
	delay(1);	//延迟1us
	OLED_SDIN_Set();	//信号线置高
	delay(1);	//延迟1us
}


/*
	@brief			模拟IIC读取从机应答信号
	@param			无
	@retval			无
 */
static unsigned char IIC_Wait_Ack(void)
{
	unsigned char ack;

	OLED_SCLK_Clr();	//时钟线置低
	delay(1);	//延迟1us
	OLED_SDIN_Set();	//信号线置高
	delay(1);	//延迟1us
	OLED_SCLK_Set();	//时钟线置高
	delay(1);	//延迟1us

	if(OLED_READ_SDIN())	//读取SDA的电平
		ack = IIC_NO_ACK;	//如果为1，则从机没有应答
	else
		ack = IIC_ACK;		//如果为0，则从机应答

	OLED_SCLK_Clr();//时钟线置低
	delay(1);	//延迟1us

	return ack;	//返回读取到的应答信息
}


/*
	@brief			模拟IIC主机发送应答信号
	@param			无
	@retval			无
 */
// static void IIC_Send_Ack(unsigned char ack)
// {
// 	OLED_SCLK_Clr();	//时钟线置低
// 	delay(1);	//延迟1us

// 	if(ack == IIC_ACK)	//根据情况发送应答/不应答
// 		OLED_SDIN_Clr();	//应答
// 	else
// 		OLED_SDIN_Set();	//不应答
// 	delay(1);	//延迟1us

// 	OLED_SCLK_Set();	//时钟线置高，发送应答/不应答信号
// 	delay(1);	//延迟1us
// 	OLED_SCLK_Clr();	//时钟线置低
// 	delay(1);	//延迟1us
// }


/*
	@brief			IIC写入一个字节
	@param			IIC_Byte：写入的字节
	@retval			无
 */
static void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;  //定义变量
	for(i=0;i<8;i++) //for循环8次
	{
		OLED_SCLK_Clr();	//时钟线置低，为传输数据做准备
		delay(1);	//延迟1us

		if(IIC_Byte & 0x80)	//读取最高位
		  	OLED_SDIN_Set();//最高位为1
		else
			OLED_SDIN_Clr();	//最高位为0

		IIC_Byte <<= 1;  //数据左移1位
		delay(1);	//延迟1us
		OLED_SCLK_Set(); //时钟线置高，产生上升沿，把数据发送出去
		delay(1);	//延迟1us
	}
	OLED_SCLK_Clr();	//时钟线置低
	delay(1);	//延迟1us

	while(IIC_Wait_Ack());	//从机应答
}



/*
	@brief			IIC写读取一个字节
	@param			ack：应答/不应答
	@retval			无
 */
// static unsigned char Read_IIC_Byte(unsigned char ack)
// {
// 	unsigned char data = 0;  //定义变量
// 	unsigned char i;  //定义变量

// 	OLED_SCLK_Clr();	//时钟线置低，为传输数据做准备
// 	delay(1);	//延迟1us

// 	OLED_SDIN_Set();

// 	for(i=0;i<8;i++) //for循环8次
// 	{
// 		OLED_SCLK_Set();	//时钟线置高，读取数据
// 		delay(1);	//延迟1us

// 		data <<= 1;	//左移1位
// 		if(OLED_READ_SDIN())	//读取最高位
// 		  	data |= 0x01;	//读到高位位1
// 		else 
// 			data &= 0xfe;	//读到高位为0

// 		OLED_SCLK_Clr();	//时钟线置低，准备读取下一个数据
// 		delay(1);	//延迟1us
// 	}

// 	IIC_Send_Ack(ack);	//发送应答/不应答

// 	return data;	//返回读取到的数据
// }


/*
	@brief			IIC写入命令
	@param			IIC_Command：写入的命令
	@retval			无
 */
static void Write_IIC_Command(unsigned char IIC_Command)
{
   OLED_IIC_Start();
   Write_IIC_Byte(0x78);//写入从机地址，SD0 = 0
   Write_IIC_Byte(0x00);//写入命令
   Write_IIC_Byte(IIC_Command);//数据
   OLED_IIC_Stop();  //发送停止信号
}


/*
	@brief			IIC写入数据
	@param			IIC_Data：数据
	@retval			无
 */
static void Write_IIC_Data(unsigned char IIC_Data)
{
   OLED_IIC_Start();
   Write_IIC_Byte(0x78);	//写入从机地址，SD0 = 0
   Write_IIC_Byte(0x40);	//写入数据
   Write_IIC_Byte(IIC_Data);//数据
   OLED_IIC_Stop();		//发送停止信号
}


/*
	@brief			对OLED写入一个字节
	@param			dat：数据
					cmd：1，写诶数据；0，写入命令
	@retval			无
 */
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       Write_IIC_Data(dat); //写入数据
	  }
	else {
       Write_IIC_Command(dat); //写入命令
	}
}


/*
	@brief			设置数据写入的起始行、列
	@param			x： 列的起始低地址与起始高地址；0x00~0x0f：设置起始列低地址（在页寻址模式）；
						0x10~0x1f：设置起始列高地址（在页寻址模式）
					y：起始页地址 0~7
	@retval			无
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//写入页地址
	OLED_WR_Byte((x&0x0f),OLED_CMD);  //写入列的地址  低半字节
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//写入列的地址 高半字节
}   	     	  


/*
	@brief			开显示
	@param			无
	@retval			无
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X14,OLED_CMD);  //使能，开
	OLED_WR_Byte(0XAF,OLED_CMD);  //开显示
}


/*
	@brief			关显示
	@param			无
	@retval			无
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0XAE,OLED_CMD);  //关显示
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X10,OLED_CMD);  //失能，关
}		   			 


/*
	@brief			清屏
	@param			无
	@retval			无
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //定义变量
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); //写入 0 清屏
	}
}


/*
	@brief			显示一个字符
	@param			x：起始列
					y：起始页，SIZE = 16占两页；SIZE = 12占1页
					chr：字符
	@retval			无
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //获取字符的偏移量	
		if(x>Max_Column-1){x=0;y=y+2;} //如果列数超出了范围，就从下2页的第0列开始

		if(SIZE ==16) //字符大小如果为 16 = 8*16
			{
				OLED_Set_Pos(x,y);	//从x y 开始画点
				for(i=0;i<8;i++)  //循环8次 占8列
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); //找出字符 c 的数组位置，先在第一页把列画完
				OLED_Set_Pos(x,y+1); //页数加1
				for(i=0;i<8;i++)  //循环8次
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA); //把第二页的列数画完
			}
		else 	//字符大小为 6 = 6*8
			{	
				OLED_Set_Pos(x,y+1); //一页就可以画完
				for(i=0;i<6;i++) //循环6次 ，占6列
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //把字符画完
			}
}


/*
	@brief			计算m^n
	@param			m：输入的一个数
					n：输入数的次方
	@retval			result：一个数的n次方
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			在指定的位置，显示一个指定长度大小的数
	@param			x：起始列
					y：起始页
					num：数字
					len：数字的长度
					size：显示数字的大小
	@retval			无
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;  //定义变量
	unsigned char enshow=0;		//定义变量

	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//取出输入数的每个位，由高到低
		if(enshow==0&&t<(len-1)) //enshow：是否为第一个数；t<(len-1)：判断是否为最后一个数
		{
			if(temp==0) //如果该数为0 
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');//显示 0 ；x+(size2/2)*t根据字体大小偏移的列数（8）
				continue; //跳过剩下语句，继续重复循环（避免重复显示）
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); //显示一个位；x+(size2/2)*t根据字体大小偏移的列数（8）
	}
} 


/*
	@brief			显示字符串
	@param			x：起始列
					y：起始页
					*chr：第一个字符首地址
	@retval			无
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //定义变量

	while (chr[j]!='\0') //如果不是最后一个字符
	{		
		OLED_ShowChar(x,y,chr[j]); //显示字符
		x+=8; //列数加8 ，一个字符的列数占8
		if(x>=128){x=0;y+=2;} //如果x大于等于128，切换页，从该页的第一列显示
		j++; //下一个字符
	}
}


/*
	@brief			显示中文
	@param			x：起始列；一个字体占16列
					y：起始页；一个字体占两页
					no：字体的序号
	@retval			无
 */
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0; //定义变量

	OLED_Set_Pos(x,y);	//从 x y 开始画点，先画第一页
    for(t=0;t<16;t++) //循环16次，画第一页的16列
		{
			OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//画no在数组位置的第一页16列的点
			adder+=1; //数组地址加1
     	}	
		OLED_Set_Pos(x,y+1); //画第二页
    for(t=0;t<16;t++)//循环16次，画第二页的16列
		{	
			OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//画no在数组位置的第二页16列的点
			adder+=1;//数组地址加1
        }					
}


/*
	@brief			显示图片
	@param			x0：起始列地址
					y0：起始页地址
					x1：终止列地址
					y1：终止页地址
					BMP[]：存放图片代码的数组
	@retval			无
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 	unsigned int j=0; //定义变量
 	unsigned char x,y; //定义变量
  
 	if(y1%8==0) y=y1/8;   //判断终止页是否为8的整数倍
 	 else y=y1/8+1;

		for(y=y0;y<y1;y++) //从起始页开始，画到终止页
		{
			OLED_Set_Pos(x0,y); //在页的起始列开始画
   			for(x=x0;x<x1;x++) //画x1 - x0 列
	    		{
	    			OLED_WR_Byte(BMP[j++],OLED_DATA);	//画图片的点    	
	    		}
		}
} 


/*
	@brief			OLED初始化函数
	@param			无
	@retval			无
 */				    
void OLED_Init(void)
{
	OLED_GPIO_Init();	//GPIO口初始化
 
    printf("OLED_GPIO_Init\n");
 
	delay_ms(1000);	//延迟，由于单片机上电初始化比OLED快，所以必须加上延迟，等待OLED上复位完成

	OLED_WR_Byte(0xAE,OLED_CMD);	//关闭显示

	OLED_WR_Byte(0x00,OLED_CMD);	//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);	//设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);	//设置起始行地址
	OLED_WR_Byte(0xB0,OLED_CMD);	//设置页地址

	OLED_WR_Byte(0x81,OLED_CMD); 	// 对比度设置，可设置亮度
	OLED_WR_Byte(0xFF,OLED_CMD);	//  265  

	OLED_WR_Byte(0xA1,OLED_CMD);	//设置段（SEG）的起始映射地址；column的127地址是SEG0的地址
	OLED_WR_Byte(0xA6,OLED_CMD);	//正常显示；0xa7逆显示

	OLED_WR_Byte(0xA8,OLED_CMD);	//设置驱动路数（16~64）
	OLED_WR_Byte(0x3F,OLED_CMD);	//64duty
	
	OLED_WR_Byte(0xC8,OLED_CMD);	//重映射模式，COM[N-1]~COM0扫描

	OLED_WR_Byte(0xD3,OLED_CMD);	//设置显示偏移
	OLED_WR_Byte(0x00,OLED_CMD);	//无偏移
	
	OLED_WR_Byte(0xD5,OLED_CMD);	//设置震荡器分频
	OLED_WR_Byte(0x80,OLED_CMD);	//使用默认值
	
	OLED_WR_Byte(0xD9,OLED_CMD);	//设置 Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	//使用官方推荐值
	
	OLED_WR_Byte(0xDA,OLED_CMD);	//设置 com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	//使用默认值
	
	OLED_WR_Byte(0xDB,OLED_CMD);	//设置 Vcomh，可调节亮度（默认）
	OLED_WR_Byte(0x40,OLED_CMD);	////使用官方推荐值
	
	OLED_WR_Byte(0x8D,OLED_CMD);	//设置OLED电荷泵
	OLED_WR_Byte(0x14,OLED_CMD);	//开显示
	
	OLED_WR_Byte(0xAF,OLED_CMD);	//开启OLED面板显示
	OLED_Clear();        //清屏
	OLED_Set_Pos(0,0); 	 //设置数据写入的起始行、列

	
	printf("OLED_Init\n");
}  


/*
	@brief			OLED滚屏函数，范围0~1页，水平向左
	@param			无
	@retval			无
 */	
void OLED_Scroll(void)
{
	OLED_WR_Byte(0x2E,OLED_CMD);	//关闭滚动
	OLED_WR_Byte(0x27,OLED_CMD);	//水平向左滚动
	OLED_WR_Byte(0x00,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0x00,OLED_CMD);	//起始页 0
	OLED_WR_Byte(0x00,OLED_CMD);	//滚动时间间隔
	OLED_WR_Byte(0x01,OLED_CMD);	//终止页 1
	OLED_WR_Byte(0x00,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0xFF,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0x2F,OLED_CMD);	//开启滚动
}

