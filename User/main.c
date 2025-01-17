/**
	@HARDWARE	:	GD32E230C8T6    使用主频72Mhz    晶振8Mhz
  @VISION		: V1.0
	@AUTHOR		: RUBO
	@DATE			: 2024年7月9日
	@FUNCTION	: 热解粒子主控板程序
*/

#include "main.h"

#define N 5   	// 算数平均滤波系数
#define M 8			// 中值滤波系数

/**
	@brief: 串口中断处理函数
*/
void USART0_IRQHandler(void)
{
    //接收中断
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
        int_RX();
    }
}
/**
	@brief: 定时器2中断处理函数，定时周期1ms
*/
void TIMER2_IRQHandler(void)
{
    if ( timer_interrupt_flag_get(TIMER2, TIMER_INT_UP) != RESET )
    {
        TIM_Cnt++;
        if(TIM_Cnt >= 500)
        {
            TIM_Cnt = 0;
            // 激光闪亮设置
            gpio_bit_reset(GPIOA, GPIO_PIN_15);
            gpio_bit_set(GPIOA, GPIO_PIN_8);
        }
        else
        {
            if(TIM_Cnt == LEDP_TIME) SEC_LDOP_TEST = 1;//激光值采集时间点到
            if(TIM_Cnt == BLUE_LEDP_TIME) SEC_BLUE_LDOP_TEST = 1;
            if(TIM_Cnt == SMOKE_TIME) SEC_SMOKE_TEST = 1;//烟雾值采集时间点到
            if(TIM_Cnt == LAser_TIME) {
                gpio_bit_set(GPIOA, GPIO_PIN_15);
                gpio_bit_reset(GPIOA, GPIO_PIN_8);//关闭激光管
            }
        }
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP); //清除中断标志位
    }
}

/***********************   主函数    *******************************/

