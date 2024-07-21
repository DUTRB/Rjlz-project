#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <stdio.h>
#include "gd32e23x.h"
#include "IIC.h"
#include "24cxx.h"

//#include "gd32f350r_eval.h"

//定义AD模拟输入数据
typedef union {			//联合体，用于取16位数据的高八位和低八位
    uint16_t data_16;
    uint8_t data_8[2];//data_8[1]存放的是data_16的高八位，data_8[0]存放的是data_16的低八位
} data;


typedef struct
{
    data OBS;//减光率
    data LDOPowerIn;//激光值
		data LDO_Blue_PowerIn;//激光值
    data SmokeIn;//烟雾值
} DataIn;


union  BENDI_TEST
{
    unsigned long   ADLONG;
    uint16_t   ADATA[2];
    uint8_t  adata[4];
};


extern  uint8_t   OBS_OVER_F      ;//OBS已标定 =1标定完毕,=0 未标定
extern  uint8_t   N2_OBS_F        ;//氮气OBS已标定 =1标定完毕,=0未标定
extern  uint8_t   AIR_OBS_F       ;//空气OBS已标定 =1标定完毕,=0未标定
extern	data			OBS_Y						;//OBS斜率值
extern	uint16_t			OBS_BIAS				;//纯净空气本底值
extern	uint8_t	BD_0_FLAG  			  ;//氮气标定返回标记，连续8次减光率发送168后恢复正常，中间不能断电
extern	uint8_t	OBS_FLAG					;//OBS标定标志位,bit 0---空气有烟标定,bit 1-----氮气无烟标定,bit 2-----OBS标定完成


extern	DataIn	adcData						;//采集参数

extern data OBS_AIR;
extern data Smoke_AIR;
extern data OBS_N2;
extern data Smoke_N2;

void calibration_init(void);
void gpio_init(void);

#endif

