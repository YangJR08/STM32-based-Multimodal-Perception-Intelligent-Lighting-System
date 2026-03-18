#ifndef APP_RELAY_H
#define APP_RELAY_H

#include <stdint.h>

void Relay_Init(void);
void Relay_Set(uint8_t on);
uint8_t Relay_GetState(void);

#endif