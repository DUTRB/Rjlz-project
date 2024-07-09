/**
  ******************************************************************************
  * @file    		iic.c
  * @author 		GYP
  * @version 		V1.0
  * @date   		2021.05.01
  * @brief			模拟IIC通信
  * @attention		SCL：PB6，SDA：PB7。因为是用IO口模拟IIC通信，所以任意一个IO口都可以。
  ******************************************************************************
 **/

#include "iic.h"


void Delay_us(int us)
{
    delay_1us(us);
}


//写SCL数据
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
//写SDA数据
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
//读SDA数据
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
  * @brief			IIC的IO口初始化函数
  * @param			None
  * @retval  		None
  * @attention		SCL：PB6，SDA：PB7，均初始化为推挽输出，均默认输出高电平使总线为空闲状态
  ******************************************************************************
 **/

void IIC_Init(void)
{

    gpio_mode_set(SDA_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SDA_PIN);//推挽输出
    gpio_output_options_set(SDA_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  SDA_PIN);



    gpio_mode_set(SCL_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SCL_PIN);
    //gpio_output_options_set(SCL_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  SCL_PIN);//推挽
    gpio_output_options_set(SCL_GPIO, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, SCL_PIN);//开漏


    gpio_bit_set(SCL_GPIO, SCL_PIN);//SCL输出高电平，将总线置为空闲状态
    gpio_bit_set(SDA_GPIO, SDA_PIN);//SDA输出高电平，将总线置为空闲状态

}


/**
  ******************************************************************************
  * @brief			产生IIC起始信号
  * @param			None
  * @retval  		None
  * @attention		起始信号：SCL=1时，SDA由高到低变化。

					数据有效性：
					IIC总线进行数据传送时，时钟信号为高电平期间，数据线上的数据必须保持稳定，
					只有在时钟线上的信号为低电平期间，数据线上的高电平或低电平状态才允许变化。
					即：数据在SCL的上升沿到来之前就需准备好。并在在下降沿到来之前必须稳定。
					SCL = 1，SDA稳定不变。
					SCL = 0，SDA可以变化。

					延时的作用：让SCL或SDA信号稳定。对于起始和停止信号，至少稳定4us。对于其它信号1或2us即可。
  ******************************************************************************
 **/

void IIC_Start(void)
{
    SDA_OUT();		//SDA配置为输出模式
    //IIC_SDA = 1;	//SCL、SDA置高，使总线为空闲状态
    //IIC_SCL = 1;
    iic_write_sda(1);
    iic_write_scl(1);
    Delay_us(4);	//延时4us，确定SCL和SDA信号都稳定为1了，普中说要>4.7us
    //IIC_SDA = 0;	//起始信号：SCL=1时，SDA由高到低变化
    iic_write_sda(0);
    Delay_us(4);	//延时4us，让SDA信号稳定一下，普中说要>4us
    //IIC_SCL = 0;	//SCL=0，钳住I2C总线，以允许SDA变化，准备开始数据传送（发送或接收数据）
    iic_write_scl(0);
}


/**
  ******************************************************************************
  * @brief			产生IIC停止信号
  * @param			None
  * @retval  		None
  * @attention		停止信号：SCL=1时，SDA由低到高变化。
	******************************************************************************
 **/

void IIC_Stop(void)
{
    SDA_OUT();		//SDA配置为输出模式
    //IIC_SCL = 0;	//SCL=0，以允许SDA变化
    //IIC_SDA = 0;
    iic_write_scl(0);
    iic_write_sda(0);


    //Delay_us(4);	//延时4us，确保SCL=0、SDA=0稳定
    /* 正点原子和普中这里的程序顺序不同：
       正点原子：Delay_us(4);IIC_SCL = 1;
       普中：IIC_SCL = 1;Delay_us(4);
       我觉得普中的比较正确，SCL=1要稳定下来，再让SDA跳变。为了保险，可以在上面写个延时4us，确保SCL=0、SDA=0也稳定
    */

    //IIC_SCL = 1;
    iic_write_scl(1);
    Delay_us(4);	//延时4us，稳定SCL=1、SDA=0，普中说要>4us
    //IIC_SDA = 1;	//起始信号：SCL=1时，SDA由低到高变化
    iic_write_sda(1);
    Delay_us(4);	//延时4us，让SDA信号稳定一下，普中说要>4.7us
}



/**
  ******************************************************************************
  * @brief			等待应答信号到来
  * @param			None
  * @retval  		0：接收应答成功，1：接收应答失败
  * @attention
	******************************************************************************
 **/

u8 IIC_Wait_ACK(void)
{
    u8 ucErrTime = 0;
    SDA_IN();		//SDA配置为输入模式
    //IIC_SDA = 1;	//SDA=1让总线空闲，来让主机读取从机的应答信号
    iic_write_sda(1);
    Delay_us(1);	//延时1us，让SDA稳定一下
    //IIC_SCL = 1;	//SCL=1，让SDA稳定不变，这样才能读取SDA的数据
    iic_write_scl(1);
    Delay_us(1);
    while(iic_read_sda())		//SDA数据线接收到的始终为高就一直等待
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();	//一段时间没有ACK，主机产生停止信号，返回1
            return 1;
        }
    }
    //IIC_SCL = 0;  //有ACK，SCL=0，以允许SDA变化，准备开始新一轮的数据传送
    iic_write_scl(0);
    return 0;
}



