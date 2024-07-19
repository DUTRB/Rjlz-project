/**
	@author: rubo
	@date  :24-7-18
	@brief :ʹ��ģ��IIC�������Ӵ����� SGP30 ����CO2��TVOC��Ũ�ȡ�
	        ���߷�ʽ ��PB0: SCL   PB1: SDA
*/
#include "sgp30.h"
#include "bsp_usart.h"
#include "stdio.h"
#include "systick.h"

sgp30_data sgp_data = {0};

/******************************************************************
 * �� �� �� �ƣ�SGP30_GPIO_Init
 * �� �� ˵ ����SGP30�����ų�ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       ע��ֻ�����ų�ʼ����������ʼ���� SGP30_Init
******************************************************************/
void SGP30_GPIO_Init(void)
{
    /* ʹ��ʱ�� */
    //rcu_periph_clock_enable(RCU_SCL);
    //rcu_periph_clock_enable(RCU_SDA);

    /* ����SCLΪ���ģʽ */
    gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
    /* ����Ϊ������� 50MHZ */
    gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);

    /* ����SDAΪ���ģʽ */
    gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
    /* ����Ϊ������� 50MHZ */
    gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);
}


/******************************************************************
 * �� �� �� �ƣ�IIC_Start
 * �� �� ˵ ����IIC��ʼʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
******************************************************************/
void IIC2_Start(void)
{
    SDA2_OUT();

    SDA2(1);
    delay_us(5);
    SCL2(1);
    delay_us(5);

    SDA2(0);
    delay_us(5);
    SCL2(0);
    delay_us(5);

}
/******************************************************************
 * �� �� �� �ƣ�IIC_Stop
 * �� �� ˵ ����IICֹͣ�ź�
 * �� �� �� �Σ���
 * �� �� �� �أ���
******************************************************************/
void IIC2_Stop(void)
{
    SDA2_OUT();
    SCL2(0);
    SDA2(0);

    SCL2(1);
    delay_us(5);
    SDA2(1);
    delay_us(5);

}

/******************************************************************
 * �� �� �� �ƣ�IIC_Send_Ack
 * �� �� ˵ ������������Ӧ����߷�Ӧ���ź�
 * �� �� �� �Σ�0����Ӧ��  1���ͷ�Ӧ��
 * �� �� �� �أ���
******************************************************************/
void IIC2_Send_Ack(unsigned char ack)
{
    SDA2_OUT();
    SCL2(0);
    SDA2(0);
    delay_us(5);
    if(!ack) SDA2(0);
    else     SDA2(1);
    SCL2(1);
    delay_us(5);
    SCL2(0);
    SDA2(1);
}


/******************************************************************
 * �� �� �� �ƣ�I2C_WaitAck
 * �� �� ˵ �����ȴ��ӻ�Ӧ��
 * �� �� �� �Σ���
 * �� �� �� �أ�0��Ӧ��  1��ʱ��Ӧ��
******************************************************************/
unsigned char IIC2_WaitAck(void)
{

    char ack = 0;
    unsigned char ack_flag = 10;
    SCL2(0);
    SDA2(1);
    SDA2_IN();
    delay_us(5);
    SCL2(1);
    delay_us(5);

    while( (SDA2_GET()==1) && ( ack_flag ) )
    {
        ack_flag--;
        delay_us(5);
    }

    if( ack_flag <= 0 )
    {
        IIC2_Stop();
        return 1;
    }
    else
    {
        SCL2(0);
        SDA2_OUT();
    }
    return ack;
}

/******************************************************************
 * �� �� �� �ƣ�Send_Byte
 * �� �� ˵ ����д��һ���ֽ�
 * �� �� �� �Σ�datҪд�˵�����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void IIC2_Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA2_OUT();
    SCL2(0);//����ʱ�ӿ�ʼ���ݴ���

    for( i = 0; i < 8; i++ )
    {
        SDA2( (dat & 0x80) >> 7 );
        delay_us(1);
        SCL2(1);
        delay_us(5);
        SCL2(0);
        delay_us(5);
        dat<<=1;
    }
}

/******************************************************************
 * �� �� �� �ƣ�Read_Byte
 * �� �� ˵ ����IIC��ʱ��
 * �� �� �� �Σ���
 * �� �� �� �أ�����������
******************************************************************/
unsigned char IIC2_Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA2_IN();//SDA����Ϊ����
    for(i=0; i<8; i++ )
    {
        SCL2(0);
        delay_us(5);
        SCL2(1);
        delay_us(5);
        receive<<=1;
        if( SDA2_GET() )
        {
            receive|=1;
        }
        delay_us(5);
    }
    SCL2(0);
    return receive;
}

