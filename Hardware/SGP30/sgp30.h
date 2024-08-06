/**
	@author: rubo
	@date  :24-7-18
*/
#ifndef _BSP_SGP30_H_
#define _BSP_SGP30_H_

#include "gd32e23x.h"


//端口移植
#define RCU_SDA RCU_GPIOB
#define PORT_SDA GPIOB
#define GPIO_SDA GPIO_PIN_1

#define RCU_SCL RCU_GPIOB
#define PORT_SCL GPIOB
#define GPIO_SCL GPIO_PIN_0

//设置SDA输出模式
#define SDA2_OUT()        gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
//设置SDA输入模式
#define SDA2_IN()        	gpio_mode_set(PORT_SDA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
//获取SDA引脚的电平变化
#define SDA2_GET()        gpio_input_bit_get(PORT_SDA,GPIO_SDA)
//SDA与SCL输出
#define SDA2(x)          gpio_bit_write(PORT_SDA,GPIO_SDA, (x?SET:RESET))
#define SCL2(x)          gpio_bit_write(PORT_SCL,GPIO_SCL, (x?SET:RESET))

typedef struct{
	uint32_t CO2_val;
	uint32_t TVOC_val;
}sgp30_data;

extern sgp30_data sgp_data;

void SGP30_Init(void);
uint32_t SGP30_Read(void);
void SGP30_Write_cmd(uint8_t a, uint8_t b);
void SGP30_Get_Value(void);
uint32_t SGP30_Get_CO2_Value(void);
uint32_t SGP30_Get_TVOC_Value(void);
#endif
