/*
 * rgb.c
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */
#include "rgb.h"
#include "string.h"

RGB_State ledState = {0, 0, 0}; // All OFF initially

const ColorMap colorTable[] = {
    {"red",     1, 0, 0},
    {"green",   0, 1, 0},
    {"blue",    0, 0, 1},
    {"cyan",    0, 1, 1},
    {"magenta", 1, 0, 1},
    {"white",   1, 1, 1},
    {"lemon",   1, 1, 0},
    {"off",     0, 0, 0}
};

void Set_Color(const char *colorName)
{
    for (int i = 0; i < 8; i++) {
        if (strcmp(colorName, colorTable[i].name) == 0) {
            Set_RGB(colorTable[i].r, colorTable[i].g, colorTable[i].b);
            return;
        }
    }
}

void Set_RGB(uint8_t r, uint8_t g, uint8_t b)
{
    // Update hardware
    HAL_GPIO_WritePin(GPIOB, led_R_Pin, r ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_G_Pin, g ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_B_Pin, b ? GPIO_PIN_RESET : GPIO_PIN_SET);

    // Update stored state
    ledState.r = r;
    ledState.g = g;
    ledState.b = b;
}

void Restore_RGB(void)
{
    HAL_GPIO_WritePin(GPIOB, led_R_Pin, ledState.r ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_G_Pin, ledState.g ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_B_Pin, ledState.b ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void RGB(uint8_t r, uint8_t g, uint8_t b)
{
    // Update hardware
    HAL_GPIO_WritePin(GPIOB, led_R_Pin, r ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_G_Pin, g ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, led_B_Pin, b ? GPIO_PIN_RESET : GPIO_PIN_SET);


}
