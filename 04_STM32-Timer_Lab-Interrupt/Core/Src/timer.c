/*
 * timer.c
 *
 *  Created on: Sep 6, 2024
 *      Author: HFA
 */
#include "timer.h"
#include "stm32f7xx_hal.h"


void TIM2_Init(void)
{
    // Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // Prescaler and Auto-reload preload enable
	// 168 MHz / (167 + 1) = 1 MHz (1 μs per count)
    TIM2->PSC = 167;
    // 1000 counts = 1 ms
    TIM2->ARR = 1000 - 1;
    // Update interrupt enable
    TIM2->DIER |= TIM_DIER_UIE;
    // Counter enable
    TIM2->CR1 |= TIM_CR1_CEN;

    // Delete after initialization is complete.
    vTaskDelete(NULL);
}

void NVIC_Update(void)
{
	// TIM2 interrupt'ını etkinleştir
    NVIC_EnableIRQ(TIM2_IRQn);
    // TIM2 interrupt priority is maximum
    NVIC_SetPriority(TIM2_IRQn, 0);

    // NVIC'te TIM2 interrupt'ı için yapılandırma
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);  // Öncelik seviyesi 0 (en yüksek)
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          // TIM2 interrupt'ını etkinleştir

    // Delete after initialization is complete.
    vTaskDelete(NULL);
}

void TIM2_IRQHandler(void)
{
    // Update Interrupt Flag)
    if (TIM2->SR & TIM_SR_UIF)
    {
    	// Clean UIF flag
        TIM2->SR &= ~TIM_SR_UIF;

        // Do something when timer interrupt occurs
       /*
        *
        */
    }
}
