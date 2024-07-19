/**
	@author: rubo
	@date  :24-7-18
*/
#ifndef _BSP_DS18B20_H_
#define _BSP_DS18B20_H_

#include "gd32e23x.h"


//端口移植
#define RCU_DQ  RCU_GPIOA
#define PORT_DQ GPIOA
#define GPIO_DQ GPIO_PIN_3


//设置DQ输出模式
#define DQ_OUT()        	gpio_mode_set(PORT_DQ,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_DQ)
//设置DQ输入模式
#define DQ_IN()            	gpio_mode_set(PORT_DQ,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_DQ)
//获取DQ引脚的电平变化
#define DQ_GET()        	gpio_input_bit_get(PORT_DQ,GPIO_DQ)
//DQ输出
#define DQ(x)       		gpio_bit_write(PORT_DQ,GPIO_DQ, (x?SET:RESET))

void DS18B20_Reset(void);
uint8_t DS18B20_Check(void);
int DS18B20_GPIO_Init(void);
void DS18B20_Start(void);
float DS18B20_GetTemperture(void);
#endif