int main(void)
{
    int k = 0;
    uint32_t adc_ave = 0;
    uint32_t adc_blue_ave = 0;
    uint8_t adc_degree = 4;  //读取的次数的幂 2^4

    /***************** 初始化配置 ****************/
    systick_config(); // 时钟配置
    rcu_config();     // 外设时钟
    gpio_init();			// GPIO
    nvic_config();	  // NVIC
    adc_config();			// ADC
    timer_config();		// 定时器
    AT24CXX_Init();		// 存储芯片
    gd_eval_com_init(USART0);//串口 波特率 19200
    DS18B20_GPIO_Init(); // 温度
    SGP30_Init();  // TVOC
    RS485_init_data();//通信数据

    //开启串口RBNR中断：读取数据缓冲区不为空中断和溢出
    while (RESET == usart_flag_get(USART0 , USART_FLAG_TC));
    usart_interrupt_enable(USART0, USART_INT_RBNE);

    // 此处源代码逻辑修改至 data.c 文件中，封装为函数执行
    calibration_init();

    uint32_t temp;   // 临时变量
    uint8_t y;       // 临时变量
		float MS1100_data;
		
	
    while(1)
    {
			printf("%.1f, %d, %d, %.2f\n", DS18B20_GetTemperture(), SGP30_Get_CO2_Value(), 
																			SGP30_Get_TVOC_Value(), (Get_MS1100_ADC_Value(30) / 4095.0) * 3.3);
        //SGP30_Get_Value();
			//printf("%d\n", sgp_data.CO2_val);
			//printf("%d\n", sgp_data.TVOC_val);
        //printf("TVOC: %d\r\n",middleValueFilter());
				//MS1100_data = (Get_MS1100_ADC_Value(30) / 4095.0) * 3.3;
			//printf("%.2f\n", MS1100_data);
        //printf("TVOC: %d\r\n",SGP30_Get_TVOC_Value());
        delay_ms(500);

        //1. 检测激光模拟量定时到
        if(SEC_LDOP_TEST)
        {
            SEC_LDOP_TEST = 0;

            adc_ave = 0;
            for(k = 0; k < (1 << adc_degree); k++)
            {
                //读取ADC次数
                //此处的channel 5代表红光检测反馈值
                adc_ave += get_adc_Average(ADC_CHANNEL_5) * 5;
            }
            //这里循环8次读取，因此>>4位就是除以16
            adc_ave =  (adc_ave >> adc_degree);

            adcData.LDOPowerIn.data_16 = adc_ave;		// 获得激光值

            //printf("RED_laser_val: %d\r\n", adcData.LDOPowerIn.data_16);


            //adcData.LDOPowerIn.data_16 = get_adc(ADC_CHANNEL_1);		// 获得激光值  get_adc(channel)
        }
				
				// 蓝激光检测
        if(SEC_BLUE_LDOP_TEST)
        {
            SEC_BLUE_LDOP_TEST = 0;

            adc_blue_ave = 0;
            for(k = 0; k < (1 << adc_degree); k++)
            {
                //读取ADC次数
                // 此处的channel 5代表红光检测反馈值
                adc_blue_ave += get_adc_Average(ADC_CHANNEL_6) * 5;
            }
            //这里循环8次读取，因此>>4位就是除以16
            adc_blue_ave =  (adc_blue_ave >> adc_degree);

            adcData.LDO_Blue_PowerIn.data_16 = adc_blue_ave;		// 获得激光值

            //printf("BLUE_laser_val: %d\r\n", adcData.LDO_Blue_PowerIn.data_16);


            //adcData.LDOPowerIn.data_16 = get_adc(ADC_CHANNEL_1);		// 获得激光值  get_adc(channel)
        }

        //2. 检测烟雾模拟量定时到
        if(SEC_SMOKE_TEST)
        {
            SEC_SMOKE_TEST = 0;//清零模拟量检测20mS到定时标记

            temp_adc = get_adc_Average(ADC_CHANNEL_4);//取16次AD的平均值
            //平滑滤波
            SmokeIns_group[count++] = temp_adc;//保存当前采集数据
            if(count>39)           //已经采集足够40次
            {
                for(y=0; y<40; y++)	Paixu_group[y] = SmokeIns_group[y] ;
                mabub(&Paixu_group[0],40);            //是冒泡排序算法，主要是简单，占用程序空间少
                sum = 0;
                for(y=4; y<36;)                             //计算去掉最大最小值各4个的数据和
                    sum +=Paixu_group[y++];
                group_trans( &SmokeIns_group[0],40 );      //将数组中数据前移一个位置，第一位置数据抛弃
                //now_smoke = (uint16_t)( sum >> 4 ); //只取了16次采集数据的累积值的平均值
                adcData.SmokeIn.data_16 = (uint16_t)( sum >> 5 ); //只取了16次采集数据的累积值的平均值

                //printf("smoke_val: %d\r\n", adcData.SmokeIn.data_16);


                if(OBS_OVER_F)//OBS标记完成，计算减光率值
                {
                    if(OBS_BIAS &0X8000) //有偏置为负标记
                    {
                        adcData.SmokeIn.data_16 += (OBS_BIAS&0X7FFF);   //减去纯净气体下的偏置
                        if ( adcData.SmokeIn.data_16 > 4095 )	adcData.SmokeIn.data_16 = 4095;
                    }
                    else
                    {
                        if(adcData.SmokeIn.data_16 > OBS_BIAS)	adcData.SmokeIn.data_16 -= OBS_BIAS;   //减去纯净气体下的偏置
                        else adcData.SmokeIn.data_16 = 0;
                    }
                    temp = adcData.SmokeIn.data_16 << 4;
                    adcData.OBS.data_16 = (uint16_t)(temp / OBS_Y.data_16); //求当前检测数据OBS值
                } else adcData.OBS.data_16 = 0;

                count=39;//重新赋值计数器，保证下次采集数据落到最后位置
            }
        }

        // 3. 传输检测值
        //RS485_PROG();//命令处理子函数

    }
}


/**
	  @brief: 求adc值总和
*/
uint16_t sumAdcs(uint16_t s[]) {
    int i=0;
    sum=0;
    maxn=0;
    minn=0xFFFF;
    for(i=0; i<18; i++) {
        if(maxn<s[i]) maxn=s[i];
        if(minn>s[i]) minn=s[i];
        sum+=s[i];
    }
    return (sum-maxn-minn)/16;
}

/**
		\brief: 数组平移，将一个数组当中的数据向前移动一个位置，第一位数据抛弃
*/
void group_trans(uint16_t *group_array,uint8_t n)
{
    uchar group_i=1;
    for(; group_i<n; group_i++)
        *(group_array+group_i-1)=*(group_array+group_i);
}

