/*
 * routines.c
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */

#include "pwm.h"
#include "rgb.h"
#include "routines.h"
#include "ble_protocol.h"
#include "flash.h"
#include "stdbool.h"
#include "cmsis_gcc.h"


 bool start_control_flag;   //This flag is used for controlling the Treatment session state.
 bool end_control_flag;		//This flag is used for controlling the Idle state.
 bool app_end_session_flag;
 bool man_end_session_flag;
 bool charge_full;


uint32_t session_start_tick;	//A global variable used to store the Starting time of the session.
uint32_t session_end_tick;		//A global variable used to store the End time of the session.



/**
 * @brief performs Status Led checks and Treatment Led checks.
 *
 * This functions is called after booting.
 * 	Shows sequence of 7 colours on status LED for checking rgb LED functionality.
 * 	Checks Treatment LEDs by blinking them once.
 */
void start_routine(){

			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);		//Starts pwm on timer channel 1 for Treatment LEDs.
			HAL_Delay(led_delay);
			HAL_GPIO_WritePin(GPIOC, PWR_CTRL_Pin, GPIO_PIN_SET);
			Set_Color("green");			//GREEN
			HAL_Delay(led_delay);
			Set_Color("blue");			//BLUE
			HAL_Delay(led_delay);
			Set_Color("red");			//RED
			HAL_Delay(led_delay);
			Set_Color("cyan");			//CYAN
			HAL_Delay(led_delay);
			Set_Color("magenta");		//PURPLE
			HAL_Delay(led_delay);
			Set_Color("lemon");			//LEMON
			HAL_Delay(led_delay);
			Set_Color("white");			//WHITE
			HAL_Delay(led_delay);
			Set_Color("off");			//TURN-OFF
			HAL_Delay(led_delay);
			treatment_led(default_duty);
			HAL_Delay(1000);

			treatment_led(0);
			/*Set_Color("green");			//GREEN
			HAL_Delay(led_delay);
			Set_Color("blue");			//BLUE
			HAL_Delay(led_delay);
			Set_Color("red");			//RED
			HAL_Delay(led_delay);
			Set_Color("cyan");			//CYAN
			HAL_Delay(led_delay);
			Set_Color("magenta");		//PURPLE
			HAL_Delay(led_delay);
			Set_Color("lemon");			//LEMON
			HAL_Delay(led_delay);
			Set_Color("white");			//WHITE
			HAL_Delay(led_delay);
			Set_Color("off");			//TURN-OFF*/
			HAL_Delay(200);
}




/**
 * @brief Performs Shutdown process
 *
 * This function shuts down the device after completing shutdown routine.
 * 		Disables interrupt and stops BLE Command processing.
 * 		Shows shutdown status on Status LED by blinking red colour thrice.
 * 		Shutsdown the device using powerCtrl().
 */
void shutdown_routine(){

			//__disable_irq();

			treatment_led(0);
			Set_Color("red");				//RED
			HAL_Delay(led_delay);
			Set_Color("off");				//TURN-OFF
			HAL_Delay(led_delay);
			Set_Color("red");				//RED
			HAL_Delay(led_delay);
			Set_Color("off");				//TURN-OFF
			HAL_Delay(led_delay);
			Set_Color("red");				//RED
			HAL_Delay(led_delay);
			Set_Color("off");				//TURN-OFF
			HAL_Delay(led_delay);

			powerCtrl();
}




/*
 * @brief Executes Ping functionality.
 * This function blinks status led with blue colour.
 * 	Sets status LED colour to blue.S
 * 	Holds this state for 400ms.
 * 	Restores the previous state on status LED.
 */
void ping(){
			//shows blue colour on Status LED for 400ms by controlling RGB Led related GPIOs
			RGB(0, 0, 1);


			HAL_Delay(400);
			Restore_RGB();  //Restores previous RGB state after showing blue status for 400ms
}






//This function ends the 30 minutes Treatment session and puts the device in idle state.
//Starts 3 minutes idle timer.
void session_end(){
	man_end_session_flag = false;
	treatment_led(duty_zero);	//Stops pwm generation and Turns  OFF treatment leds.

	Set_Color("cyan");							//shows CYAN color on status LED.

	session_end_tick = HAL_GetTick();			//Starts 3 minutes idle timer.
	end_control_flag = true;
	//sendResponse(seq, PACK, 0x0, 1);
}

void app_session_end(){
			treatment_led(duty_zero);	//Stops pwm generation and Turns  OFF treatment leds.
			Set_Color("cyan");							//shows CYAN color on status LED.
			session_end_tick = HAL_GetTick();			//Starts 3 minutes idle timer.
			End_session_flag = false;
			end_control_flag = true;
			sendResponse(seq, PACK, 0x0, 1);
}

void manual_session_end(){
			treatment_led(duty_zero);	//Stops pwm generation and Turns  OFF treatment leds.
			Set_Color("cyan");							//shows CYAN color on status LED.
			session_end_tick = HAL_GetTick();			//Starts 3 minutes idle timer.
			End_session_flag = false;
			end_control_flag = true;
			sendResponse(seq, PACK, 0x0, 1);
}

