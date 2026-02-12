/*
 * adc.c
 *
 *  Created on: Oct 29, 2025
 *      Author: ManoharThokala
 */
#include "adc.h"
#include "main.h"
#include "ble_protocol.h"
#include <stdio.h>
#include <string.h>


/**
 * @brief  Reads battery voltage using ADC and converts it to a percentage.
 *
 * @note   Expects a voltage divider with R1 = R2 (divide by 2).
 *         Battery voltage range considered: 3.2V (0%) to 4.2V (100%).
 *
 * @note   We get raw adc value as [(measured input voltage)/(adc Vref)]*4095
 * 		   So to get the actual voltage seen by the adc, we use Vadc = ((float)raw / 4095.0f) * 3.3f
 * 			  then this value gets multiplied by 2 (VDR) to get battery value.
 *
 * @return Battery level in percentage (0–100).
 */
uint8_t Get_Battery_Percentage(void)
{
	 uint16_t raw;			// Variable to store raw ADC conversion result (0–4095 for 12-bit ADC)

	 // Perform ADC Conversion
	     HAL_ADC_Start(&hadc1);                              // Start ADC conversion
	     HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);   // Wait until conversion completes
	     raw = HAL_ADC_GetValue(&hadc1);                     // Read converted ADC value
	     HAL_ADC_Stop(&hadc1);                               // Stop ADC to save power


    //Convert ADC reading to measured voltage (3.3V reference)
    float Vadc = ((float)raw / 4095.0f) * 2.3f;

    //voltage divider ratio (R1=R2) so multiply by 2
    float Vbat = Vadc * 2.0f;

    // Compute Vadc in millivolts: (raw/4095)*2300 mV
        uint32_t vadc_mv = (raw * 2300UL) / 4095UL;

        // Battery voltage (x2 for divider)
        uint32_t vbat_mv = vadc_mv * 2;

        char msg[32];
        snprintf(msg, sizeof(msg), "Vbat = %lu mV\r\n", vbat_mv);
        HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    //Linear mapping between 3.2V and 4.2V for battery percentage calculation.
    float Vmin = 3.2f;
    float Vmax = 4.2f;
    float percent = ((Vbat - Vmin) / (Vmax - Vmin)) * 100.0f;

    // Clamp between 0–100
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
//    uint8_t test = 25;
//   return test;
    return (uint8_t)percent;

   // return (uint16_t)raw;

}

/*
uint16_t Get_LDR_Value(void)
{
	uint16_t raw = Read_ADC_Channel(ADC_CHANNEL_6);  // Use helper function
	HAL_ADC_Start(&hadc1);
	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	    raw = HAL_ADC_GetValue(&hadc1);
	    HAL_ADC_Stop(&hadc1);
    return;
}
*/

