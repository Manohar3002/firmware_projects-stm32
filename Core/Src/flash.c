/*
 * flash.c
 *
 *  Created on: Nov 1, 2025
 *      Author: ManoharThokala
 */


#include <stdbool.h>
#include "flash.h"



void Flash_Init(void)
{
    HAL_FLASH_Unlock();
    HAL_FLASH_Lock();
}

void Flash_ErasePage(uint32_t address)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t PageError = 0;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    erase.NbPages = 1;

    HAL_FLASHEx_Erase(&erase, &PageError);
}


void Flash_WriteFlag(bool flag)
{
    HAL_FLASH_Unlock();

    // just erase the page — no return check
    Flash_ErasePage(FLAG_FLASH_ADDR);

    uint64_t data64 = flag ? 1 : 0;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLAG_FLASH_ADDR, data64);

    HAL_FLASH_Lock();
}


bool Flash_ReadFlag()
{
    uint8_t flag = *(uint8_t *)FLAG_FLASH_ADDR;
    return (flag == 1);  // return true if 1, false otherwise
}

void Flash_Write(uint32_t address, uint8_t *data12, bool flag)
{
    uint64_t word1 = 0;
    uint64_t word2 = 0;

    HAL_FLASH_Unlock();

    // Erase the page containing this address
    Flash_ErasePage(address);

    /* First double-word: data[0..7] */
    memcpy(&word1, data12, 8);

    /* Second double-word */
    memcpy(&word2, data12 + 8, 4);          // data[8..11]
    ((uint8_t*)&word2)[4] = flag ? 1 : 0;   // separate flag (byte 12)

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address,     word1);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + 8, word2);

    HAL_FLASH_Lock();
}