/**
  ******************************************************************************
  * @brief			产生ACK应答
  * @param			None
  * @retval  		None
  * @attention		SDA = 0表示ACK
	******************************************************************************
 **/

void IIC_ACK(void)
{
    //IIC_SCL = 0;	//SCL=0，以允许SDA变化，来产生ACK应答
    iic_write_scl(0);
    SDA_OUT();		//SDA配置为输出模式
    //IIC_SDA = 0;	//SDA=0，产生ACK应答
    iic_write_sda(0);
    Delay_us(2);	//延时2us，让通信稳定一下
    //IIC_SCL = 1;	//SCL=1，让SDA稳定不变，来让从机接收ACK数据
    iic_write_scl(1);
    Delay_us(2);	//普中说要>4us
    //IIC_SCL = 0;	//SCL=0，以允许SDA变化，准备开始下一轮的数据传送
    iic_write_scl(0);
}



/**
  ******************************************************************************
  * @brief			不产生ACK应答
  * @param			None
  * @retval  		None
  * @attention		SDA = 1表示NACK
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
    Delay_us(2);	//普中说要>4us
    //IIC_SCL = 0;
    iic_write_scl(0);
}



/**
  ******************************************************************************
  * @brief			IIC发送一个字节数据
  * @param			None
  * @retval  		None
  * @attention
	******************************************************************************
 **/

void IIC_Send_Byte(u8 txd)
{
    u8 i = 0;
    SDA_OUT();		//SDA配置为输出模式
    //IIC_SCL = 0;	//SCL=0，以允许SDA变化，开始数据传送
    iic_write_scl(0);
    for(i=0; i<8; i++)
    {
        //IIC_SDA = (txd & 0x80) >> 7;  //获取数据txd的最高位（第7位），右移7位，移到最低位。IIC_SD为0表示发送0，为1表示发送1（0x01和1一样只是进制不同）。没有改变txd本身的值。
        if((txd&0x80)>>7)
        {
            iic_write_sda(1);
        }
        else
        {
            iic_write_sda(0);
        }

        txd <<= 1;		//ted = txd<<1，txd左移一位，下次传输txd的第6位
        Delay_us(2);	//延时2us，让SDA稳定一下，数据在SCL上升沿到来之前就需要准备好
        //IIC_SCL = 1;	//SCL=1，让SDA稳定不变，来让从机接收数据
        iic_write_scl(1);
        Delay_us(2); 	//数据在SCL下降沿到来之前必须稳定
        //IIC_SCL = 0;	//SCL=0，以允许SDA变化，准备开始下一轮的数据传送，后面for循环紧跟着，所以延时稳定一下
        iic_write_scl(0);
        Delay_us(2);
    }
}



/**
  ******************************************************************************
  * @brief			IIC接收一个字节数据
  * @param			ack=1：发送ACK，ack=0：发送NACK
  * @retval  		None
  * @attention
	******************************************************************************
 **/

u8 IIC_Read_Byte(u8 ack)
{
    u8 i, receive = 0;
    SDA_IN();	//SDA配置为输入模式
    for(i=0; i<8; i++ )
    {
        //IIC_SCL = 0;	//SCL=0，以允许SDA变化，开始数据传送
        iic_write_scl(0);
        Delay_us(2);
        //IIC_SCL = 1;	//SCL=1，让SDA稳定不变，来让主机接收数据
        iic_write_scl(1);
        receive <<= 1;	//接收数据缓冲区先移位1次，如果先判断后移位，移到最后一次就会移出去了
        if(iic_read_sda()) receive++;	//根据接收到的SDA，改变receive的第0位，receive不++表示收到0，++表示收到1
        Delay_us(1);	//延时稳定一下
    }
    if (!ack)
        IIC_NACK();	//发送NACK
    else
        IIC_ACK();	//发送ACK
    return receive;
}

//写数据
void WriteData(u8 DevID, u8 Addr, u8 Dat)
{
    IIC_Start();
    IIC_Send_Byte(0X30);	//发送设备地址和写信号
    IIC_Wait_ACK();
    IIC_Send_Byte(Addr);  //发送地址
    IIC_Wait_ACK();
    IIC_Send_Byte(Dat);   //发送字节
    IIC_Wait_ACK();
    IIC_Stop();
    Delay_us(10);
}
//读数据
void ReadData(u8 DevID, u8 Addr, u8 *Pbuf, u8 Num)
{
    u8 i;
    IIC_Start();
    IIC_Send_Byte(0X30);	//发送设备地址和写信号
    IIC_Wait_ACK();
    IIC_Send_Byte(Addr);	//发送读地址
    IIC_Wait_ACK();
    IIC_Start();
    IIC_Send_Byte(0X31);	//发送设备地址和读信号
    IIC_Wait_ACK();
    for(i = 0; i < (Num - 1); i++)
    {
        Pbuf[i] = IIC_Read_Byte(1);
    }
    Pbuf[i] = IIC_Read_Byte(0);
    IIC_Stop();
    Delay_us(10);
}

