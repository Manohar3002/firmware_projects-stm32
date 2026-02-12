/*
 * ble_protocol.c
 *
 *  Created on: Oct 29, 2025
 *      Author: ManoharThokala
 */

#include "string.h"
#include "stdbool.h"
#include "adc.h"
#include "pwm.h"
#include "rgb.h"
#include "routines.h"
#include "flash.h"
#include "ble_protocol.h"
#include "cmsis_gcc.h"


//Variables
uint8_t rx_byte = 0;
volatile uint16_t uart_rx_head = 0;					// Head index used for controlling new byte write position in the byte buffer.
uint16_t uart_rx_tail = 0;								// tail index used for accessing bytes present in the byte buffer.
volatile uint8_t byte_buffer[MAX_FRAME_SIZE];		// Circular buffer for storing received bytes through uart interrupt. buffer size -> 64
volatile uint8_t byte_count = 0;						// Byte counter, stores information about new bytes ready to process in the byte buffer.
uint8_t frame[MAX_FRAME_SIZE];						// An array used for building a command.
uint8_t frame_index;								// An index used for building a new frame[].
Frame_t frameQueue[MAX_FRAMES];						// Circular buffer for storing the executable commands.
uint8_t frameHead = 0;								// Head index used for controlling new command write position in the frameQueue[].
uint8_t frameTail = 0;								// Tail index used for accessing available commands from the frameQueue[] while executing.
uint8_t frameCount = 0;								// Frame counter, stores information about no. of new commands ready to execute.
uint8_t seq;										// Global variable stores Command sequence number, later used while sending response.
uint16_t manual_full_count;
uint16_t app_full_count;
uint16_t manual_perm_count;
uint16_t app_perm_count;
uint16_t man_part_count;
uint16_t app_part_count;

//trigger flags
 bool Start_session_flag;
 bool End_session_flag;
 bool SHTDWN_Device_flag;
 bool clinical_mode_flag;
 bool session_active_flag;
 bool session_end_flag;
 bool rx_buf_overflow_flag;
 bool dev_charging_flag;
 bool frame_buf_overflow_flag;
 bool byte_buf_overflow_flag = false;
 bool touchlocked_flag;



/**
 * @brief UART interrupt callback
 *
 * 	This is an ISR triggered when a new byte arrives through uart2
 *			Pushes the received byte into ring buffer uart_rx_buffer[] of size 64.
 *			increments byte_count, which is checked in the process_UART() for processing the bytes
 *			Re-enables UART interrupt for next byte.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {

    	uint8_t index = uart_rx_head;
       	byte_buffer[index] = rx_byte;
       	byte_count++;

       	uart_rx_head = (uart_rx_head+1) % MAX_FRAME_SIZE;

        // Re-enable UART interrupt for next byte
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

/**
 * @brief Builds a command frame from the received bytes through uart.
 *
 * 	Checks for byte count for available bytes in the uart rx buffer.
 * 	Starts building the frame when available byte is Start Pilot.
 *  Continues building the frame byte by byte until available byte is End Pilot and frame index is not more than Max frame size.
 *  if end pilot is missing at the expected position in the frame, notifies user with a NACK Response.
 *  Copies built frame into Frame queue if frame count is less than Max frames.
 */
