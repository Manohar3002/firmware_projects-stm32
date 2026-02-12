/*
 * pctrl.c
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */

#include "main.h"

void powerCtrl(){

	HAL_GPIO_WritePin(GPIOC, PWR_CTRL_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	//HAL_GPIO_WritePin(GPIOA, POWER_CTRL_REG_Pin, GPIO_PIN_RESET);

}