void idle_state(){
	//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);	//Stops pwm generation and Turns  OFF treatment leds.
	treatment_led(0);
	Set_Color("cyan");							//shows CYAN color on status LED.
	session_end_tick = HAL_GetTick();			//Starts 3 minutes idle timer.
	end_control_flag = true;
}
/**
 * @brief starts the Treatment session.
 *
 *  Sets treatment LEDs with 65% PWM
 * 	Stores system time in session_start_tick for tracking session duration.
 */
void session_start(){
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);	//starts pwm on Timer1 channel1 for Treatment Leds.
	Set_Color("lemon");							//shows LEMON color on status LED.
	treatment_led(default_duty);				//Turns ON treatment leds with pwm --> default_duty.
	session_start_tick = HAL_GetTick();			//Starts 30 minutes session timer.

}

void app_session_start(){
	app_end_session_flag =  true;
	session_start();
	man_end_session_flag =  false;
	end_control_flag = false;
	sendResponse(seq, PACK, 0x0, 1);
}

void manual_session_start(){
	app_end_session_flag =  false;
	session_start();
	man_end_session_flag =  true;
	end_control_flag = false;
	sendResponse(seq, PACK, 0x0, 1);
}

/*
 * This function handles the Treatment session.
 * 		Checks Start_session_flag and start Treatment session when Start_session_flag is woken Through the BLE commands.
 *		Checks Treatment session duration and calls session end when session duratio
 */
void session_state(){
	if(!dev_charging_flag){
	  if (app_end_session_flag   &&   (HAL_GetTick() - session_start_tick)>session_duration)
	  	          {

		  	  	  	  uint8_t sess_complt = 0x01;
	  	        	  sendResponse(seq, 0x05, &sess_complt, 1);
	  	        	  app_part_count--;
	  	        	  app_full_count++;
	  	        	  app_perm_count++;
	  	        	  session_end();

	  	          }
	  if (man_end_session_flag   &&   (HAL_GetTick() - session_start_tick)>session_duration)
	  	  	          {

	  		  	  	  	  uint8_t sess_complt = 0x01;
	  	  	        	  sendResponse(seq, 0x05, &sess_complt, 1);
	  	  	        	  man_part_count--;
	  	  	        	  manual_full_count++;
	  	  	        	  manual_perm_count++;
	  	  	        	  session_end();
	  	  	          }

	  if ( end_control_flag &&  (HAL_GetTick() - session_end_tick)> idle_duration) shutdown_routine();
	  //else Set_Color("cyan");
 }
}








/**
 * @brief This function Checks the Charging status of the device.
 * when the device is connected to the powersupply for charging,
 * 				stops bluetooth command processing.
 * 				Ends session if running.
 * 				Shows Red color on Status LED while charging.
 * 				Shows Green color on Status LED When device is fully charged.
 * 				Shuts down the device when disconnected from the charger.
 */
void check_dev_charging() {
	HAL_GPIO_WritePin(GPIOC, PWR_CTRL_Pin, GPIO_PIN_SET);
    uint8_t chg  = HAL_GPIO_ReadPin(GPIOB, BAT_CHG_Pin);   // active low
    uint8_t full = HAL_GPIO_ReadPin(GPIOB, BAT_FULL_Pin);  // active low

    // ---- Determine charging state ----

    bool charging     = (chg == 1 && full == 0);
    	 charge_full  = (chg == 0 && full == 1);
    bool charger_none = (chg == 1 && full == 1);
    //bool unknown_state  = (chg == 0 && full == 0);   // should not happen

    // If device is connected to charger (charging OR full)
    if (charging || charge_full) {
        dev_charging_flag = true;
        treatment_led(duty_zero);
        Set_Color("red");    // Device is charging or just connected
    }

    if(dev_charging_flag) {

        chg  = HAL_GPIO_ReadPin(GPIOB, BAT_CHG_Pin);
        full = HAL_GPIO_ReadPin(GPIOB, BAT_FULL_Pin);

        charging     = (chg == 1 && full == 0);
        charge_full  = (chg == 0 && full == 1);
        charger_none = (chg == 1 && full == 1);
        //unknown_state  = (chg == 0 && full == 0);

        if (charge_full) {
            Set_Color("green");   // Battery full
        }

        if (charger_none) {
            shutdown_routine();   // Charger removed → shutdown
        }



        HAL_Delay(100);
    }
}




/**
 * @brief This function is triggered when self_diagnosis command is received from the BLE.
 *
 * 	Checks the treatment LEDs by toggling them ON and OFF.
 *  Treatment Session duration does not resets.
 */
void self_diagnosis(){

			treatment_led(duty_zero);
			HAL_Delay(led_delay);
			treatment_led(default_duty);
			HAL_Delay(led_delay);
			treatment_led(duty_zero);
			//sendResponse(seq, 0x05, 0x02, 1);
}










/*
 * This function Checks the clinical_mode_flag state from the flash memory.
 * 	if clinical_mode_flag is true then starts the treatment session.
 * 	 else makes the session_end_flag true, then later this flag state is checked in the session_state();
 *
 */
void clinical_mode_check(){
	clinical_mode_flag = Flash_ReadFlag();


	  if(clinical_mode_flag){

		  touchlocked_flag = 1;

	  }else touchlocked_flag = 0;
}











