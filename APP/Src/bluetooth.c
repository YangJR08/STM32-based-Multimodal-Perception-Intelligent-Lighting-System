#include "bluetooth.h"

#include <stdlib.h>
#include <string.h>

#define BT_RX_LINE_MAX 64U

static UART_HandleTypeDef *s_huart = NULL;
static uint8_t s_rx_byte = 0;
static char s_rx_line[BT_RX_LINE_MAX];
static uint8_t s_rx_index = 0;

static BluetoothCommand s_pending_cmd = {BT_CMD_NONE, 0};
static uint8_t s_cmd_ready = 0;

static void Bluetooth_ParseLine(const char *line) {
  if (strcmp(line, "LED_ON") == 0) {
    s_pending_cmd.type = BT_CMD_LED_ON;
    s_pending_cmd.value = 0U;
    s_cmd_ready = 1U;
    return;
  }

  if (strcmp(line, "LED_OFF") == 0) {
    s_pending_cmd.type = BT_CMD_LED_OFF;
    s_pending_cmd.value = 0U;
    s_cmd_ready = 1U;
    return;
  }

  if (strncmp(line, "BRIGHT_", 7U) == 0) {
    int brightness = atoi(&line[7]);
    if (brightness < 0) {
      brightness = 0;
    }
    if (brightness > 100) {
      brightness = 100;
    }

    s_pending_cmd.type = BT_CMD_BRIGHT;
    s_pending_cmd.value = (uint8_t)brightness;
    s_cmd_ready = 1U;
  }
}

static void Bluetooth_PushByte(uint8_t byte) {
  if (byte == '\r' || byte == '\n') {
    if (s_rx_index > 0U) {
      s_rx_line[s_rx_index] = '\0';
      Bluetooth_ParseLine(s_rx_line);
      s_rx_index = 0U;
    }
    return;
  }

  if (s_rx_index < (BT_RX_LINE_MAX - 1U)) {
    s_rx_line[s_rx_index++] = (char)byte;
  } else {
    s_rx_index = 0U;
  }
}

void Bluetooth_Init(UART_HandleTypeDef *huart) {
  s_huart = huart;
  s_rx_index = 0U;
  s_cmd_ready = 0U;
  HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1U);
}

void Bluetooth_Process(void) {
}

uint8_t Bluetooth_GetCommand(BluetoothCommand *cmd) {
  if ((cmd == NULL) || (s_cmd_ready == 0U)) {
    return 0U;
  }

  *cmd = s_pending_cmd;
  s_pending_cmd.type = BT_CMD_NONE;
  s_pending_cmd.value = 0U;
  s_cmd_ready = 0U;
  return 1U;
}

void Bluetooth_SendString(const char *str) {
  if ((s_huart == NULL) || (str == NULL)) {
    return;
  }

  HAL_UART_Transmit(s_huart, (uint8_t *)str, (uint16_t)strlen(str), 100U);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == s_huart) {
    Bluetooth_PushByte(s_rx_byte);
    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1U);
  }
}