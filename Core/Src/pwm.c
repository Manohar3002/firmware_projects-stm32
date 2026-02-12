/*
 * pwm.c
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */


#include "pwm.h"


uint8_t duty;

void treatment_led(uint8_t duty){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);

}
void treatment_led_off(){
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}
