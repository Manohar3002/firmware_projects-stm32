/*
 * touch.c
 *
 *  Created on: Oct 29, 2025
 *      Author: ManoharThokala
 */

#include "main.h"
#include "pctrl.h"
#include "pwm.h"
#include "rgb.h"
#include "routines.h"
#include "stdbool.h"
#include "ble_protocol.h"
#include "touch.h"


#define touch_debounce 1000
#define power_debounce 2000
bool toggle_dev_state;
bool tap1;
uint32_t start_time;



uint32_t tap_time = 0;
uint32_t tch_rls_time = 0;

bool touch_pressed = false;

/*
void touch(void)
{
	if(!dev_charging_flag && !touchlocked_flag){
	// Wait for touch press
    if (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin))
    {
        uint32_t startTime = HAL_GetTick();

        // Keep checking until debounce time
        while (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin))
        {
        	HAL_GPIO_WritePin(GPIOB, led_R_Pin, GPIO_PIN_RESET);
        	HAL_GPIO_WritePin(GPIOA, led_G_Pin, GPIO_PIN_SET);
        	HAL_GPIO_WritePin(GPIOA, led_B_Pin, GPIO_PIN_RESET);

            if (HAL_GetTick() - startTime >= touch_debounce)
            {
                // Toggle once
                toggle_dev_state = !toggle_dev_state;

                if (toggle_dev_state)
                {
                    manual_session_start();
                    man_part_count++;

                }
                else
                {
                    session_end();

                }

                // WAIT HERE until touch is released
                while (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin)){}
                Restore_RGB();
                return;  //exit function
            }
        }
    }

    Restore_RGB();
	}
	if(!dev_charging_flag && touchlocked_flag){
		while (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin))
		        {
		        	HAL_GPIO_WritePin(GPIOB, led_R_Pin, GPIO_PIN_RESET);
		        	HAL_GPIO_WritePin(GPIOA, led_G_Pin, GPIO_PIN_SET);
		        	HAL_GPIO_WritePin(GPIOA, led_B_Pin, GPIO_PIN_SET);
		        }
		Restore_RGB();
	}
}
*/

void power_on(void){

	if (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin))
	    {
	        uint32_t startTime = HAL_GetTick();

	        // Keep checking until debounce time
	        while (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin))
	        {
	            if (HAL_GetTick() - startTime >= power_debounce)
	            {

	            	HAL_GPIO_WritePin(GPIOC, PWR_CTRL_Pin, GPIO_PIN_SET);
	            	return;

	            }

	        }
	    }
}

void touch(){
	if(!dev_charging_flag && !touchlocked_flag){
		bool touch = HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin);
		uint32_t cur_time = HAL_GetTick();

		if(touch && !touch_pressed){

		    start_time = cur_time;
		    touch_pressed = true;
		    while(!man_end_session_flag && HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin) ){
		    	RGB(1, 0, 1);
		    	HAL_Delay(5);

		    	if((HAL_GetTick() - cur_time)>1000){
                // Toggle once
                //toggle_dev_state = !toggle_dev_state;

                //if (toggle_dev_state)
                //{

                    manual_session_start();
                    man_part_count++;

                //}
                //else
                //{
                    //session_end();

                //}

                // WAIT HERE until touch is released
                while (HAL_GPIO_ReadPin(GPIOC, TOUCH_RD_Pin)){}
                Restore_RGB();
                return;  //exit function
		    }
		    	Restore_RGB();
		   }
		}
		if(!touch && touch_pressed){

			touch_pressed = false;
			uint32_t tap_dur = cur_time - start_time;
			if(tap_dur>=30 && tap_dur<=200){
				if(!tap1){
					tap1 = true;
					tch_rls_time = cur_time;
				}
				else {
					if(!end_control_flag && (cur_time - tch_rls_time)<=600){
						tap1 = false;


						session_end();

						//shutdown_routine();
					}
					else tap1 = false;
				}

			}
		}
		if(tap1 && (cur_time - tch_rls_time)>600){
			tap1 = false;

		}
	}
}
