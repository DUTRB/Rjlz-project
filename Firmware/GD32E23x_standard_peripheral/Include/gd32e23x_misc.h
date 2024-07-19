/*!
    \file  gd32e23x_misc.h
    \brief definitions for the MISC
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/



#ifndef GD32E23X_MISC_H
#define GD32E23X_MISC_H

#include "gd32e23x.h"

/* constants definitions */
/* set the RAM and FLASH base address */
#define NVIC_VECTTAB_RAM            ((uint32_t)0x20000000)                      /*!< RAM base address */
#define NVIC_VECTTAB_FLASH          ((uint32_t)0x08000000)                      /*!< Flash base address */

/* set the NVIC vector table offset mask */
#define NVIC_VECTTAB_OFFSET_MASK    ((uint32_t)0x1FFFFF80)                      /*!< NVIC vector table offset mask */

/* the register key mask, if you want to do the write operation, you should write 0x5FA to VECTKEY bits */
#define NVIC_AIRCR_VECTKEY_MASK     ((uint32_t)0x05FA0000)                      /*!< NVIC VECTKEY mask */

/* choose the method to enter or exit the lowpower mode */
#define SCB_SCR_SLEEPONEXIT         ((uint8_t)0x02)                             /*!< choose the the system whether enter low power mode by exiting from ISR */
#define SCB_SCR_SLEEPDEEP           ((uint8_t)0x04)                             /*!< choose the the system enter the DEEPSLEEP mode or SLEEP mode */
#define SCB_SCR_SEVONPEND           ((uint8_t)0x10)                             /*!< choose the interrupt source that can wake up the lowpower mode */

#define SCB_LPM_SLEEP_EXIT_ISR      SCB_SCR_SLEEPONEXIT                         /*!< low power mode by exiting from ISR */
#define SCB_LPM_DEEPSLEEP           SCB_SCR_SLEEPDEEP                           /*!< DEEPSLEEP mode or SLEEP mode */
#define SCB_LPM_WAKE_BY_ALL_INT     SCB_SCR_SEVONPEND                           /*!< wakeup by all interrupt */

/* choose the systick clock source */
#define SYSTICK_CLKSOURCE_HCLK_DIV8 ((uint32_t)0xFFFFFFFBU)                     /*!< systick clock source is from HCLK/8 */
#define SYSTICK_CLKSOURCE_HCLK      ((uint32_t)0x00000004U)                     /*!< systick clock source is from HCLK */

/* function declarations */

/* enable NVIC request */
void nvic_irq_enable(uint8_t nvic_irq, uint8_t nvic_irq_priority);
/* disable NVIC request */
void nvic_irq_disable(uint8_t nvic_irq);
/* initiates a system reset request to reset the MCU */
void nvic_system_reset(void);

/* set the NVIC vector table base address */
void nvic_vector_table_set(uint32_t nvic_vict_tab, uint32_t offset);

/* set the state of the low power mode */
void system_lowpower_set(uint8_t lowpower_mode);
/* reset the state of the low power mode */
void system_lowpower_reset(uint8_t lowpower_mode);

/* set the systick clock source */
void systick_clksource_set(uint32_t systick_clksource);

#endif /* GD32E23X_MISC_H */
