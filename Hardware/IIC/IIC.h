#ifndef __IIC_H
#define __IIC_H

//#include "gd32f3x0.h"
#include "gd32e23x.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>
//#include "gd32f350r_eval.h"

#define  u8 uint8_t
#define  u16 uint16_t
#define  u32 uint32_t

/* SCL：PA0，SDA：PA1 */
#define SCL_GPIO GPIOA
#define SCL_PIN GPIO_PIN_0
#define SDA_GPIO GPIOA
#define SDA_PIN GPIO_PIN_1

#define SDA_IN()	{gpio_mode_set(SDA_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SDA_PIN); }	//SDA：复位，PB13上拉/下拉输入    
#define SDA_OUT()	{gpio_mode_set(SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);}	//SDA：复位，PB13通用推挽输出50Hz     



/* IIC所有操作函数 */
void iic_write_scl(u8 bit);
void iic_write_sda(u8 bit);
u8 iic_read_sda(void);


void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号

void IIC_ACK(void);					//IIC发送ACK信号
void IIC_NACK(void);				//IIC不发送ACK信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节


u8 IIC_Read_Byte(u8 ack);//IIC读取一个字节
u8 IIC_Wait_ACK(void); 				//IIC等待ACK信号

void Delay_us(int us);

#endif
