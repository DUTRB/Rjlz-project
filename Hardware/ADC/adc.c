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
		
    // ģ������ģʽ
    gpio_mode_set(GPIOA,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // ADC����Ϊ������ һ��ʹ��һ��ͨ��
    adc_channel_length_config(ADC_REGULAR_CHANNEL,1);

		// ����ͨ������
    //adc_regular_channel_config(0,ADC_CHANNEL_1,ADC_SAMPLETIME_55POINT5);

		// �ⲿ����
    adc_external_trigger_config(ADC_REGULAR_CHANNEL,ENABLE);
    
		// ����ADC������ʽ������ʹ�������������ʹ���ⲿ����
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,ADC_EXTTRIG_REGULAR_NONE);
    // �����Ҷ���
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);//���ݶ��뷽ʽ �Ҷ���

    // adc ʹ��
    adc_enable();
    delay_1ms(1U);//��ʱΪ����ADC�ȶ�
    //����ADC�Զ�У׼
    adc_calibration_enable();
    // �������ת��
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

/*!
    \brief      ��ȡ ADC ����ֵ
*/
uint16_t get_adc(uint8_t ch)
{
		// ����ָ��ADC�Ĺ�����ͨ����һ�����У�ת��ʱ�� ADC_SAMPLETIME_7POINT5
    adc_regular_channel_config(0,ch,ADC_SAMPLETIME_55POINT5);
		// ʹ��ָ����ADC�����ת����������
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);		

    while(!adc_flag_get(ADC_FLAG_EOC));	//�ȴ�ת������
    adc_flag_clear(ADC_FLAG_EOC);//���ת����ɱ�־λ

    //�������һ��ADC�������ת�����
    return (adc_regular_data_read() & 0xfff);
}


/*!
    \brief      ��ȡ ADC ����ƽ��ֵ
*/
uint16_t get_adc_Average(uint8_t channel)
{
    int i = 0;
    uint32_t adc_raw = 0;
    uint8_t degree = 4;  //��ȡ�Ĵ������� 2^4
    for(i = 0; i < (1 << degree); i++)
    {
        //��ȡADC����
        adc_raw += get_adc(channel);
    }
    //����ѭ��8�ζ�ȡ�����>>4λ���ǳ���16
    adc_raw =  (adc_raw >> degree);
    return adc_raw;
}

