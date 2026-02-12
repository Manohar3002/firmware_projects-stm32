/*
 * ble_protocol.h
 *
 *  Created on: Oct 29, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_BLE_PROTOCOL_H_
#define INC_BLE_PROTOCOL_H_


#include <stdbool.h>
#include "pwm.h"
#include "rgb.h"
#include "routines.h"

extern bool Start_session_flag;
extern bool End_session_flag;
extern bool clinical_mode_flag;
extern bool session_active_flag;
extern bool session_end_flag;
extern bool byte_buf_overflow_flag;
extern bool frame_buf_overflow_flag;
extern bool dev_charging_flag;
extern bool end_pilot_missing_flag;
extern bool touchlocked_flag;
extern uint8_t seq;

//Command codes:
#define	bat_level   	   				0x05
#define	Start_session					0x06
#define	End_session						0x07
#define	Reset_device					0x08
#define	Ping_device						0x09
#define	Set_Red_dosage					0x0a 	//(Associated data is 1 byte dose level)
#define	Get_Red_dosage					0x0b
#define	Set_IR_dosage					0x0c	//(Associated data is 1 byte dose level)
#define	Get_IR_dosage					0x0d
#define	Get_device_state				0x0e	//(Associated data is 1 byte device state)
#define	Run_Self_diag					0x0f
#define	SHTDWN_Device					0x10
#define	Set_session_time				0x13 	//(Associated 2 byte seconds data)
#define	Silent_Ping						0x14
#define	Read_HW_FW_Version				0x15
#define	Get_Session_Time				0x16
#define	GET_Device_unique_ID			0x17
#define	Get_Statistics					0x18
#define	Reset_temp_statistics			0x19
#define	GET_MBR							0x1A
#define	Update_default_red_dose			0x1B
#define	Update_default_IR_dose			0x1C
#define	Update_name						0x7E 	//(Associated name data)
#define	Set_reset_clinical_mode			0x1D
#define	Reset_MBR						0x1E
#define	ACK_new_MBR						0x1F
#define	Send_Initial_battery_level 		0x20
#define Ble_reset						0x21
#define Reset_perm_statistics			0x22
#define Touch_lock						0x26
#define Touch_unlock					0x27




//PACK Codes
#define Session_completed 	 			0x01
#define	Idle_timeout_shutdown			0x02
#define Low_Battery_shutdown			0x03
#define Heart_beat						0x04

//NACK Error codes:
#define	Unknown_cmd_code				0x01
#define	Param_out_of_bounds				0x02		//Parameters range out of bounds
#define	hardware_error					0x03		//Command failed due to hardware error
#define	Device_busy						0x04
#define	Checksum_error					0x05
#define	Missing_End_pilot				0x06
#define	Cmd_compliment_mismatch			0x07
#define	Command_pool_full				0x08
#define	Session_ended_L_Bat				0x09		//Session ended due to Low Battery
#define	MBR_sign_failed					0x0a		//MBR signature failed in eeprom
#define byte_buf_overflow 				0x0b	//frame length is more than 64 bytes
#define frame_buffer_overflow			0x0c	//frame buffer overflow
#define C_mode_data_mismatch   			0x0d	//invalid Clinical mode value



//Response codes:
#define PACK							0x05 	//(Associated response data if applicable)
#define NACK							0x06 	//(Associated Error codes and data if applicable in response data field)


#define MAX_FRAME_SIZE 64
#define MAX_FRAMES       20     						// number of frames to buffer

#define START_PILOT 0x24  // '$'
#define END_PILOT   0x23  // '#'

extern volatile uint8_t byte_count;
extern uint8_t  rx_byte;  // temporary storage for each interrupt
extern uint16_t manual_full_count;
extern uint16_t app_full_count;
extern uint16_t manual_perm_count;
extern uint16_t app_perm_count;
extern uint16_t man_part_count;
extern uint16_t app_part_count;


typedef struct {
    uint8_t data[MAX_FRAME_SIZE];
    uint8_t len;
} Frame_t;

extern Frame_t frameQueue[MAX_FRAMES];
extern UART_HandleTypeDef huart2;  // BM71 UART



// Function prototypes
uint16_t crc16_modbus(uint8_t *data, uint16_t len);
void ble_check_for_new_command(void);
void sendResponse(uint8_t seq, uint8_t respCode, uint8_t *data, uint8_t dataLen);
void processFrame(uint8_t *frame, uint8_t len);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void process_UART();
void frame_count_check();

#endif /* INC_BLE_PROTOCOL_H_ */
