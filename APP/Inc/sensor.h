#ifndef APP_SENSOR_H
#define APP_SENSOR_H

#include <stdint.h>

void Sensor_Init(void);
uint16_t Sensor_ReadAmbientAdc(void);
uint8_t Sensor_ReadMotion(void);
float Sensor_ReadDistanceCm(void);

#endif