#ifndef PORG_RS485_H
#define PORG_RS485_H

#endif


//#include "gd32f3x0.h"
#include "gd32e23x.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>


#define  TR_MAX_COUNTER       18   //收发最大总字节数

#define  RS485_RX   gpio_bit_reset(GPIOA, GPIO_PIN_5) //485接收
#define  RS485_TX   gpio_bit_set(GPIOA, GPIO_PIN_5)   //485发送



#define benji_add   0x0c														  //烟雾板地址

#define  uchar unsigned char
#define  uint  unsigned int

extern uchar     guanluadd                ;//管路地址，0-3
extern uchar 		 mozuadd									;//模组地址，0-15


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


//RS485控制位
extern RS485_BIT_STRUCT RS485_C;
#define RS485_RX_Statue   (RS485_C.a)//rs485需要切换接收,=0,不需要,=1,需要
#define RS485_NEED				(RS485_C.b)//RS485需处理标,记＝1,需要处理,＝0,不需处理


void int_RX(void);
void int_TX(void);
void RS485_PROG(void);  //rs485通信处理子程序
void out_rs485(uchar i);
void RS485_INI_DATA(void);

