/**
  ******************************************************************************
  * @file    		iic.c
  * @author 		GYP
  * @version 		V1.0
  * @date   		2021.05.01
  * @brief			ģ��IICͨ��
  * @attention		SCL��PB6��SDA��PB7����Ϊ����IO��ģ��IICͨ�ţ���������һ��IO�ڶ����ԡ�
  ******************************************************************************
 **/

#include "iic.h"


void Delay_us(int us)
{
    delay_1us(us);
}


//дSCL����
void iic_write_scl(u8 bit)
{
    if(bit == 0)
    {
        gpio_bit_reset(SCL_GPIO, SCL_PIN);
    }
    else
    {
        gpio_bit_set(SCL_GPIO, SCL_PIN);
    }
}
//дSDA����
void iic_write_sda(u8 bit)
{
    if(bit == 0)
    {
        gpio_bit_reset(SDA_GPIO, SDA_PIN);
    }
    else
    {
        gpio_bit_set(SDA_GPIO, SDA_PIN);
    }
}
//��SDA����
u8 iic_read_sda(void)
{
    if(gpio_input_bit_get(SDA_GPIO, SDA_PIN) == 0)
    {
        return 0x00;
    }
    else
    {
        return 0x01;
    }
}


/**
  ******************************************************************************
  * @brief			IIC��IO�ڳ�ʼ������
  * @param			None
  * @retval  		None
  * @attention		SCL��PB6��SDA��PB7������ʼ��Ϊ�����������Ĭ������ߵ�ƽʹ����Ϊ����״̬
  ******************************************************************************
 **/

void IIC_Init(void)
{

    gpio_mode_set(SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SDA_PIN);//�������
    gpio_output_options_set(SDA_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  SDA_PIN);



    gpio_mode_set(SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SCL_PIN);
    //gpio_output_options_set(SCL_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  SCL_PIN);//����
    gpio_output_options_set(SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, SCL_PIN);//��©


    gpio_bit_set(SCL_GPIO, SCL_PIN);//SCL����ߵ�ƽ����������Ϊ����״̬
    gpio_bit_set(SDA_GPIO, SDA_PIN);//SDA����ߵ�ƽ����������Ϊ����״̬

}


/**
  ******************************************************************************
  * @brief			����IIC��ʼ�ź�
  * @param			None
  * @retval  		None
  * @attention		��ʼ�źţ�SCL=1ʱ��SDA�ɸߵ��ͱ仯��

					������Ч�ԣ�
					IIC���߽������ݴ���ʱ��ʱ���ź�Ϊ�ߵ�ƽ�ڼ䣬�������ϵ����ݱ��뱣���ȶ���
					ֻ����ʱ�����ϵ��ź�Ϊ�͵�ƽ�ڼ䣬�������ϵĸߵ�ƽ��͵�ƽ״̬������仯��
					����������SCL�������ص���֮ǰ����׼���á��������½��ص���֮ǰ�����ȶ���
					SCL = 1��SDA�ȶ����䡣
					SCL = 0��SDA���Ա仯��

					��ʱ�����ã���SCL��SDA�ź��ȶ���������ʼ��ֹͣ�źţ������ȶ�4us�����������ź�1��2us���ɡ�
  ******************************************************************************
 **/

void IIC_Start(void)
{
    SDA_OUT();		//SDA����Ϊ���ģʽ
    //IIC_SDA = 1;	//SCL��SDA�øߣ�ʹ����Ϊ����״̬
    //IIC_SCL = 1;
    iic_write_sda(1);
    iic_write_scl(1);
    Delay_us(4);	//��ʱ4us��ȷ��SCL��SDA�źŶ��ȶ�Ϊ1�ˣ�����˵Ҫ>4.7us
    //IIC_SDA = 0;	//��ʼ�źţ�SCL=1ʱ��SDA�ɸߵ��ͱ仯
    iic_write_sda(0);
    Delay_us(4);	//��ʱ4us����SDA�ź��ȶ�һ�£�����˵Ҫ>4us
    //IIC_SCL = 0;	//SCL=0��ǯסI2C���ߣ�������SDA�仯��׼����ʼ���ݴ��ͣ����ͻ�������ݣ�
    iic_write_scl(0);
}


