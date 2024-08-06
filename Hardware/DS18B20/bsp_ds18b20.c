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
 * 函 数 名 称：bsp_ds18b20_GPIO_Init
 * 函 数 说 明：MLX90614的引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：1未检测到器件   0检测到器件
******************************************************************/
int DS18B20_GPIO_Init(void)
{
    uint8_t ret = 255;
    /* 使能时钟 */
    //rcu_periph_clock_enable(RCU_DQ);
    /* 配置DQ为输出模式 */
    gpio_mode_set(PORT_DQ,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_DQ);
    /* 配置为推挽输出 50MHZ */
    gpio_output_options_set(PORT_DQ,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_DQ);

    ret = DS18B20_Check();//检测器件是否存在
    return ret;
}



/******************************************************************
 * 函 数 名 称：DS18B20_Read_Byte
 * 函 数 说 明：从DS18B20读取一个字节
 * 函 数 形 参：无
 * 函 数 返 回：读取到的字节数据
******************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i=0,dat=0;

    for (i=0; i<8; i++)
    {
        DQ_OUT();//设置为输入模式
        DQ(0); //拉低
        delay_us(2);
        DQ(1); //释放总线
        DQ_IN();//设置为输入模式
			// 对延时精度要求非常高！！！！
			// 读数据时此处延时为等待数据输出
			// 大于6us数据不稳定，大于10us数据直接输出0
        delay_us(5);
        dat>>=1;
        if( DQ_GET() )
        {
            dat=dat|0x80;
        }
        delay_us(60);
				DQ(1); //释放总线
    }
    return dat;
}

/******************************************************************
 * 函 数 名 称：DS18B20_Write_Byte
 * 函 数 说 明：写一个字节到DS18B20
 * 函 数 形 参：dat：要写入的字节
 * 函 数 返 回：无
******************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t i;
    uint8_t testb;
    DQ_OUT();//设置输出模式
    for (i=0; i<8; i++)
    {
        if ( (dat&0x01) ) //写1
        {
            DQ(0);
            delay_us(2);
            DQ(1);
            delay_us(60);
        }
        else //写0
        {
            DQ(0);//拉低60us
            delay_us(60);
            DQ(1);//释放总线
            delay_us(2);
        }
        dat=dat>>1;//传输下一位
    }
}



/******************************************************************
 * 函 数 名 称：DS18B20_Check
 * 函 数 说 明：检测DS18B20是否存在
 * 函 数 形 参：无
 * 函 数 返 回：1:未检测到DS18B20的存在  0:存在
******************************************************************/
uint8_t DS18B20_Check(void)
{
    uint8_t timeout=0;
    //复位DS18B20
    DQ_OUT(); //设置为输出模式
    DQ(0); //拉低DQ
    delay_us(750); //拉低750us
    DQ(1); //拉高DQ
    delay_us(15);  //15us

    //设置为输入模式
    DQ_IN();
    //等待拉低，拉低说明有应答
    while ( DQ_GET() &&timeout<200)
    {
        timeout++;//超时判断
        delay_us(1);
    };
    //设备未应答
    if(timeout>=200)
        return 1;
    else
        timeout=0;

    //等待18B20释放总线
    while ( !DQ_GET() &&timeout<240)
    {
        timeout++;//超时判断
        delay_us(1);
    };
    //释放总线失败
    if(timeout>=240)
        return 1;

    return 0;
}

/******************************************************************
 * 函 数 名 称：DS18B20_Start
 * 函 数 说 明：DS18B20开始温度转换
 * 函 数 形 参：无
 * 函 数 返 回：无
******************************************************************/
void DS18B20_Start(void)
{
    DS18B20_Check();            //查询是否有设备应答
    DS18B20_Write_Byte(0xcc);   //对总线上所有设备进行寻址
    DS18B20_Write_Byte(0x44);   //启动温度转换
}

/******************************************************************
 * 函 数 名 称：DS18B20_GetTemperture
 * 函 数 说 明：从ds18b20得到温度值
 * 函 数 形 参：无
 * 函 数 返 回：温度数据
******************************************************************/
float DS18B20_GetTemperture(void)
{
    uint16_t tempe;
    uint8_t dataL=0,dataH=0;
    float value;

    DS18B20_Start();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);//对总线上所有设备进行寻址
    DS18B20_Write_Byte(0xbe);// 读取数据命令
    dataL=DS18B20_Read_Byte(); //LSB
    dataH=DS18B20_Read_Byte(); //MSB
    tempe=(dataH<<8)+dataL;//整合数据


    if(dataH&0X80)//高位为1，说明是负温度
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
