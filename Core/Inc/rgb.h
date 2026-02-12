/*
 * rgb.h
 *
 *  Created on: Oct 28, 2025
 *      Author: ManoharThokala
 */

#ifndef INC_RGB_H_
#define INC_RGB_H_

#include "stdint.h"
#include "pctrl.h"




typedef struct {
    const char *name;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorMap;


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_State;

extern RGB_State ledState;

void Set_RGB(uint8_t r,uint8_t g,uint8_t b);
void Restore_RGB(void);
void Set_Color(const char *colorName);
void RGB(uint8_t r, uint8_t g, uint8_t b);

#endif /* INC_RGB_H_ */