/******************************************************************
 * �� �� �� �ƣ�SGP30_Write
 * �� �� ˵ ����SGP30д����
 * �� �� �� �Σ�regaddr_H�����8λ   regaddr_L�����8λ
 * �� �� �� �أ���
******************************************************************/
void SGP30_Write_cmd(uint8_t regaddr_H, uint8_t regaddr_L)
{
    IIC2_Start();
    IIC2_Send_Byte(0XB0); //����������ַ+дָ��
    IIC2_WaitAck();
    IIC2_Send_Byte(regaddr_H);           //���Ϳ��Ƶ�ַ
    IIC2_WaitAck();
    IIC2_Send_Byte(regaddr_L);    //��������
    IIC2_WaitAck();
    IIC2_Stop();
    delay_1ms(100);
}

/******************************************************************
 * �� �� �� �ƣ�SGP30_Read
 * �� �� ˵ �������ݶ�ȡ
 * �� �� �� �Σ���
 * �� �� �� �أ���ȡ����co2 TVOC
 * ��       ע��SGP30��ȡ�����ݸ�ʽΪ��2λCO2����+1λCO2��CRCУ��+2λTVOC����+1λTVOC��CRCУ�顣
                ģ���ϵ���Ҫ15s���ҳ�ʼ�����ڳ�ʼ���׶ζ�ȡ��CO2Ũ��Ϊ400ppm��TVOCΪ0ppd�Һ㶨���䡣
                ����ϵ��һֱ����ֱ��TVOC��Ϊ0����CO2��Ϊ400��SGP30ģ��ų�ʼ����ɡ�
                ��ʼ����ɺ�տ�ʼ�������ݻᲨ���Ƚϴ�������������һ��ʱ�����������ȶ���
                �����ഫ�����Ƚ������ܻ���Ӱ�죬�������ݳ��ֲ����������ģ���������˲����������˲���
******************************************************************/
uint32_t SGP30_Read(void)
{
    uint32_t dat;
    uint8_t crc;

    IIC2_Start();
    IIC2_Send_Byte(0XB1); //����������ַ+��ָ��
    IIC2_WaitAck();

    dat = IIC2_Read_Byte();//CO2���ݸ�8λ
    IIC2_Send_Ack(0);
    dat <<= 8;

    dat += IIC2_Read_Byte();//CO2���ݵ�8λ
    IIC2_Send_Ack(0);

    crc = IIC2_Read_Byte(); //CO2��CRCУ��
    IIC2_Send_Ack(0);
    crc = crc;


    dat <<= 8;
    dat += IIC2_Read_Byte();//TVOC���ݸ�8λ
    IIC2_Send_Ack(0);

    dat <<= 8;
    dat += IIC2_Read_Byte();//TVOC���ݵ�8λ
    IIC2_Send_Ack(1);

    IIC2_Stop();
    return(dat);
}

/******************************************************************
 * �� �� �� �ƣ�SGP30_Init
 * �� �� ˵ ����SGP30��ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
******************************************************************/
void SGP30_Init(void)
{
    SGP30_GPIO_Init();
    SGP30_Write_cmd(0x20, 0x03);
}

/******************************************************************
 * �� �� �� �ƣ�SGP30_Get_Value
 * �� �� ˵ ����SGP30������ֵ������
 * �� �� �� �Σ���
 * �� �� �� �أ���
******************************************************************/
void SGP30_Get_Value(void)
{
		uint32_t sgp30_dat;          	//����SGP30��ȡ��������
		
    SGP30_Write_cmd(0x20,0x08);
		sgp30_dat = SGP30_Read();                  //��ȡSGP30��ֵ
		sgp_data.CO2_val = (sgp30_dat & 0xffff0000) >> 16;  //��ȡCO2��ֵ
		sgp_data.TVOC_val = sgp30_dat & 0x0000ffff;         //��ȡTVOC��ֵ
		return ;
}
