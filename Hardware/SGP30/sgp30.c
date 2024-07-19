/**
	@author: rubo
	@date  :24-7-18
	@brief :使用模拟IIC总线连接传感器 SGP30 测量CO2和TVOC的浓度。
	        接线方式 ：PB0: SCL   PB1: SDA
*/
#include "sgp30.h"
#include "bsp_usart.h"
#include "stdio.h"
#include "systick.h"

sgp30_data sgp_data = {0};

/******************************************************************
 * 函 数 名 称：SGP30_GPIO_Init
 * 函 数 说 明：SGP30的引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 备       注：只是引脚初始化，真正初始化： SGP30_Init
******************************************************************/
void SGP30_GPIO_Init(void)
{
    /* 使能时钟 */
    //rcu_periph_clock_enable(RCU_SCL);
    //rcu_periph_clock_enable(RCU_SDA);

    /* 配置SCL为输出模式 */
    gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
    /* 配置为推挽输出 50MHZ */
    gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);

    /* 配置SDA为输出模式 */
    gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
    /* 配置为推挽输出 50MHZ */
    gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);
}


/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
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
 * 函 数 名 称：IIC_Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
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
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答或者非应答信号
 * 函 数 形 参：0发送应答  1发送非应答
 * 函 数 返 回：无
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
 * 函 数 名 称：I2C_WaitAck
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：0有应答  1超时无应答
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
 * 函 数 名 称：Send_Byte
 * 函 数 说 明：写入一个字节
 * 函 数 形 参：dat要写人的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC2_Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA2_OUT();
    SCL2(0);//拉低时钟开始数据传输

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
 * 函 数 名 称：Read_Byte
 * 函 数 说 明：IIC读时序
 * 函 数 形 参：无
 * 函 数 返 回：读到的数据
******************************************************************/
unsigned char IIC2_Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA2_IN();//SDA设置为输入
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
 * 函 数 名 称：SGP30_Write
 * 函 数 说 明：SGP30写命令
 * 函 数 形 参：regaddr_H命令高8位   regaddr_L命令低8位
 * 函 数 返 回：无
******************************************************************/
void SGP30_Write_cmd(uint8_t regaddr_H, uint8_t regaddr_L)
{
    IIC2_Start();
    IIC2_Send_Byte(0XB0); //发送器件地址+写指令
    IIC2_WaitAck();
    IIC2_Send_Byte(regaddr_H);           //发送控制地址
    IIC2_WaitAck();
    IIC2_Send_Byte(regaddr_L);    //发送数据
    IIC2_WaitAck();
    IIC2_Stop();
    delay_1ms(100);
}

/******************************************************************
 * 函 数 名 称：SGP30_Read
 * 函 数 说 明：数据读取
 * 函 数 形 参：空
 * 函 数 返 回：读取到的co2 TVOC
 * 备       注：SGP30获取的数据格式为：2位CO2数据+1位CO2的CRC校验+2位TVOC数据+1位TVOC的CRC校验。
                模块上电需要15s左右初始化，在初始化阶段读取的CO2浓度为400ppm，TVOC为0ppd且恒定不变。
                因此上电后一直读，直到TVOC不为0并且CO2不为400，SGP30模块才初始化完成。
                初始化完成后刚开始读出数据会波动比较大，属于正常现象，一段时间后会逐渐趋于稳定。
                气体类传感器比较容易受环境影响，测量数据出现波动是正常的，可以添加滤波函数进行滤波。
******************************************************************/
uint32_t SGP30_Read(void)
{
    uint32_t dat;
    uint8_t crc;

    IIC2_Start();
    IIC2_Send_Byte(0XB1); //发送器件地址+读指令
    IIC2_WaitAck();

    dat = IIC2_Read_Byte();//CO2数据高8位
    IIC2_Send_Ack(0);
    dat <<= 8;

    dat += IIC2_Read_Byte();//CO2数据低8位
    IIC2_Send_Ack(0);

    crc = IIC2_Read_Byte(); //CO2的CRC校验
    IIC2_Send_Ack(0);
    crc = crc;


    dat <<= 8;
    dat += IIC2_Read_Byte();//TVOC数据高8位
    IIC2_Send_Ack(0);

    dat <<= 8;
    dat += IIC2_Read_Byte();//TVOC数据低8位
    IIC2_Send_Ack(1);

    IIC2_Stop();
    return(dat);
}

/******************************************************************
 * 函 数 名 称：SGP30_Init
 * 函 数 说 明：SGP30初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
******************************************************************/
void SGP30_Init(void)
{
    SGP30_GPIO_Init();
    SGP30_Write_cmd(0x20, 0x03);
}

/******************************************************************
 * 函 数 名 称：SGP30_Get_Value
 * 函 数 说 明：SGP30计算数值并返回
 * 函 数 形 参：无
 * 函 数 返 回：无
******************************************************************/
void SGP30_Get_Value(void)
{
		uint32_t sgp30_dat;          	//定义SGP30读取到的数据
		
    SGP30_Write_cmd(0x20,0x08);
		sgp30_dat = SGP30_Read();                  //读取SGP30的值
		sgp_data.CO2_val = (sgp30_dat & 0xffff0000) >> 16;  //获取CO2的值
		sgp_data.TVOC_val = sgp30_dat & 0x0000ffff;         //获取TVOC的值
		return ;
}