/**
  ******************************************************************************
  * @brief			����IICֹͣ�ź�
  * @param			None
  * @retval  		None
  * @attention		ֹͣ�źţ�SCL=1ʱ��SDA�ɵ͵��߱仯��
	******************************************************************************
 **/

void IIC_Stop(void)
{
    SDA_OUT();		//SDA����Ϊ���ģʽ
    //IIC_SCL = 0;	//SCL=0��������SDA�仯
    //IIC_SDA = 0;
    iic_write_scl(0);
    iic_write_sda(0);


    //Delay_us(4);	//��ʱ4us��ȷ��SCL=0��SDA=0�ȶ�
    /* ����ԭ�Ӻ���������ĳ���˳��ͬ��
       ����ԭ�ӣ�Delay_us(4);IIC_SCL = 1;
       ���У�IIC_SCL = 1;Delay_us(4);
       �Ҿ������еıȽ���ȷ��SCL=1Ҫ�ȶ�����������SDA���䡣Ϊ�˱��գ�����������д����ʱ4us��ȷ��SCL=0��SDA=0Ҳ�ȶ�
    */

    //IIC_SCL = 1;
    iic_write_scl(1);
    Delay_us(4);	//��ʱ4us���ȶ�SCL=1��SDA=0������˵Ҫ>4us
    //IIC_SDA = 1;	//��ʼ�źţ�SCL=1ʱ��SDA�ɵ͵��߱仯
    iic_write_sda(1);
    Delay_us(4);	//��ʱ4us����SDA�ź��ȶ�һ�£�����˵Ҫ>4.7us
}



/**
  ******************************************************************************
  * @brief			�ȴ�Ӧ���źŵ���
  * @param			None
  * @retval  		0������Ӧ��ɹ���1������Ӧ��ʧ��
  * @attention
	******************************************************************************
 **/

u8 IIC_Wait_ACK(void)
{
    u8 ucErrTime = 0;
    SDA_IN();		//SDA����Ϊ����ģʽ
    //IIC_SDA = 1;	//SDA=1�����߿��У�����������ȡ�ӻ���Ӧ���ź�
    iic_write_sda(1);
    Delay_us(1);	//��ʱ1us����SDA�ȶ�һ��
    //IIC_SCL = 1;	//SCL=1����SDA�ȶ����䣬�������ܶ�ȡSDA������
    iic_write_scl(1);
    Delay_us(1);
    while(iic_read_sda())		//SDA�����߽��յ���ʼ��Ϊ�߾�һֱ�ȴ�
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();	//һ��ʱ��û��ACK����������ֹͣ�źţ�����1
            return 1;
        }
    }
    //IIC_SCL = 0;  //��ACK��SCL=0��������SDA�仯��׼����ʼ��һ�ֵ����ݴ���
    iic_write_scl(0);
    return 0;
}



/**
  ******************************************************************************
  * @brief			����ACKӦ��
  * @param			None
  * @retval  		None
  * @attention		SDA = 0��ʾACK
	******************************************************************************
 **/

void IIC_ACK(void)
{
    //IIC_SCL = 0;	//SCL=0��������SDA�仯��������ACKӦ��
    iic_write_scl(0);
    SDA_OUT();		//SDA����Ϊ���ģʽ
    //IIC_SDA = 0;	//SDA=0������ACKӦ��
    iic_write_sda(0);
    Delay_us(2);	//��ʱ2us����ͨ���ȶ�һ��
    //IIC_SCL = 1;	//SCL=1����SDA�ȶ����䣬���ôӻ�����ACK����
    iic_write_scl(1);
    Delay_us(2);	//����˵Ҫ>4us
    //IIC_SCL = 0;	//SCL=0��������SDA�仯��׼����ʼ��һ�ֵ����ݴ���
    iic_write_scl(0);
}



/**
  ******************************************************************************
  * @brief			������ACKӦ��
  * @param			None
  * @retval  		None
  * @attention		SDA = 1��ʾNACK
	******************************************************************************
 **/

void IIC_NACK(void)
{
    //IIC_SCL = 0;
    iic_write_scl(0);
    SDA_OUT();
    //IIC_SDA = 1;
    iic_write_sda(1);
    Delay_us(2);
    //IIC_SCL = 1;
    iic_write_scl(1);
    Delay_us(2);	//����˵Ҫ>4us
    //IIC_SCL = 0;
    iic_write_scl(0);
}



