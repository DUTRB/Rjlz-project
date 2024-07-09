#include "data.h"


DataIn adcData= {0};

data OBS_AIR;
data Smoke_AIR;
data OBS_N2;
data Smoke_N2;


uint8_t   OBS_OVER_F      ;// //OBS已标定 =1标定完毕,=0 未标定
uint8_t   N2_OBS_F        ;////氮气OBS已标定 =1标定完毕,=0未标定
uint8_t   AIR_OBS_F       ;////空气OBS已标定 =1标定完毕,=0未标定
data			OBS_Y						;//OBS斜率值
uint16_t	OBS_BIAS				;//纯净空气本底值
uint8_t	BD_0_FLAG  			  ; //氮气标定返回标记，连续8次减光率发送168后恢复正常，中间不能断电
uint8_t	OBS_FLAG					;//OBS标定标志位,bit 0---空气有烟标定,bit 1-----氮气无烟标定,bit 2-----OBS标定完成

