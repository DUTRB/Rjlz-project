#ifndef PORG_RS485_H
#define PORG_RS485_H

#endif


//#include "gd32f3x0.h"
#include "gd32e23x.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>


#define  TR_MAX_COUNTER       18   //�շ�������ֽ���

#define  RS485_RX   gpio_bit_reset(GPIOA, GPIO_PIN_5) //485����
#define  RS485_TX   gpio_bit_set(GPIOA, GPIO_PIN_5)   //485����



#define benji_add   0x0c														  //������ַ

#define  uchar unsigned char
#define  uint  unsigned int

extern uchar     guanluadd                ;//��·��ַ��0-3
extern uchar 		 mozuadd									;//ģ���ַ��0-15


typedef struct
{
    uint8_t a:1;    //bit 0
    uint8_t b:1;    //bit 1
    uint8_t c:1;    //bit 2
    uint8_t d:1;    //bit 3
    uint8_t e:1;    //bit 4
    uint8_t f:1;    //bit 5
    uint8_t g:1;    //bit 6
    uint8_t h:1;    //bit 7

} RS485_BIT_STRUCT;


//RS485����λ
extern RS485_BIT_STRUCT RS485_C;
#define RS485_RX_Statue   (RS485_C.a)//rs485��Ҫ�л�����,=0,����Ҫ,=1,��Ҫ
#define RS485_NEED				(RS485_C.b)//RS485�账���,�ǣ�1,��Ҫ����,��0,���账��


void int_RX(void);
void int_TX(void);
void RS485_PROG(void);  //rs485ͨ�Ŵ����ӳ���
void out_rs485(uchar i);
void RS485_INI_DATA(void);

