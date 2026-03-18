#include "sensor.h"

#include "adc.h"
#include "main.h"
#include "ultrasonic.h"

void Sensor_Init(void) {
  HAL_ADCEx_Calibration_Start(&hadc1);
  Ultrasonic_Init();
}

uint16_t Sensor_ReadAmbientAdc(void) {
  uint16_t value = 0;

  if (HAL_ADC_Start(&hadc1) == HAL_OK) {
    if (HAL_ADC_PollForConversion(&hadc1, 10U) == HAL_OK) {
      value = (uint16_t)HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
  }

  return value;
}

uint8_t Sensor_ReadMotion(void) {
  return (HAL_GPIO_ReadPin(PB1_INF_GPIO_Port, PB1_INF_Pin) == GPIO_PIN_SET) ? 1U : 0U;
}

float Sensor_ReadDistanceCm(void) {
  return Ultrasonic_ReadDistanceCm();
}