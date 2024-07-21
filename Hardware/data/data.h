#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <stdio.h>
#include "gd32e23x.h"
#include "IIC.h"
#include "24cxx.h"

//#include "gd32f350r_eval.h"

//����ADģ����������
typedef union {			//�����壬����ȡ16λ���ݵĸ߰�λ�͵Ͱ�λ
    uint16_t data_16;
    uint8_t data_8[2];//data_8[1]��ŵ���data_16�ĸ߰�λ��data_8[0]��ŵ���data_16�ĵͰ�λ
} data;


typedef struct
{
    data OBS;//������
    data LDOPowerIn;//����ֵ
		data LDO_Blue_PowerIn;//����ֵ
    data SmokeIn;//����ֵ
} DataIn;


union  BENDI_TEST
{
    unsigned long   ADLONG;
    uint16_t   ADATA[2];
    uint8_t  adata[4];
};


extern  uint8_t   OBS_OVER_F      ;//OBS�ѱ궨 =1�궨���,=0 δ�궨
extern  uint8_t   N2_OBS_F        ;//����OBS�ѱ궨 =1�궨���,=0δ�궨
extern  uint8_t   AIR_OBS_F       ;//����OBS�ѱ궨 =1�궨���,=0δ�궨
extern	data			OBS_Y						;//OBSб��ֵ
extern	uint16_t			OBS_BIAS				;//������������ֵ
extern	uint8_t	BD_0_FLAG  			  ;//�����궨���ر�ǣ�����8�μ����ʷ���168��ָ��������м䲻�ܶϵ�
extern	uint8_t	OBS_FLAG					;//OBS�궨��־λ,bit 0---�������̱궨,bit 1-----�������̱궨,bit 2-----OBS�궨���


extern	DataIn	adcData						;//�ɼ�����

extern data OBS_AIR;
extern data Smoke_AIR;
extern data OBS_N2;
extern data Smoke_N2;

void calibration_init(void);
void gpio_init(void);

#endif

