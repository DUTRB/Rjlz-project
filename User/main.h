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

uint8_t count = 0;	//  �ɼ����������
uint16_t SmokeIns_group[40]= {0}; // �ɼ�����
uint16_t Paixu_group[40] = {0};	  // ��������

#define LEDP_TIME 25   //����ֵ�ɼ�ʱ���
#define SMOKE_TIME 50  //����ֵ�ɼ�ʱ���
#define LAser_TIME 500 //�رռ���ʱ���
uint8_t SEC_SMOKE_TEST = 0;  //����ֵ�ɼ�ʱ�䵽���,=0 ���ڽ���,=1 ���
uint8_t SEC_LDOP_TEST = 0;   //����ֵ�ɼ�ʱ�䵽���,=0 ���ڽ���,=1 ���


uint16_t TIM_Cnt = 0;  //TIM��ʱ����Laser_FLASH�������������ģ��PWM

//ƽ��ֵȡֵ
uint16_t SmokeIns[32]= {0}; //32�����ݣ�����ȡƽ��ֵ, >>5λ


uint16_t maxn=0,minn=0x0000FFFF;  // �ɼ����ֵ����Сֵ
uint32_t sum=0;		    // �ɼ���ֵ��
uint16_t temp_adc=0;  // �ɼ���ֵ



void rcu_config(void);   //ʱ�����ú���
void timer_config(void); //��ʱ�����ú��� ��1ms
uint16_t get_adc(uint8_t ch);  //���ADC��ֵ
uint16_t get_adc_Average(uint8_t channel);   //���ADCƽ��ֵ
void gpio_init(void);   //GPIO ��ʼ��
void nvic_config(void);  //NVIC ��ʼ��
void send_data(uint32_t COM,const uint16_t sendbuff);  //��������
void mabub(uint16_t *p,uint8_t n);  //ð������
void group_trans(uint16_t *group_array,uint8_t n);//������������ǰ��һ��λ�ã���һλ������

