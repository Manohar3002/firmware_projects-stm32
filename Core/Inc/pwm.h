/*
 * pwm.h
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "main.h"

extern uint8_t duty;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

void treatment_led(uint8_t duty);
void treatment_led_off();

#endif /* INC_PWM_H_ */
