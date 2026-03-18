#ifndef APP_ULTRASONIC_H
#define APP_ULTRASONIC_H

void Ultrasonic_Init(void);
float Ultrasonic_ReadDistanceCm(void);
void Ultrasonic_TIM_IRQHandler(void);

#endif