void process_UART(){
			while(byte_count>0){
							  // Read next byte from circular UART buffer
							  uint8_t index = uart_rx_tail;
					          uint8_t byte = byte_buffer[index];
					          // Increment tail pointer and wrap around when reached end of the buffer.
					          uart_rx_tail = (uart_rx_tail + 1) % MAX_FRAME_SIZE;

					          /*
					           *   CHECK FOR START PILOT
					           *   If this byte is the START_PILOT, begin a new frame.
					           *   Reset frame index and store the first byte.
					           */
					          if (byte == START_PILOT){
					        	  	  	  	 frame_index = 0;
					        	  	  	  	 frame[frame_index++] = byte; // Stores Start pilot and then increments the frame index.
					          	  	  	  	 }

					           /*
					            *    STORE DATA BYTES INTO FRAME BUFFER
					            *    Only store data if a frame is already in progress.
					            */
					          else if(frame_index>0){
					          	        	  if(frame_index >= MAX_FRAME_SIZE){ 				// Prevent writing beyond frame buffer size
					          	        	  	        	  frame_index=0;					// reset frame index due to overflow
					          	        	  	        	  byte_buf_overflow_flag = true;
					          	        	  	          	  }
					          	        	  // Store received byte into frame buffer
					          	        	  frame[frame_index++] = byte;						//Stores the next byte and incremets the frame index.
					          	          	  }

					          /*
					           *   CHECK IF WE RECEIVED COMPLETE FRAME
					           *   Expected frame length = header(8 bytes) + data length
					           *   frame[4] holds data length (from protocol format)
					           */

					          uint8_t expected_len = 8 + frame[4];   // header + data payload

					          if(frame_index == expected_len){


					        	  	   //CHECK FOR END PILOT
					        	  	   //Last byte must be END_PILOT
					        	  	  if (byte == END_PILOT){

					        	  	        	  if (frameCount < MAX_FRAMES) {
					        	  	        	                  frameQueue[frameHead].len = frame_index;
					        	  	        	                  // Copy complete frame into queue buffer
					        	  	        	                  memcpy((uint8_t*)frameQueue[frameHead].data, (const uint8_t*)frame, frame_index);
					        	  	        	                  frameHead = (frameHead + 1) % MAX_FRAMES;
					        	  	        	                  frameCount++;

					        	  	        	   }else frame_buf_overflow_flag = true; // frame Queue buffer full → cannot store


					        	  	        	  frame_index = 0;			//Reset frame index for next frame.

					        	  	  }else {  								// Handles Missing END_PILOT error.
					        	  	       uint8_t err = Missing_End_pilot;
					        	  	       sendResponse(seq, NACK, &err, 1);// notify host
					        	  	       frame_index = 0;

					        	  	  }

					          	}
					          //safely decrement byte count after processing a byte.
					          __disable_irq(); //Disables interrupt requests for Preventing Race condition from Uart interrupts.
					          byte_count--;
					          __enable_irq(); //Re-enabling intterrupt requests after decrementing byte count for next byte.
			}

}

/**
 * @brief Computes CRC-16 (Modbus variant) for a given data buffer.
 *
 * Polynomial used : 0xA001  (reverse of 0x8005)
 * Initial crc value   : 0xFFFF
 * CRC order        : LSB first (least-significant bit processed first)
 */
uint16_t crc16_modbus(uint8_t *data, uint16_t len)
{
    // CRC initialization value required by Modbus protocol
    uint16_t crc = 0xFFFF;

    // Process each byte in the Payload
    for (uint16_t pos = 0; pos < len; pos++) {

        // XOR current byte into low-order CRC byte
        crc ^= (uint16_t)data[pos];

        // Process 8 bits (each bit of the byte)
        for (uint8_t i = 0; i < 8; i++) {

            // Check if LSB is 1 (LSB-first CRC)
            if (crc & 0x0001) {
                // Shift right and apply polynomial
                crc = (crc >> 1) ^ 0xA001;
            } else {
                // Shift right without XOR
                crc >>= 1;
            }
        }
    }

    // Final CRC result (LSB first, as required by Modbus)
    return crc;
}

/**
 * @brief Checks for pending frames in the queue and processes them.
 *
 * This function is called from the main loop.
 * It performs the following tasks:
 *  - Verifies if any frames are available to process.
 *  - If a frame buffer overflow occurred earlier, sends a NACK response.
 *  - Retrieves the next frame from the circular queue.
 *  - Updates frame counters and tail pointer.
 *  - Passes the frame to processFrame() for decoding and handling.
 */
void frame_count_check(){

	if(frameCount > 0) // If at least one frame is available in the queue
		  	  {
		    	// If an overflow was detected earlier, notify host with NACK
			  if(frame_buf_overflow_flag) {
			  	      		  uint8_t err = frame_buffer_overflow;
			  	      	  	  sendResponse(seq, NACK, &err, 1);
			  	      	  }

			  uint8_t index = frameTail;  				// Get index of the next frame to process
		      frameTail = (frameTail + 1) % MAX_FRAMES; // Advance tail pointer to next frame (circular buffer wrap-around)
		      	      frameCount--;						// Decrement the number of pending frames

		      processFrame((uint8_t *)frameQueue[index].data, frameQueue[index].len);	// Process the frame data at the retrieved index
	}

}


/**
 * @brief Processes a received frame after extraction from the frame queue.
 *
 * The function performs the following operations:
 *  - Validates minimum frame length.
 *  - Extracts header fields (seq, cmd, cmd complement, data, etc.)
 *  - Checks command complement validity.
 *  - Verifies CRC correctness for data integrity.
 *  - Executes the command if valid and sends appropriate response.
 *
 * @param frame Pointer to the received frame buffer.
 * @param len   Length of the received frame.
 */
