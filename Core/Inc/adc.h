/*
 * adc.h
 *
 *  Created on: Oct 29, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"



extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;
uint8_t Get_Battery_Percentage(void);



#endif /* INC_ADC_H_ */
