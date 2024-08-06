#include "adc.h"



/*!
    \brief      ADC configuration function
*/
void adc_config(void)
{
    /**
			PA4 -----> SMOKE_AD_IN -----> ADC-IN4
			PA5 -----> RED-POWERIN -----> ADC-IN5
			PA6 -----> BLUE-POWERIN -----> ADC-IN6
		*/
		
    // 模拟输入模式
    gpio_mode_set(GPIOA,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // ADC设置为规则组 一共使用一个通道
    adc_channel_length_config(ADC_REGULAR_CHANNEL,1);

		// 常规通道配置
    //adc_regular_channel_config(0,ADC_CHANNEL_1,ADC_SAMPLETIME_55POINT5);

		// 外部触发
    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
    
		// 配置ADC触发方式，这里使用软件触发，不使用外部触发
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_NONE);
    // 数据右对齐
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);//数据对齐方式 右对齐

    // adc 使能
    adc_enable();
    delay_1ms(1U);//延时为了让ADC稳定
    //进行ADC自动校准
    adc_calibration_enable();
    // 开启软件转换
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

/*!
    \brief      获取 ADC 采样值
*/
uint16_t get_adc(uint8_t ch)
{
		// 设置指定ADC的规则组通道，一个序列，转换时间 ADC_SAMPLETIME_7POINT5
    adc_regular_channel_config(0,ch,ADC_SAMPLETIME_55POINT5);
		// 使能指定的ADC的软件转换启动功能
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);		

    while(!adc_flag_get(ADC_FLAG_EOC));	//等待转换结束
    adc_flag_clear(ADC_FLAG_EOC);//清除转换完成标志位

    //返回最近一次ADC规则组的转换结果
    return (adc_regular_data_read() & 0xfff);
}


/*!
    \brief      获取 ADC 采样平均值
*/
uint16_t get_adc_Average(uint8_t channel)
{
    int i = 0;
    uint32_t adc_raw = 0;
    uint8_t degree = 4;  //读取的次数的幂 2^4
    for(i = 0; i < (1 << degree); i++)
    {
        //读取ADC次数
        adc_raw += get_adc(channel);
    }
    //这里循环8次读取，因此>>4位就是除以16
    adc_raw =  (adc_raw >> degree);
    return adc_raw;
}