void processFrame(uint8_t *frame, uint8_t len) {
	// Frame must contain header + CRC; reject if too short
    				if (len < 8) return;

    // Extract header fields from the frame
    seq  = frame[1];					// Sequence number
    uint8_t cmd  = frame[2];			// Command code
    uint8_t cmdC = frame[3];			// Command complement
    uint8_t dataLen = frame[4];			// Length of the payload
    uint8_t *data = &frame[5];			// Pointer to data section


    // Validate command compliment
    if ((uint8_t)~cmd != cmdC) {
        uint8_t err = Cmd_compliment_mismatch;
        sendResponse(seq, NACK, &err, 1);
        return;
    }

    // Compute and validate CRC
    // CRC16 is calculated on: frame[1] to frame[4 + dataLen]

    uint16_t crc_calc = crc16_modbus(&frame[1], 4 + dataLen);
    uint16_t crc_recv = frame[5 + dataLen] | (frame[6 + dataLen] << 8);

    // CRC mismatch → invalid frame
    if (crc_calc != crc_recv) {
        uint8_t err = Checksum_error;
        sendResponse(seq, NACK, &err, 1);
        return;
    }

    if(dev_charging_flag){
    	switch (cmd) {


            case bat_level: {
            	uint8_t percent = Get_Battery_Percentage();
            	sendResponse(seq, PACK, &percent, 1);
                break;
            }
            case Ping_device: {
                    	ping();
                    	sendResponse(seq, PACK, 0x0, 1);
                        break;
                    }
            case Get_device_state: {

                    	uint8_t status_code = 0x13;
                    	sendResponse(seq, PACK, &status_code, 1);

                        break;
                    }

    	}
    }

    if(!dev_charging_flag){
    // Command valid, execute
    switch (cmd) {


        case bat_level: {
        	uint8_t percent = Get_Battery_Percentage();
        	sendResponse(seq, PACK, &percent, 1);
            break;
        }

        case Start_session:{
        	app_part_count++;
        	app_session_start();
            break;
        }

        case End_session: {
        	app_session_end();
            break;
        }

        case Reset_device: {

            break;
        }

        case Ping_device: {
        	ping();
        	sendResponse(seq, PACK, 0x0, 1);
            break;
        }

        case Set_Red_dosage: {
        	duty = data[0];
        	if(duty>0 && duty<101){
        		treatment_led(duty);
        		sendResponse(seq, PACK, 0x00, 1);
        	}

        	   else{
        	       uint8_t err = Param_out_of_bounds;
        	       sendResponse(seq, NACK, &err, 1);
        	   }

            break;
        }

        case Get_Red_dosage: {
        	duty = data[0];
        	if(duty>0 && duty<101){
        		sendResponse(seq, PACK, &duty, 1);}

        	  else{
        		  uint8_t err = Param_out_of_bounds;
        		  sendResponse(seq, PACK, &err, 1);}
            break;
        }

        case Set_IR_dosage: {

            break;
        }

        case Get_IR_dosage: {

            break;
        }

        case Get_device_state: {
        	uint8_t status_code;
        	if(!end_control_flag){
        		status_code =0x12;
        		sendResponse(seq, PACK, &status_code, 1);
        	}
        	else if(end_control_flag){
        	    	status_code = 0x11;
        	    	sendResponse(seq, PACK, &status_code, 1);
        	}

            break;
        }

        case Run_Self_diag: {
        	self_diagnosis();
            break;
        }

        case SHTDWN_Device: {
        	sendResponse(seq, PACK, 0x0, 1);
        	shutdown_routine();
            break;
        }

        case Set_session_time: {

            break;
        }

        case Silent_Ping: {
        	sendResponse(seq, PACK, 0x0, 1);
            break;
        }

        case Read_HW_FW_Version: {

            break;
        }

        case Get_Session_Time:{

            if (!end_control_flag)
            {
                uint32_t cur_sess_duration = HAL_GetTick() - session_start_tick;   	 // time in ms
                uint32_t remaining_ms = session_duration - cur_sess_duration;
                sendResponse(seq, PACK, (uint8_t*)&remaining_ms, 4);            // send 4 bytes
            }
            else
            {
                uint32_t zero = 0;
                sendResponse(seq, PACK, (uint8_t*)&zero, 4);
            }
            break;
        }


        case GET_Device_unique_ID: {

            break;
        }

        case Get_Statistics: {

                uint8_t stats[12];

                stats[0]  =  manual_full_count & 0xFF;
                stats[1]  = (manual_full_count >> 8) & 0xFF;

                stats[2]  =  app_full_count & 0xFF;
                stats[3]  = (app_full_count >> 8) & 0xFF;

                stats[4]  =  manual_perm_count & 0xFF;
                stats[5]  = (manual_perm_count >> 8) & 0xFF;

                stats[6]  =  app_perm_count & 0xFF;
                stats[7]  = (app_perm_count >> 8) & 0xFF;

                stats[8]  =  man_part_count & 0xFF;
                stats[9]  = (man_part_count >> 8) & 0xFF;

                stats[10] =  app_part_count & 0xFF;
                stats[11] = (app_part_count >> 8) & 0xFF;

                sendResponse(seq, PACK, stats, 12);
                break;
        }


        case Reset_temp_statistics: {
            manual_full_count = 0;
            app_full_count = 0;
            man_part_count = 0;
            app_part_count = 0;

            uint8_t ack = 0x00;   // success
            sendResponse(seq, PACK, &ack, 1);
            break;
        }

        case Reset_perm_statistics: {
        	manual_perm_count = 0;
        	app_perm_count = 0;
            uint8_t ack = 0x00;   // success
            sendResponse(seq, PACK, &ack, 1);
            break;
        }

        case GET_MBR: {

            break;
        }

        case Update_default_red_dose: {
        	treatment_led(65);
        	sendResponse(seq, PACK, 0x00, 1);

            break;
        }

        case Update_default_IR_dose: {

            break;
        }

        case Update_name: {

            break;
        }

        case Set_reset_clinical_mode: {
        	uint8_t mode = data[0];
        	if (mode == 0xaa){
        		clinical_mode_flag = true;
        		sendResponse(seq, PACK, &mode, 1);
        		Flash_WriteFlag(clinical_mode_flag);
        	}
        	if (mode == 0x55){
        	    clinical_mode_flag = false;
        	    touchlocked_flag = 0;
        	    sendResponse(seq, PACK, &mode, 1);
        	    Flash_WriteFlag(clinical_mode_flag);
        	        	}
        	else{
        		uint8_t err = C_mode_data_mismatch;
        		sendResponse(seq, NACK, &err, 1);
        	}
            break;
        }

        case Reset_MBR: {

            break;
        }

        case ACK_new_MBR: {

            break;
        }

        case Send_Initial_battery_level: {

            break;
        }

        case Ble_reset: {
        	HAL_GPIO_WritePin(GPIOC, BT_RST_Pin, GPIO_PIN_RESET);
        	HAL_Delay(100);
        	HAL_GPIO_WritePin(GPIOC, BT_RST_Pin, GPIO_PIN_SET);
        	HAL_Delay(100);
        	break;
        }
        case Touch_lock: {
        	touchlocked_flag = 1;
        	sendResponse(seq, PACK, 0x0, 1);
                    break;
                }
        case Touch_unlock: {
                	touchlocked_flag = 0;
                	sendResponse(seq, PACK, 0x0, 1);
                    break;
                }

        default: {

            uint8_t err = Unknown_cmd_code;
            sendResponse(seq, NACK, &err, 1);
            break;
        }
    }
    }
}

