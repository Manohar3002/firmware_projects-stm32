/*
 * eeprom.h
 *
 *  Created on: Nov 19, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#include "stm32g0xx_hal.h"

// Change these based on your EEPROM
#define EEPROM_I2C_ADDR      0xA0     // AT24CS default
#define EEPROM_PAGE_SIZE     16       // AT24CS = 16
#define EEPROM_TIMEOUT       100
#define EEPROM_SIZE_BYTES    512      // AT24CS = 512 bytes

extern I2C_HandleTypeDef hi2c1;       // change if using different I2C

HAL_StatusTypeDef EEPROM_WriteByte(uint16_t addr, uint8_t data);
uint8_t EEPROM_ReadByte(uint16_t addr);

HAL_StatusTypeDef EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t len);
HAL_StatusTypeDef EEPROM_Read(uint16_t addr, uint8_t *data, uint16_t len);

// Optional: safer write (writes only if different)
HAL_StatusTypeDef EEPROM_WriteByte_Safe(uint16_t addr, uint8_t data);

#endif /* INC_EEPROM_H_ */