//-------------------------------------------------------------------
//函数说明：冒泡排序算法，主要是简单，占用程序空间少
//*p：数组指针  n：要排序的数据个数  //最大数组字节可达到255个字节
//输出：递增数组
//-------------------------------------------------------------------
void mabub(uint16_t *p,uint8_t n)//是冒泡排序算法，主要是简单，占用程序空间少
{
    uint8_t m,k,j,i;
    uint16_t d;
    k=0;
    m=n-1;
    while(k<m)
    {
        j=m-1;
        m=0;
        for (i=k; i<=j; i++)
            if ((*(p+i))>(*(p+i+1)))
            {
                d=*(p+i);
                *(p+i)=*(p+i+1);
                *(p+i+1)=d;
                m=i;
            }
        j=k+1;
        k=0;
        for (i=m; i>=j; i--)
            if ((*(p+i-1))>(*(p+i)))
            {
                d=*(p+i);
                *(p+i)=*(p+i-1);
                *(p+i-1)=d;
                k=i;
            }
    }
}


/**
    \brief      configure the nested vectored interrupt controller
  */
void nvic_config(void)
{
    nvic_irq_enable(USART0_IRQn, 1);
    nvic_irq_enable(TIMER2_IRQn, 0);	//设置中断优先级
}


/*!
    \brief      RCU configuration function
*/
void rcu_config(void)
{
    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* ADCCLK = PCLK2/6 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);//配置ADC时钟来源
    rcu_periph_clock_enable(RCU_ADC);//使能ADC时钟

    //修改为定时器2
    rcu_periph_clock_enable(RCU_TIMER2);
}


/*!
    \brief      TIMER configuration function
*/
void timer_config(void)
{
    timer_parameter_struct timer_initpara;
    timer_deinit(TIMER2);
    timer_struct_para_init(&timer_initpara);

    /* TIMER2 定时器参数配置 */
    timer_initpara.prescaler         = 72 - 1;//设置预分频值,72MHz时钟
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;//对齐模式
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;//计数方向，向上计数
    timer_initpara.period            = 1000 - 1;//设置自动重装载周期值
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;//时钟分频因子
    timer_initpara.repetitioncounter = 0;//重复计数器
    timer_init(TIMER2,&timer_initpara);

    timer_interrupt_enable(TIMER2, TIMER_INT_UP);//使能更新（溢出）中断
    timer_enable(TIMER2);///使能TIMER
}

/*!
    \brief      算数平均滤波
*/
u16 averageFilter(void)
{
    u16 sum = 0;
    u8 i;
    for(i = 0; i < N; ++i)
    {
        //sum += (u16)SGP30_Get_TVOC_Value();     	// 检测 SGP30
        sum += (u16)get_adc_Average(ADC_CHANNEL_7);	// 检测 MS1100

    }
    return sum/N;
}

/*!
    \brief      中值滤波
*/
u16 middleValueFilter(void)
{
    u16 value_buf[M];
    u16 i,j,k,temp;
    for( i = 0; i < M; ++i)
    {
        value_buf[i] = averageFilter();

    }
    for(j = 0 ; j < M-1; ++j)
    {
        for(k = 0; k < M-j-1; ++k)
        {
            //从小到大排序，冒泡法排序
            if(value_buf[k] > value_buf[k+1])
            {
                temp = value_buf[k];
                value_buf[k] = value_buf[k+1];
                value_buf[k+1] = temp;
            }
        }
    }

    return value_buf[(M-1)/2];
}

/*!
    \brief      MS1100 ADC读取均值
*/
u16 Get_MS1100_ADC_Value(u16 num)
{
    u16 Data=0;
    u8 i = 0;
    for( i = 0; i < num; i++ )
    {
        /*        读取ADC常规组数据寄存器  */
        Data += (u16)get_adc_Average(ADC_CHANNEL_7);
        delay_1ms(1);
    }
    Data = Data/num;

    return Data;
}

/*!
    \brief      相当于“中位值滤波法”+“算术平均滤波法”
								连续采样N个数据，去掉一个最大值和一个最小值然后计算N-2个数据的算术平均值
*/
#define Count 10
u16 middleAverageFilter()
{
    u16 i,j,k;
    u16 temp,sum = 0;
    u16 value_buf[Count];
    for(i = 0; i < Count; ++i)
    {
        //value_buf[i] = getValue();
    }
    /*从小到大冒泡排序*/	
    for(j = 0; j < Count-1; ++j)
    {
        for(k = 0; k < Count-j-1; ++k)
        {
            if(value_buf[k] > value_buf[k+1])
            {
                temp = value_buf[k];
                value_buf[k] = value_buf[k+1];
                value_buf[k+1] = temp;
							
            }
        }
    }
    for(i = 1; i < Count-1; ++i)
    {
        sum += value_buf[i];
    } 
    return sum/(Count-2);
}

