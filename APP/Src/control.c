#include "control.h"

#include <stdio.h>

#include "bluetooth.h"
#include "led.h"
#include "relay.h"
#include "sensor.h"
#include "usart.h"

#define SENSOR_SAMPLE_PERIOD_MS    200U
#define DISTANCE_SAMPLE_PERIOD_MS  300U
#define STATUS_REPORT_PERIOD_MS   1000U
#define NO_PERSON_OFF_DELAY_MS   20000U

typedef enum {
  CONTROL_MODE_AUTO = 0,
  CONTROL_MODE_FORCE_ON,
  CONTROL_MODE_FORCE_OFF,
  CONTROL_MODE_MANUAL_BRIGHT
} ControlMode;

static ControlMode s_mode = CONTROL_MODE_AUTO;
static uint8_t s_manual_brightness = 80U;

static uint32_t s_last_presence_tick = 0U;
static uint32_t s_last_sensor_tick = 0U;
static uint32_t s_last_distance_tick = 0U;
static uint32_t s_last_report_tick = 0U;

static uint16_t s_ambient_adc = 0U;
static uint8_t s_motion = 0U;
static float s_distance_cm = -1.0f;

static uint8_t MapAmbientToBrightness(uint16_t adc) {
  uint32_t duty = (4095U - adc) * 100U / 4095U;
  if (duty > 100U) {
    duty = 100U;
  }
  return (uint8_t)duty;
}

static void ApplyLightState(uint8_t on, uint8_t brightness) {
  if (on) {
    Relay_Set(1U);
    LED_SetBrightness(brightness);
  } else {
    LED_SetBrightness(0U);
    Relay_Set(0U);
  }
}

static void HandleBluetoothCommand(void) {
  BluetoothCommand cmd;

  while (Bluetooth_GetCommand(&cmd)) {
    switch (cmd.type) {
      case BT_CMD_LED_ON:
        s_mode = CONTROL_MODE_FORCE_ON;
        break;
      case BT_CMD_LED_OFF:
        s_mode = CONTROL_MODE_FORCE_OFF;
        break;
      case BT_CMD_BRIGHT:
        s_mode = CONTROL_MODE_MANUAL_BRIGHT;
        s_manual_brightness = cmd.value;
        break;
      case BT_CMD_NONE:
      default:
        break;
    }
  }
}

void Control_Init(void) {
  LED_Init();
  Relay_Init();
  Sensor_Init();
  Bluetooth_Init(&huart2);

  s_mode = CONTROL_MODE_AUTO;
  s_last_presence_tick = HAL_GetTick();
  s_last_sensor_tick = 0U;
  s_last_distance_tick = 0U;
  s_last_report_tick = 0U;
}

void Control_Update(void) {
  uint32_t now = HAL_GetTick();
  uint8_t should_on = 0U;
  uint8_t brightness = 0U;
  uint8_t aux_presence = 0U;

  Bluetooth_Process();
  HandleBluetoothCommand();

  if ((now - s_last_sensor_tick) >= SENSOR_SAMPLE_PERIOD_MS) {
    s_last_sensor_tick = now;
    s_ambient_adc = Sensor_ReadAmbientAdc();
    s_motion = Sensor_ReadMotion();
  }

  if ((now - s_last_distance_tick) >= DISTANCE_SAMPLE_PERIOD_MS) {
    s_last_distance_tick = now;
    s_distance_cm = Sensor_ReadDistanceCm();
  }

  if ((s_distance_cm > 2.0f) && (s_distance_cm < 120.0f)) {
    aux_presence = 1U;
  }

  if ((s_motion != 0U) || (aux_presence != 0U)) {
    s_last_presence_tick = now;
  }

  switch (s_mode) {
    case CONTROL_MODE_FORCE_ON:
      should_on = 1U;
      brightness = 100U;
      break;
    case CONTROL_MODE_FORCE_OFF:
      should_on = 0U;
      brightness = 0U;
      break;
    case CONTROL_MODE_MANUAL_BRIGHT:
      should_on = 1U;
      brightness = s_manual_brightness;
      break;
    case CONTROL_MODE_AUTO:
    default:
      should_on = ((now - s_last_presence_tick) <= NO_PERSON_OFF_DELAY_MS) ? 1U : 0U;
      brightness = MapAmbientToBrightness(s_ambient_adc);
      break;
  }

  ApplyLightState(should_on, brightness);

  if ((now - s_last_report_tick) >= STATUS_REPORT_PERIOD_MS) {
    char msg[48];
    uint16_t dist_u16 = (s_distance_cm > 0.0f) ? (uint16_t)(s_distance_cm + 0.5f) : 0U;
    s_last_report_tick = now;
    snprintf(msg, sizeof(msg), "DIST_%u ADC_%u BR_%u\r\n", dist_u16, s_ambient_adc, LED_GetBrightness());
    Bluetooth_SendString(msg);
  }
}