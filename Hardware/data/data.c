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



uint8_t	temp8;   // 临时变量
uint16_t temp16; // 临时变量

void calibration_init() {
    temp8 = AT24CXX_ReadOneByte(255);	//如果不是第一次初始化，是在使用过程中想检查是否正常，直接读即可，不需要再写入。避免了每次检查都写入。
    if(temp8 == 0X55)
    {
        //读取
        OBS_Y.data_16 = AT24CXX_ReadLenByte(0x09,2);
        OBS_BIAS = AT24CXX_ReadLenByte(0x0b,2);
        OBS_FLAG = AT24CXX_ReadOneByte(0x0d);
        if(OBS_FLAG & 0x01) AIR_OBS_F = 1;
        else AIR_OBS_F = 0;
        if(OBS_FLAG & 0x02) N2_OBS_F = 1;
        else N2_OBS_F = 0;
        if(OBS_FLAG & 0x04) OBS_OVER_F = 1;
        else OBS_OVER_F = 0;

        Smoke_N2.data_16 = AT24CXX_ReadLenByte(0x01,2);//读当前氮气情况下OBS值
        OBS_N2.data_16 = AT24CXX_ReadLenByte(0x03,2);

        Smoke_AIR.data_16 = AT24CXX_ReadLenByte(0x05,2);//读当前空气情况下OBS值
        OBS_AIR.data_16 = AT24CXX_ReadLenByte(0x07,2);
    }
    else	//如果是第一次初始化
    {
        AT24CXX_WriteOneByte(255, 0X55);
        temp8 = AT24CXX_ReadOneByte(255);
        if(temp8 != 0X55) AT24CXX_WriteOneByte(255, 0X55);

        AIR_OBS_F  = 0 ;         //清零空气OBS偏置已经标定标记，OBS_FLAG.0,空气标记
        N2_OBS_F   = 0 ;         //清零氮气OBS偏置已经标定标记，OBS_FLAG.1,氮气标记
        OBS_OVER_F = 0 ;         //清零OBS标定计算完毕标记，OBS_FLAG.2,计算完毕标记

        OBS_FLAG   = 0 ;       	 //清零OBS标定保存数据
        temp8 = AT24CXX_ReadOneByte(0x0d);
        if(temp8 != OBS_FLAG)	AT24CXX_WriteOneByte(0x0d,OBS_FLAG);//写当前OBS标定情况

        OBS_Y.data_16 = 0;
        temp16 = AT24CXX_ReadLenByte(0x09,2);
        if(temp16 != OBS_Y.data_16)	AT24CXX_WriteLenByte(0x09,OBS_Y.data_16,2);

        OBS_BIAS = 0;
        temp16 = AT24CXX_ReadLenByte(0x0b,2);
        if(temp16 != OBS_BIAS)	AT24CXX_WriteLenByte(0x0b,OBS_BIAS,2);
    }
		return;
}

/*!
    \brief      GPIO 初始化
*/
void gpio_init(void)
{
    //rcu_periph_clock_enable(RCU_GPIOA);
		// 源程序中为PA0，对应原电路图中的标定线1 在新版中为 PB7
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7);
    gpio_bit_set(GPIOB, GPIO_PIN_7);

    /*RS485-C PB4 高1为发送，低0为接收*/  
		// 源程序中为 PA5
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_4);

    /******************* 双色 Laser 引脚闪烁配置 **********************/
	
		//BLUE-Laser_FLASH PA15
			gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
			gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
			//gpio_bit_reset(GPIOA, GPIO_PIN_15);
			gpio_bit_reset(GPIOA, GPIO_PIN_15);
	
    //RED-Laser_FLASH PA8
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    //gpio_bit_set(GPIOA, GPIO_PIN_8);
    gpio_bit_reset(GPIOA, GPIO_PIN_8);

    
}

