 /******************************************************************************
   * 测试硬件：GD32E230C8T6    使用主频72Mhz    晶振8Mhz
   * 版 本 号: V1.0
   * 修改作者: rubo
   * 修改日期: 2024年7月9日
   * 功能介绍: 热解粒子主控板程序
  ******************************************************************************/
#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>


int main(void)
{
    systick_config();

    /* enable the LED1 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* configure LED1 GPIO port */ 
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    /* reset LED1 GPIO pin */
    gpio_bit_reset(GPIOC,GPIO_PIN_13);


    while(1)
		{

        gpio_bit_set(GPIOC,GPIO_PIN_13);
        delay_1ms(1000);
        gpio_bit_reset(GPIOC,GPIO_PIN_13);
        delay_1ms(1000);
		delay_ms(1);
		delay_us(1);

    }
}
