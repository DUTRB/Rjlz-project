/**
	@author: rubo
	@date  :24-7-18
*/
#include "bsp_ds18b20.h"
#include "bsp_usart.h"
#include "stdio.h"
#include "systick.h"

#define delay_us delay_1us

/******************************************************************
 * �� �� �� �ƣ�bsp_ds18b20_GPIO_Init
 * �� �� ˵ ����MLX90614�����ų�ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ�1δ��⵽����   0��⵽����
******************************************************************/
int DS18B20_GPIO_Init(void)
{
    uint8_t ret = 255;
    /* ʹ��ʱ�� */
    //rcu_periph_clock_enable(RCU_DQ);
    /* ����DQΪ���ģʽ */
    gpio_mode_set(PORT_DQ,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_DQ);
    /* ����Ϊ������� 50MHZ */
    gpio_output_options_set(PORT_DQ,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_DQ);

    ret = DS18B20_Check();//��������Ƿ����
    return ret;
}



/******************************************************************
 * �� �� �� �ƣ�DS18B20_Read_Byte
 * �� �� ˵ ������DS18B20��ȡһ���ֽ�
 * �� �� �� �Σ���
 * �� �� �� �أ���ȡ�����ֽ�����
******************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i=0,dat=0;

    for (i=0; i<8; i++)
    {
        DQ_OUT();//����Ϊ����ģʽ
        DQ(0); //����
        delay_us(2);
        DQ(1); //�ͷ�����
        DQ_IN();//����Ϊ����ģʽ
			// ����ʱ����Ҫ��ǳ��ߣ�������
			// ������ʱ�˴���ʱΪ�ȴ��������
			// ����6us���ݲ��ȶ�������10us����ֱ�����0
        delay_us(5);
        dat>>=1;
        if( DQ_GET() )
        {
            dat=dat|0x80;
        }
        delay_us(60);
				DQ(1); //�ͷ�����
    }
    return dat;
}

/******************************************************************
 * �� �� �� �ƣ�DS18B20_Write_Byte
 * �� �� ˵ ����дһ���ֽڵ�DS18B20
 * �� �� �� �Σ�dat��Ҫд����ֽ�
 * �� �� �� �أ���
******************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t i;
    uint8_t testb;
    DQ_OUT();//�������ģʽ
    for (i=0; i<8; i++)
    {
        if ( (dat&0x01) ) //д1
        {
            DQ(0);
            delay_us(2);
            DQ(1);
            delay_us(60);
        }
        else //д0
        {
            DQ(0);//����60us
            delay_us(60);
            DQ(1);//�ͷ�����
            delay_us(2);
        }
        dat=dat>>1;//������һλ
    }
}



/******************************************************************
 * �� �� �� �ƣ�DS18B20_Check
 * �� �� ˵ �������DS18B20�Ƿ����
 * �� �� �� �Σ���
 * �� �� �� �أ�1:δ��⵽DS18B20�Ĵ���  0:����
******************************************************************/
uint8_t DS18B20_Check(void)
{
    uint8_t timeout=0;
    //��λDS18B20
    DQ_OUT(); //����Ϊ���ģʽ
    DQ(0); //����DQ
    delay_us(750); //����750us
    DQ(1); //����DQ
    delay_us(15);  //15us

    //����Ϊ����ģʽ
    DQ_IN();
    //�ȴ����ͣ�����˵����Ӧ��
    while ( DQ_GET() &&timeout<200)
    {
        timeout++;//��ʱ�ж�
        delay_us(1);
    };
    //�豸δӦ��
    if(timeout>=200)
        return 1;
    else
        timeout=0;

    //�ȴ�18B20�ͷ�����
    while ( !DQ_GET() &&timeout<240)
    {
        timeout++;//��ʱ�ж�
        delay_us(1);
    };
    //�ͷ�����ʧ��
    if(timeout>=240)
        return 1;

    return 0;
}

/******************************************************************
 * �� �� �� �ƣ�DS18B20_Start
 * �� �� ˵ ����DS18B20��ʼ�¶�ת��
 * �� �� �� �Σ���
 * �� �� �� �أ���
******************************************************************/
void DS18B20_Start(void)
{
    DS18B20_Check();            //��ѯ�Ƿ����豸Ӧ��
    DS18B20_Write_Byte(0xcc);   //�������������豸����Ѱַ
    DS18B20_Write_Byte(0x44);   //�����¶�ת��
}

/******************************************************************
 * �� �� �� �ƣ�DS18B20_GetTemperture
 * �� �� ˵ ������ds18b20�õ��¶�ֵ
 * �� �� �� �Σ���
 * �� �� �� �أ��¶�����
******************************************************************/
float DS18B20_GetTemperture(void)
{
    uint16_t tempe;
    uint8_t dataL=0,dataH=0;
    float value;

    DS18B20_Start();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);//�������������豸����Ѱַ
    DS18B20_Write_Byte(0xbe);// ��ȡ��������
    dataL=DS18B20_Read_Byte(); //LSB
    dataH=DS18B20_Read_Byte(); //MSB
    tempe=(dataH<<8)+dataL;//��������


    if(dataH&0X80)//��λΪ1��˵���Ǹ��¶�
    {
        tempe=(~tempe)+1;
        value=tempe*(-0.0625);
    }
    else
    {
        value=tempe*0.0625;
    }
    return value;
}
