#include "PORG_RS485.h"
#include "gd32f350r_eval.h"
//#include "gd32f3x0_it.h"
#include "gd32e23x_it.h"
#include "data.h"
#include "24cxx.h"

//共用控制位
RS485_BIT_STRUCT RS485_C;

uchar 		esadd										 ;//接收到的地址码
uchar 		esdata									 ;//接收到的命令码
uchar     R_DATA[TR_MAX_COUNTER]   ;//RS485收缓冲区
uchar     T_DATA[TR_MAX_COUNTER]   ;//RS485发缓冲区
uchar     T_counter                ;//RS485发送数据总数
uchar     R_counter                ;//RS485接收数据总数
uchar     TR_JISHU                 ;//RS485收发计数器
uchar     rs_485_contror           ;//rs485通讯处理控制字

uchar     net_control_data				 ;//网络控制字

union  GLQ_TEST
{
    unsigned long   ADATA;
    uint8_t  adata[4];
};



//;-------------------------------------------------------------------
//;RS485_INI_DATA();//RS485数据初始化子程序
//;-------------------------------------------------------------------
void RS485_init_data()
{
    uchar i=0;
    for(i=0; i<TR_MAX_COUNTER; i++)     R_DATA[i]=0; //RS485收发缓冲区
    RS485_NEED = 0      ;
    T_counter = 0       ;//RS485发送数据总数
    R_counter = 0       ;//RS485接收数据总数
    TR_JISHU = 0        ;//RS485收发计数器
    rs_485_contror = 0  ;//RS485通讯处理控制字
    RS485_RX						;//RS485接收状态
    RS485_RX_Statue = 0 ;//RS485不需要置发送状态
}


//;-------------------------------------------------------------------
//out_rs485();    //rs485发送数据处理子程序
//返回值：无
//参数：i 本次发送数据字节总数
void out_rs485(uchar i)
{
    uchar y = 0;
    RS485_TX;      				//允许发送
    //T_counter = i;						//设置发送总数
    //TR_JISHU = 0;							//计数已经发送的字节数
    for(y=0; y < i; ++y)
    {
        usart_data_transmit(USART0,(uint8_t)T_DATA[y]);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));//等待数据发送完成
    }
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));//等待数据转换完成
    TR_JISHU = 0;
    RS485_RX;
    //usart_interrupt_enable(EVAL_COM1, USART_INT_TBE);//开启发送中断
}



