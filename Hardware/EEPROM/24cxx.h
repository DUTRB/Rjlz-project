#ifndef __24CXX_H
#define __24CXX_H

//#include "gd32f3x0.h"
#include "gd32e23x.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>
//#include "gd32f350r_eval.h"
#include "IIC.h"




/* 宏定义芯片的存储阵列字地址范围，比如AT24C02为0-255 */
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

#define EE_TYPE AT24C02	//这里使用的是24c02，所以定义EE_TYPE为AT24C02

void AT24CXX_Init(void); //初始化IIC
u8 AT24CXX_ReadOneByte(u16 ReadAddr);								//在指定字地址读取一个字节的数据
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite);			//在指定字地址写入一个字节的数据
u32 AT24CXX_ReadLenByte(u16 ReadAddr, u8 Len);						//从指定字地址开始读取指定长度的数据
void AT24CXX_WriteLenByte(u16 WriteAddr, u32 DataToWrite, u8 Len);	//从指定字地址开始写入指定长度的数据
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead);   		//从指定字地址开始读取指定个数的数据
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite);		//从指定字地址开始写入指定个数的数据
u8 AT24CXX_Check(void);  //检查AT24CXX是否正常

#endif