/**
 * @brief Builds and sends a response frame over UART.
 *
 * Frame Format:
 *
 *  | START | SEQ | RESP_CODE | DATA_LEN | DATA... | CRC_L | CRC_H | END |
 *
 *
 * The CRC16(Modbus) is calculated from SEQ to last DATA byte.
 *
 * @param seq       Sequence number received in the request.
 * @param respCode  Response type (NACK or PACK).
 * @param data      Pointer to response payload buffer.
 * @param dataLen   Length of the response payload.
 */
void sendResponse(uint8_t seq, uint8_t respCode, uint8_t *data, uint8_t dataLen) {
    uint8_t frame[32];
    uint16_t pos = 0;

    // Frame header construction
    frame[pos++] = START_PILOT;    // Start byte of frame
    frame[pos++] = seq;            // Sequence number
    frame[pos++] = respCode;       // Response code (NACK or PACK)
    frame[pos++] = dataLen;        // Length of payload

    for (uint8_t i = 0; i < dataLen; i++)
        frame[pos++] = data[i];		// Copy payload bytes

    // Compute CRC16
    // CRC is calculated on:
    //   SEQ, RESP_CODE, DATA_LEN, DATA...
    // i.e., frame[1] to frame[3 + dataLen]
    uint16_t crc = crc16_modbus(&frame[1], 2 + 1 + dataLen);

    // Append CRC (LSB first, MSB next)
    frame[pos++] = crc & 0xFF;
    frame[pos++] = (crc >> 8) & 0xFF;
    frame[pos++] = END_PILOT;		//End Marker

    // Transmit complete frame.
    HAL_UART_Transmit(&huart2, frame, pos, HAL_MAX_DELAY);
}