//;-------------------------------------------------------------------
//rs485通信处理子程序
//;-------------------------------------------------------------------
void RS485_PROG() 
{
    uchar i = 0;
    //data OBS_AIR,Smoke_AIR,OBS_N2,Smoke_N2;
    uint8_t temp8 = 0;
    uint16_t temp16 = 0;
    data t_anlog;
    if(RS485_NEED)//RS485需要处理
    {
        RS485_NEED = 0;
        switch(rs_485_contror) //rs485通讯处理控制字
        {
        case 0xc2 : //1000 0010――>采集参数查询
        {
            /*
            回答命令：
            1字节： 命令码

            2字节： 模组烟雾模拟量高4位
            3字节： 模组烟雾模拟量低8位

            4字节： 激光管功率模拟量值高4位
            5字节： 激光管功率模拟量值低8位

            6字节： 模组减光率（OBS）值高4位
            7字节： 模组减光率（OBS）值低8位

            8字节： 校验和

            */
            T_DATA[0] = rs_485_contror;

            T_DATA[1] = adcData.SmokeIn.data_8[1];
            T_DATA[2] = adcData.SmokeIn.data_8[0];

            T_DATA[3] = adcData.LDOPowerIn.data_8[1];
            T_DATA[4] = adcData.LDOPowerIn.data_8[0];

            T_DATA[5] = adcData.OBS.data_8[1];
            T_DATA[6] = adcData.OBS.data_8[0];

            t_anlog.data_16 = 0;
            for(i=0; i<=6; i++)               //求校验和
                t_anlog.data_16 += T_DATA[i]; //采用双字节，防止溢出出错

            T_DATA[7] = t_anlog.data_8[0];//校验和
            out_rs485(8);//回复8字节数据
            break;

        }

        /*
        OBS标定命令，统一回复5字节：
        	1字节――命令码
        	2字节――OBS高四位
        	3字节――OBS低八位
        	4字节――OBS_flag
        	5字节――校验和
        */
        case 0xc3 : //0000  0011――>无烟标定
        {
            /*
            主控发送：
            	1字节――命令地址码
            	2字节——命令地址码
            	3字节——标定代码
            	4字节――探测模组烟雾模拟量采集值高四位
            	5字节——探测模组烟雾模拟量采集值低八位
            	6字节――OBS标定值高8－11位
            	7字节——OBS标定值低0－7位
            	8字节——校验位
            接收到：
            	1字节――命令地址码
            	2字节——标定代码
            	3字节――探测模组烟雾模拟量采集值高四位
            	4字节——探测模组烟雾模拟量采集值低八位
            	5字节――OBS标定值高8－11位
            	6字节——OBS标定值低0－7位
            	7字节——校验位
            */
            t_anlog.data_16 = 0;
            for(i=0; i<=5; i++)               //求校验和
                t_anlog.data_16 += R_DATA[i];//采用双字节，防止溢出出错
            if(R_DATA[6] == t_anlog.data_8[0])//校验正确
            {
                if(R_DATA[1] == 0)//无烟标定,氮气标定
                {
                    N2_OBS_F  = 1 ;                //无烟氮气本底已经存储
                    OBS_FLAG|=0X02 ;								//OBS_FLAG.1,氮气无烟标记


                    Smoke_N2.data_8[1] = R_DATA[2];
                    Smoke_N2.data_8[0] = R_DATA[3];
                    OBS_N2.data_8[1] = R_DATA[4];
                    OBS_N2.data_8[0] = R_DATA[5];
                    //写当前空气情况下烟雾值和OBS值
                    temp16 = AT24CXX_ReadLenByte(0x01,2);
                    if(temp16 != Smoke_N2.data_16)	AT24CXX_WriteLenByte(0x01,Smoke_N2.data_16,2);

                    temp16 = AT24CXX_ReadLenByte(0x03,2);
                    if(temp16 != OBS_N2.data_16)	AT24CXX_WriteLenByte(0x03,OBS_N2.data_16,2);

                }
                if(R_DATA[1] == 1)//有烟标定，空气标定
                {
                    AIR_OBS_F  = 1 ;                //空气本底已经存储
                    OBS_FLAG|=0X01 ;								//OBS_FLAG.0,空气有烟标记


                    Smoke_AIR.data_8[1] = R_DATA[2];
                    Smoke_AIR.data_8[0] = R_DATA[3];
                    OBS_AIR.data_8[1] = R_DATA[4];
                    OBS_AIR.data_8[0] = R_DATA[5];
                    //写当前情况下烟雾值和OBS值
                    temp16 = AT24CXX_ReadLenByte(0x05,2);
                    if(temp16 != Smoke_AIR.data_16)	AT24CXX_WriteLenByte(0x05,Smoke_AIR.data_16,2);

                    temp16 = AT24CXX_ReadLenByte(0x07,2);
                    if(temp16 != OBS_AIR.data_16)	AT24CXX_WriteLenByte(0x07,OBS_AIR.data_16,2);

                }
                if(R_DATA[1] == 2)//清无烟标定
                {

                    N2_OBS_F   = 0 ;          //清零氮气OBS偏置已经标定标记
                    OBS_OVER_F = 0 ;          //清零OBS标定计算完毕标记
                    OBS_FLAG  &=0X0f9 ;       //清零OBS标定保存数据	OBS_FLAG.1,氮气标记，OBS_FLAG.2,计算完毕标记
                }
                if(R_DATA[1] == 3)//清有烟标定
                {
                    AIR_OBS_F  = 0 ;          //清零空气OBS偏置已经标定标记
                    OBS_OVER_F = 0 ;          //清零OBS标定计算完毕标记
                    OBS_FLAG  &=0X0fa;        //清零OBS标定保存数据//OBS_FLAG.0,空气标记，OBS_FLAG.2,计算完毕标记
                }
                if(R_DATA[1] == 4)//全清标定
                {
                    AIR_OBS_F  = 0 ;          //清零空气OBS偏置已经标定标记，OBS_FLAG.0,空气标记
                    N2_OBS_F   = 0 ;          //清零氮气OBS偏置已经标定标记，OBS_FLAG.1,氮气标记
                    OBS_OVER_F = 0 ;          //清零OBS标定计算完毕标记，OBS_FLAG.2,计算完毕标记
                    OBS_FLAG  &=0X0f8;        //清零OBS标定保存数据
                }
                if(1 < R_DATA[1] && R_DATA[1] < 5)//只要清除了任意标定值，都清除原来的标定计算值
                {

                    OBS_Y.data_16 = 0;
                    temp16 = AT24CXX_ReadLenByte(0x09,2);
                    if(temp16 != OBS_Y.data_16)	AT24CXX_WriteLenByte(0x09,OBS_Y.data_16,2);

                    OBS_BIAS = 0;
                    temp16 = AT24CXX_ReadLenByte(0x0b,2);
                    if(temp16 != OBS_BIAS)	AT24CXX_WriteLenByte(0x0b,OBS_BIAS,2);
                }

                if ((( N2_OBS_F & AIR_OBS_F ) == 1 ) & ( OBS_OVER_F == 0 ))
                {
                    Smoke_N2.data_16 = AT24CXX_ReadLenByte(0x01,2);//读当前氮气情况下OBS值
                    OBS_N2.data_16 = AT24CXX_ReadLenByte(0x03,2);

                    Smoke_AIR.data_16 = AT24CXX_ReadLenByte(0x05,2);//读当前空气情况下OBS值
                    OBS_AIR.data_16 = AT24CXX_ReadLenByte(0x07,2);
                    if (( Smoke_AIR.data_16 > Smoke_N2.data_16 )                      //数据正常才计算
                            & ( OBS_AIR.data_16 > OBS_N2.data_16 ))
                    {
                        OBS_OVER_F = 1 ;
                        //N2_OBS_F   = 0 ;
                        //AIR_OBS_F  = 0 ;
                        OBS_FLAG   = 0X07;
                        Smoke_AIR.data_16 -= Smoke_N2.data_16;//计算烟模拟量差值
                        OBS_AIR.data_16 -= OBS_N2.data_16;//计算OBS差值

                        OBS_Y.data_16 = ((Smoke_AIR.data_16)<<4) / (OBS_AIR.data_16)  ;//计算对应OBS系数值,放大16倍


                        Smoke_AIR.data_16 = ((OBS_Y.data_16 * OBS_N2.data_16)>>4) ;//计算根据斜率得到的烟模拟量值
                        if(Smoke_N2.data_16 > Smoke_AIR.data_16 )
                            OBS_BIAS = Smoke_N2.data_16  - Smoke_AIR.data_16  ;//计算纯净空气下的模拟量值
                        else  {
                            OBS_BIAS = Smoke_AIR.data_16  - Smoke_N2.data_16   ;//计算纯净空气下的模拟量值
                            OBS_BIAS |= 0X8000;//设置偏置为负标记
                        }

                        temp16 = AT24CXX_ReadLenByte(0x09,2);
                        if(temp16 != OBS_Y.data_16)	AT24CXX_WriteLenByte(0x09,OBS_Y.data_16,2);//写当前OBS标定系数

                        temp16 = AT24CXX_ReadLenByte(0x0b,2);
                        if(temp16 != OBS_BIAS)	AT24CXX_WriteLenByte(0x0b,OBS_BIAS,2);//写当前OBS标定模拟量本底

                    }
                }

                temp8 = AT24CXX_ReadOneByte(0x0d);
                if(temp8 != OBS_FLAG)	AT24CXX_WriteOneByte(0x0d,OBS_FLAG);//写当前OBS标定情况

                //回复5字节
                T_DATA[0] = rs_485_contror;
                T_DATA[1] = OBS_Y.data_8[1];
                T_DATA[2] = OBS_Y.data_8[0];
                T_DATA[3] = OBS_FLAG;
                t_anlog.data_16 = 0;
                for(i=0; i<=3; i++)               //求校验和
                    t_anlog.data_16 += T_DATA[i]; //采用双字节，防止溢出出错

                T_DATA[4] = t_anlog.data_8[0];//校验和
                out_rs485(5);//回复5字节数据
            }
            else//校验错误
            {

                TR_JISHU = 0;//重新接收
            }
            break;
        }
        case 0xc4 : //0000  0100――>标定参数传输
        {
            //回复5字节
            T_DATA[0] = rs_485_contror;
            T_DATA[1] = OBS_Y.data_8[1];
            T_DATA[2] = OBS_Y.data_8[0];
            T_DATA[3] = OBS_FLAG;
            t_anlog.data_16 = 0;
            for(i=0; i<=3; i++)               //求校验和
                t_anlog.data_16 += T_DATA[i]; //采用双字节，防止溢出出错

            T_DATA[4] = t_anlog.data_8[0];//校验和
            out_rs485(5);//回复5字节数据
            break;
        }
        default :
        {
            break;
        }
        }
        rs_485_contror = 0 ;//清零处理控制字
        TR_JISHU = 0;
    }
}