/**
  ******************************************************************************
  * @brief			IIC����һ���ֽ�����
  * @param			None
  * @retval  		None
  * @attention
	******************************************************************************
 **/

void IIC_Send_Byte(u8 txd)
{
    u8 i = 0;
    SDA_OUT();		//SDA����Ϊ���ģʽ
    //IIC_SCL = 0;	//SCL=0��������SDA�仯����ʼ���ݴ���
    iic_write_scl(0);
    for(i=0; i<8; i++)
    {
        //IIC_SDA = (txd & 0x80) >> 7;  //��ȡ����txd�����λ����7λ��������7λ���Ƶ����λ��IIC_SDΪ0��ʾ����0��Ϊ1��ʾ����1��0x01��1һ��ֻ�ǽ��Ʋ�ͬ����û�иı�txd�����ֵ��
        if((txd&0x80)>>7)
        {
            iic_write_sda(1);
        }
        else
        {
            iic_write_sda(0);
        }

        txd <<= 1;		//ted = txd<<1��txd����һλ���´δ���txd�ĵ�6λ
        Delay_us(2);	//��ʱ2us����SDA�ȶ�һ�£�������SCL�����ص���֮ǰ����Ҫ׼����
        //IIC_SCL = 1;	//SCL=1����SDA�ȶ����䣬���ôӻ���������
        iic_write_scl(1);
        Delay_us(2); 	//������SCL�½��ص���֮ǰ�����ȶ�
        //IIC_SCL = 0;	//SCL=0��������SDA�仯��׼����ʼ��һ�ֵ����ݴ��ͣ�����forѭ�������ţ�������ʱ�ȶ�һ��
        iic_write_scl(0);
        Delay_us(2);
    }
}



/**
  ******************************************************************************
  * @brief			IIC����һ���ֽ�����
  * @param			ack=1������ACK��ack=0������NACK
  * @retval  		None
  * @attention
	******************************************************************************
 **/

u8 IIC_Read_Byte(u8 ack)
{
    u8 i, receive = 0;
    SDA_IN();	//SDA����Ϊ����ģʽ
    for(i=0; i<8; i++ )
    {
        //IIC_SCL = 0;	//SCL=0��������SDA�仯����ʼ���ݴ���
        iic_write_scl(0);
        Delay_us(2);
        //IIC_SCL = 1;	//SCL=1����SDA�ȶ����䣬����������������
        iic_write_scl(1);
        receive <<= 1;	//�������ݻ���������λ1�Σ�������жϺ���λ���Ƶ����һ�ξͻ��Ƴ�ȥ��
        if(iic_read_sda()) receive++;	//���ݽ��յ���SDA���ı�receive�ĵ�0λ��receive��++��ʾ�յ�0��++��ʾ�յ�1
        Delay_us(1);	//��ʱ�ȶ�һ��
    }
    if (!ack)
        IIC_NACK();	//����NACK
    else
        IIC_ACK();	//����ACK
    return receive;
}

//д����
void WriteData(u8 DevID, u8 Addr, u8 Dat)
{
    IIC_Start();
    IIC_Send_Byte(0X30);	//�����豸��ַ��д�ź�
    IIC_Wait_ACK();
    IIC_Send_Byte(Addr);  //���͵�ַ
    IIC_Wait_ACK();
    IIC_Send_Byte(Dat);   //�����ֽ�
    IIC_Wait_ACK();
    IIC_Stop();
    Delay_us(10);
}
//������
void ReadData(u8 DevID, u8 Addr, u8 *Pbuf, u8 Num)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte(0X30);	//�����豸��ַ��д�ź�
    IIC_Wait_ACK();
    IIC_Send_Byte(Addr);	//���Ͷ���ַ
    IIC_Wait_ACK();
    IIC_Start();
    IIC_Send_Byte(0X31);	//�����豸��ַ�Ͷ��ź�
    IIC_Wait_ACK();
    for(i = 0; i < (Num - 1); i++)
    {
        Pbuf[i] = IIC_Read_Byte(1);
    }
    Pbuf[i] = IIC_Read_Byte(0);
    IIC_Stop();
    Delay_us(10);
}

