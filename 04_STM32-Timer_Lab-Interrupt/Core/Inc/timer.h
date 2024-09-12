/*
 * timer.h
 *
 *  Created on: Sep 6, 2024
 *      Author: HFA
 */

#ifndef CORE_INC_TIMER_H_
#define CORE_INC_TIMER_H_

void TIM2_Init(void);
void TIM2_IRQHandler(void);
void NVIC_Update(void);

#endif /* CORE_INC_TIMER_H_ */