//;-------------------------------------------------------------------
//int_RX();    	//rs485通信读取中断处理函数
//;-------------------------------------------------------------------
/*
探测器命令地址码发送2字节：
1字节――命令地址码
2字节——命令地址码
*/
void int_RX(void)
{
    //timer_enable(TIMER13);
    //timer_counter_value_config(TIMER13,0);
    //命令地址码两次不一致时，要保证TR_JISHU为0，方便下一次接收数据正常
	
    if(TR_JISHU == 0)//RS485接收数据命令地址码
    {
        R_DATA[0] = usart_data_receive(USART0);//接收命令地址码

        if( net_control_data == R_DATA[0]) //相同，说明是第2次接收命令地址字节
        {
            net_control_data = 0;
            esadd = R_DATA[0] >> 4;      //取地址码
            esdata  = R_DATA[0]&0X0F;      //取命令
            rs_485_contror = R_DATA[0] ; //此处保存发送来的命令代码,备后续数据接收处理用
            switch(esdata)
            {
            case 0x01 : //0001――>预留
            {
                //
                break;
            }
            case 0x02 : //0010――>模组采集参数查询
            {
                if( esadd == benji_add) //地址正确
                {
                    RS485_NEED = 1;//RS485需处理标,记＝1,需要处理,＝0,不需处理
                    rs_485_contror = R_DATA[0]  ;//rs485通讯处理控制字
                }
                break;
            }
            case 0x03: //0011――>OBS标定,无烟标定
            {
                /*
                主控发送：
                	1字节――命令地址码
                	2字节——命令地址码
                	3字节——标定代码
                	4字节――探测模组烟雾模拟量采集值高四位
                	5字节——探测模组烟雾模拟量采集值低八位
                	6字节――OBS标定值高8－11位
                	7字节——OBS标定值低0－7位
                	8字节——校验位

                */
                if( esadd == benji_add) //地址正确
                {
                    TR_JISHU = 1;//收发计数器开始计数
                    R_counter = 7;//接收8字节，命令地址码重发接收，需要减一
                    rs_485_contror = R_DATA[0]  ;//rs485通讯处理控制字
                }
                break;
            }
            case 0x04 : //0100――>OBS标定参数查询
            {
                if( esadd == benji_add ) //地址正确
                {
                    RS485_NEED = 1;//RS485需处理标,记＝1,需要处理,＝0,不需处理
                    rs_485_contror = R_DATA[0]  ;//rs485通讯处理控制字
                }
                break;
            }

            default : //未知命令
            {
                break;
            }
            }
        }
        else//还未接收到命令地址码
        {
            net_control_data = R_DATA[0];//不一定是正确的命令地址码
        }
    }
    else//接收数据字节
    {
        R_DATA[TR_JISHU++] = usart_data_receive(EVAL_COM1);
        if(TR_JISHU == R_counter)//已经接收到足够字节
        {
            //校验位校验
            RS485_NEED = 1;      //;RS485需处理标记=1,需要,=0,不需
        }
    }
    if(RS485_NEED == 1)//RS485需处理标记=1,说明本次通信接收成功
    {
        //****************//
    }
}


//;-------------------------------------------------------------------
//int_TX();    	//rs485通信发送中断处理函数
//;-------------------------------------------------------------------
/*
主板命令地址码发送2字节：
1字节――命令码
2字节——地址码
*/
void int_TX(void)
{
    if ( TR_JISHU < T_counter )//数据没发送完
    {
        usart_data_transmit(USART0, T_DATA[TR_JISHU++]);
    }
    else { //数据发送完
        usart_interrupt_disable(USART0, USART_INT_TBE);
        TR_JISHU       = 0;
        T_counter      = 0;
        R_counter      = 0;
        rs_485_contror = 0;  //rs485通讯处理控制字
        RS485_RX_Statue = 1; //rs485需要置接收
    }
}



