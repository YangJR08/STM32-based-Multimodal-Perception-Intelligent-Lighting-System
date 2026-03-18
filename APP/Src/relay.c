#include "relay.h"

#include "main.h"

static uint8_t s_relay_state = 0;

void Relay_Init(void) {
  Relay_Set(0);
}

void Relay_Set(uint8_t on) {
  s_relay_state = (on != 0U) ? 1U : 0U;
  HAL_GPIO_WritePin(PB0_RD_GPIO_Port, PB0_RD_Pin,
                    s_relay_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t Relay_GetState(void) {
  return s_relay_state;
}