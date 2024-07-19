#include "gd32e23x.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>
#include "gd32f350r_eval.h"
#include "gd32e23x_it.h"
#include "PORG_RS485.h"
#include "data.h"
#include "24cxx.h"
#include "adc.h"
#include "bsp_ds18b20.h"
#include "sgp30.h"

uint8_t count = 0;	//  采集数组计数号
uint16_t SmokeIns_group[40]= {0}; // 采集数组
uint16_t Paixu_group[40] = {0};	  // 排序数组

#define LEDP_TIME 25   //激光值采集时间点
#define SMOKE_TIME 50  //烟雾值采集时间点
#define LAser_TIME 500 //关闭激光时间点
uint8_t SEC_SMOKE_TEST = 0;  //烟雾值采集时间到标记,=0 正在进行,=1 完成
uint8_t SEC_LDOP_TEST = 0;   //激光值采集时间到标记,=0 正在进行,=1 完成


uint16_t TIM_Cnt = 0;  //TIM定时器给Laser_FLASH驱动激光二极管模拟PWM

//平均值取值
uint16_t SmokeIns[32]= {0}; //32个数据，方便取平均值, >>5位


uint16_t maxn=0,minn=0x0000FFFF;  // 采集最大值和最小值
uint32_t sum=0;		    // 采集数值和
uint16_t temp_adc=0;  // 采集数值



void rcu_config(void);   //时钟配置函数
void timer_config(void); //定时器配置函数 　1ms
uint16_t get_adc(uint8_t ch);  //获得ADC数值
uint16_t get_adc_Average(uint8_t channel);   //获得ADC平均值
void gpio_init(void);   //GPIO 初始化
void nvic_config(void);  //NVIC 初始化
void send_data(uint32_t COM,const uint16_t sendbuff);  //发送数据
void mabub(uint16_t *p,uint8_t n);  //冒泡排序
void group_trans(uint16_t *group_array,uint8_t n);//将数组中数据前移一个位置，第一位置抛弃

