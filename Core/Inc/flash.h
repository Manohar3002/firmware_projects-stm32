/*
 * flash.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ManoharThokala
 */

#ifndef FLASH_H
#define FLASH_H

#include "stm32g0xx_hal.h"   // or your MCU family header

#define FLAG_FLASH_ADDR  0x08007800


void Flash_Init(void);
void Flash_WriteFlag(bool flag);
bool Flash_ReadFlag();
void Flash_ErasePage(uint32_t address);

#endif
