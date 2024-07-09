/**
  ******************************************************************************
  * @file    		24cxx.c
  * @author 		GYP
  * @version 		V1.0
  * @date   		2021.03.01
  * @brief			EEPROM外部存储AT24C16
  * @attention
  ******************************************************************************
 **/

#include "24cxx.h"

/**
  ******************************************************************************
  * @brief			初始化AT24CXX使用的IIC接口
  * @param			None
  * @retval  		None
  * @attention
  ******************************************************************************
 **/
void AT24CXX_Init(void)
{
    IIC_Init();
}


/**
  ******************************************************************************
  * @brief			在AT24CXX指定的字地址读取一个字节的数据
  * @param			ReadAddr：读数据的字地址
  * @retval  		读到的数据
  * @attention		严格按照AT24CXX读字节时序编写程序
  ******************************************************************************
 **/

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
    u8 temp = 0;
    IIC_Start();

    /* 芯片存储容量>16K时，需要用2字节的空间表示存储阵列字地址
       u16 ADDR， ADDR>>8 或 ADDR/256：取高8位， ADDR%256：取低8位 */
    /* 芯片存储容量<=16K时，只需要用1字节的空间表示存储阵列字地址，不够的用器件地址补上，
       4K容量：用器件地址（第0位最低位为R/W位，第1、2、3位为可编程位）的第1位表示字地址，
       8K容量：用器件地址的第1、2位表示字地址，
       16K容量：用器件地址的第1、2、3位表示字地址。 */
    if(EE_TYPE > AT24C16)
    {
        IIC_Send_Byte(0XA0);		//发送器件地址（写），进入发送模式
        IIC_Wait_ACK();
        IIC_Send_Byte(ReadAddr >> 8);	//发送字地址的高8位
    }
    else
    {
        IIC_Send_Byte(0XA0 + ((ReadAddr/256)<<1));	//发送器件地址+字地址高8位（写），(ReadAddr/256)<<1)：取字地址高8位并左移1位（因为器件地址第0位是R/W位），然后加到0XA0上
    }

    IIC_Wait_ACK();
    IIC_Send_Byte(ReadAddr % 256);	//发送字地址的低8位
    IIC_Wait_ACK();
    IIC_Start();
    IIC_Send_Byte(0XA1);			//发送器件地址（读），进入接收模式
    IIC_Wait_ACK();
    temp = IIC_Read_Byte(0);		//接收数据并发送NACK
    IIC_Stop();			//主机产生一个停止信号
    return temp;
}



/**
  ******************************************************************************
  * @brief			在AT24CXX指定的字地址写入一个字节的数据
  * @param			WriteAddr：写数据的字地址，DataToWrite：要写入的数据
  * @retval  		None
  * @attention		严格按照AT24CXX写字节时序编写程序
  ******************************************************************************
 **/

void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);	    //发送器件地址（写）
        IIC_Wait_ACK();
        IIC_Send_Byte(WriteAddr >> 8);
    }
    else
    {
        IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));
    }
    IIC_Wait_ACK();
    IIC_Send_Byte(WriteAddr % 256);
    IIC_Wait_ACK();
    IIC_Send_Byte(DataToWrite);
    IIC_Wait_ACK();
    IIC_Stop();		//产生一个停止信号
    delay_1ms(10);
}



/**
  ******************************************************************************
  * @brief			从AT24CXX指定字地址开始读取指定长度的数据
  * @param			ReadAddr：开始读数据的字地址，Len：要读出数据的长度（2或4）
  * @retval  		读到的数据
  * @attention		该函数用于读出16bit或者32bit的数据，把高字地址中的数据存在temp的高位中
  ******************************************************************************
 **/
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len)
{
    u8 i;
    u32 temp = 0;
    for(i=0; i<Len; i++)
    {
        temp <<= 8;		//要先移位后存数据，否则最后会移出去数据
        temp += AT24CXX_ReadOneByte(ReadAddr + Len-i-1);
    }
    return temp;
}



/**
  ******************************************************************************
  * @brief			从AT24CXX指定字地址开始写入指定长度的数据
  * @param			WriteAddr：开始写数据的字地址，Len：要写入数据的长度（2或4），DataToWrite：要写入的数据
  * @retval  		None
  * @attention		该函数用于写入16bit或者32bit的数据
  ******************************************************************************
 **/

void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len)
{
    u8 i;
    for(i=0; i<Len; i++)
    {
        AT24CXX_WriteOneByte(WriteAddr + i, (DataToWrite >> (8*i)) & 0XFF);
    }
}



/**
  ******************************************************************************
  * @brief			从AT24CXX指定字地址开始读取指定个数的数据
  * @param			ReadAddr：开始读数据的字地址，NumToRead：要读出数据的个数，pBuffer：存数据数组的首地址
  * @retval
  * @attention
  ******************************************************************************
 **/

void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
    while(NumToRead)
    {
        *pBuffer = AT24CXX_ReadOneByte(ReadAddr++);	//ReadAddr++是先赋值后++，比如b=i++; 等价于b=i; i++;
        pBuffer++;		//数组地址+1，指向下一个元素
        NumToRead--;
    }
}



/**
  ******************************************************************************
  * @brief			从AT24CXX指定字地址开始写入指定个数的数据
  * @param			WriteAddr：开始写数据的字地址，NumToWrite：要写入数据的个数，pBuffer：存数据数组的首地址
  * @retval
  * @attention
  ******************************************************************************
 **/

void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
    while(NumToWrite--)		//先赋值后--，和把NumToWrite--写进循环里一样
    {
        AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}



/**
  ******************************************************************************
  * @brief			检查AT24CXX是否正常
  * @param			None
  * @retval  		1：检测失败，0：检测成功
  * @attention		这里用了AT24C02的最后一个字地址(255)来存储检查用的标志字，如果用其它芯片，这个地址要修改
  ******************************************************************************
 **/

u8 AT24CXX_Check(void)
{
    u8 temp;
    /* 这样写，就只需要在第一次初始化后的检查中写、读，后面的检查只需要读不需要写 */
    temp = AT24CXX_ReadOneByte(255);	//如果不是第一次初始化，是在使用过程中想检查是否正常，直接读即可，不需要再写入。避免了每次检查都写入。
    if(temp == 0X55) return 0;
    else	//如果是第一次初始化，能写能读（随便写什么），说明正常
    {
        AT24CXX_WriteOneByte(255, 0X55);
        temp = AT24CXX_ReadOneByte(255);
        if(temp == 0X55) return 0;
    }
    return 1;
}
