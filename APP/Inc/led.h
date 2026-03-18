#ifndef APP_LED_H
#define APP_LED_H

#include <stdint.h>

void LED_Init(void);
void LED_SetBrightness(uint8_t percent);
uint8_t LED_GetBrightness(void);

#endif