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



uint8_t	temp8;   // ��ʱ����
uint16_t temp16; // ��ʱ����

void calibration_init() {
    temp8 = AT24CXX_ReadOneByte(255);	//������ǵ�һ�γ�ʼ��������ʹ�ù����������Ƿ�������ֱ�Ӷ����ɣ�����Ҫ��д�롣������ÿ�μ�鶼д�롣
    if(temp8 == 0X55)
    {
        //��ȡ
        OBS_Y.data_16 = AT24CXX_ReadLenByte(0x09,2);
        OBS_BIAS = AT24CXX_ReadLenByte(0x0b,2);
        OBS_FLAG = AT24CXX_ReadOneByte(0x0d);
        if(OBS_FLAG & 0x01) AIR_OBS_F = 1;
        else AIR_OBS_F = 0;
        if(OBS_FLAG & 0x02) N2_OBS_F = 1;
        else N2_OBS_F = 0;
        if(OBS_FLAG & 0x04) OBS_OVER_F = 1;
        else OBS_OVER_F = 0;

        Smoke_N2.data_16 = AT24CXX_ReadLenByte(0x01,2);//����ǰ���������OBSֵ
        OBS_N2.data_16 = AT24CXX_ReadLenByte(0x03,2);

        Smoke_AIR.data_16 = AT24CXX_ReadLenByte(0x05,2);//����ǰ���������OBSֵ
        OBS_AIR.data_16 = AT24CXX_ReadLenByte(0x07,2);
    }
    else	//����ǵ�һ�γ�ʼ��
    {
        AT24CXX_WriteOneByte(255, 0X55);
        temp8 = AT24CXX_ReadOneByte(255);
        if(temp8 != 0X55) AT24CXX_WriteOneByte(255, 0X55);

        AIR_OBS_F  = 0 ;         //�������OBSƫ���Ѿ��궨��ǣ�OBS_FLAG.0,�������
        N2_OBS_F   = 0 ;         //���㵪��OBSƫ���Ѿ��궨��ǣ�OBS_FLAG.1,�������
        OBS_OVER_F = 0 ;         //����OBS�궨������ϱ�ǣ�OBS_FLAG.2,������ϱ��

        OBS_FLAG   = 0 ;       	 //����OBS�궨��������
        temp8 = AT24CXX_ReadOneByte(0x0d);
        if(temp8 != OBS_FLAG)	AT24CXX_WriteOneByte(0x0d,OBS_FLAG);//д��ǰOBS�궨���

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
    \brief      GPIO ��ʼ��
*/
void gpio_init(void)
{
    //rcu_periph_clock_enable(RCU_GPIOA);
		// Դ������ΪPA0����Ӧԭ��·ͼ�еı궨��1 ���°���Ϊ PB7
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7);
    gpio_bit_set(GPIOB, GPIO_PIN_7);

    /*RS485-C PB4 ��1Ϊ���ͣ���0Ϊ����*/  
		// Դ������Ϊ PA5
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_4);

    /******************* ˫ɫ Laser ������˸���� **********************/
	
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

