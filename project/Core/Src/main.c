/* Lab 5: STM32F103C8 / TIM2_CH1 / PA0. No board required for simulator. */
#include "stm32f1xx.h"

#ifndef DEMO_PWM
#define DEMO_PWM 1
#endif

volatile uint32_t irq_count, elapsed_ms, led_state, duty_counts;
volatile uint32_t phase_step, breath_cycles, ready;
volatile uint32_t demo_pwm = DEMO_PWM;
/* Debugger reference model only; not the measured PA0 pin level. */
volatile uint32_t pwm_reference;

static void clock_init(void)
{
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
    RCC->CFGR = RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    SystemCoreClockUpdate();
}

static void timer_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 = 0;
#if DEMO_PWM
    GPIOA->CRL = (GPIOA->CRL & ~0xFU) | 0xBU; /* PA0 AF push-pull */
    TIM2->PSC = 71;  /* 72 MHz / 72 = 1 MHz */
    TIM2->ARR = 999; /* PWM period: 1 ms */
    TIM2->CCR1 = 0;
    TIM2->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIM2->CCER = TIM_CCER_CC1E; /* PWM1, active high */
#else
    GPIOA->CRL = (GPIOA->CRL & ~0xFU) | 0x3U; /* PA0 GPIO push-pull */
    GPIOA->BSRR = 1;
    led_state = 1;
    TIM2->PSC = 7199;
    TIM2->ARR = 9999; /* Update every 1 s; toggle -> 2 s full cycle */
#endif
    TIM2->CR1 = TIM_CR1_ARPE;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0; /* Discard the initialization update flag */
    TIM2->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    ready = 1;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = (uint16_t)~TIM_SR_UIF;
        irq_count++;
#if DEMO_PWM
        elapsed_ms++;
        if ((elapsed_ms % 4U) == 0U) {
            uint32_t level;
            phase_step++;
            if (phase_step == 500U) {
                phase_step = 0;
                breath_cycles++;
            }
            level = (phase_step <= 250U) ? phase_step : 500U - phase_step;
            /* Square curve gives finer changes near darkness. */
            duty_counts = (level * level * 1000U) / 62500U;
            TIM2->CCR1 = duty_counts;
        }
#else
        elapsed_ms += 1000;
        led_state ^= 1U;
        GPIOA->BSRR = led_state ? 1U : (1U << 16);
#endif
    }
}

int main(void)
{
    clock_init();
    timer_init();
    while (1) {
#if DEMO_PWM
        /* Read-only comparison reference for Logic Analyzer, NOT PA0 feedback. */
        pwm_reference = (TIM2->CNT < TIM2->CCR1);
#else
        __WFI();
#endif
    }
}
