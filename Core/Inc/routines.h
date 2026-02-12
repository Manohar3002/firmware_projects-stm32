/*
 * routines.h
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_ROUTINES_H_
#define INC_ROUTINES_H_
#include "stdbool.h"

extern uint32_t session_start_tick;
extern uint32_t session_end_tick;

#define led_delay 500			//This Macro controls the LED color transition duration in milli seconds.
#define duty_zero 0				//This Macro is used for setting the treatment led brightness to 0%.
#define default_duty 65			//This Macro is used for setting the treatment led brightness to 65%.
#define session_duration 1800000//This Macro is used for setting the session duration in milli seconds, 30 min = 1000*60*30.
#define idle_duration 180000	//This Macro is used for setting the device idle duration in milli seconds, 3 min = 1000*60*3.

extern bool start_control_flag; //This flag is used for controlling the Treatment session state.
extern bool end_control_flag;	//This flag is used for controlling the Idle state.
extern bool charge_full;
extern bool man_end_session_flag;

void ping();
void start_routine();
void shutdown_routine();
void app_session_start();
void session_end();
void self_diagnosis();
void check_dev_charging();
void session_state();
void clinical_mode_check();
void app_session_start();
void manual_session_start();
void app_session_end();

#endif /* INC_ROUTINES_H_ */
