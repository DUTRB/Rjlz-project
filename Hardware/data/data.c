#include "data.h"


DataIn adcData= {0};

data OBS_AIR;
data Smoke_AIR;
data OBS_N2;
data Smoke_N2;


uint8_t   OBS_OVER_F      ;// //OBS�ѱ궨 =1�궨���,=0 δ�궨
uint8_t   N2_OBS_F        ;////����OBS�ѱ궨 =1�궨���,=0δ�궨
uint8_t   AIR_OBS_F       ;////����OBS�ѱ궨 =1�궨���,=0δ�궨
data			OBS_Y						;//OBSб��ֵ
uint16_t	OBS_BIAS				;//������������ֵ
uint8_t	BD_0_FLAG  			  ; //�����궨���ر�ǣ�����8�μ����ʷ���168��ָ��������м䲻�ܶϵ�
uint8_t	OBS_FLAG					;//OBS�궨��־λ,bit 0---�������̱궨,bit 1-----�������̱궨,bit 2-----OBS�궨���

