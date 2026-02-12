#include "eeprom.h"
#include <string.h>


static uint16_t EEPROM_GetDeviceAddress(uint16_t addr)
{
    if (addr < 256)
        return 0xA0;   // block 0
    else
        return 0xA2;   // block 1
}


// Write 1 byte
HAL_StatusTypeDef EEPROM_WriteByte(uint16_t addr, uint8_t data)
{
    uint8_t devAddr = EEPROM_GetDeviceAddress(addr);
    uint8_t wordAddr = addr & 0xFF;

    uint8_t buf[2];
    buf[0] = wordAddr;
    buf[1] = data;

    HAL_StatusTypeDef status =
        HAL_I2C_Master_Transmit(&hi2c1, devAddr, buf, 2, EEPROM_TIMEOUT);

    HAL_Delay(5);
    return status;
}


// Read 1 byte
uint8_t EEPROM_ReadByte(uint16_t addr)
{
    uint8_t devAddr = EEPROM_GetDeviceAddress(addr);
    uint8_t wordAddr = addr & 0xFF;
    uint8_t data;

    HAL_I2C_Master_Transmit(&hi2c1, devAddr, &wordAddr, 1, EEPROM_TIMEOUT);
    HAL_I2C_Master_Receive(&hi2c1, devAddr, &data, 1, EEPROM_TIMEOUT);

    return data;
}


// Multi-byte write WITH page handling
HAL_StatusTypeDef EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_OK;

    while (len > 0)
    {
        uint8_t devAddr = EEPROM_GetDeviceAddress(addr);
        uint8_t wordAddr = addr & 0xFF;

        uint8_t pageRemaining = EEPROM_PAGE_SIZE - (wordAddr % EEPROM_PAGE_SIZE);
        uint8_t chunk = (len < pageRemaining) ? len : pageRemaining;

        uint8_t buf[EEPROM_PAGE_SIZE + 1];
        buf[0] = wordAddr;
        memcpy(&buf[1], data, chunk);

        status = HAL_I2C_Master_Transmit(&hi2c1, devAddr, buf, chunk + 1, EEPROM_TIMEOUT);
        if (status != HAL_OK) return status;

        HAL_Delay(5);

        addr += chunk;
        data += chunk;
        len  -= chunk;
    }

    return HAL_OK;
}


// Multi-byte read
HAL_StatusTypeDef EEPROM_Read(uint16_t addr, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_OK;

    while (len > 0)
    {
        uint8_t devAddr = EEPROM_GetDeviceAddress(addr);
        uint8_t wordAddr = addr & 0xFF;

        uint16_t chunk = ((addr | 0xFF) - addr) + 1; // until block boundary
        if (chunk > len) chunk = len;

        HAL_I2C_Master_Transmit(&hi2c1, devAddr, &wordAddr, 1, EEPROM_TIMEOUT);
        status = HAL_I2C_Master_Receive(&hi2c1, devAddr, data, chunk, EEPROM_TIMEOUT);
        if (status != HAL_OK) return status;

        addr += chunk;
        data += chunk;
        len  -= chunk;
    }

    return HAL_OK;
}

