#ifndef APP_BLUETOOTH_H
#define APP_BLUETOOTH_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum {
  BT_CMD_NONE = 0,
  BT_CMD_LED_ON,
  BT_CMD_LED_OFF,
  BT_CMD_BRIGHT
} BluetoothCommandType;

typedef struct {
  BluetoothCommandType type;
  uint8_t value;
} BluetoothCommand;

void Bluetooth_Init(UART_HandleTypeDef *huart);
void Bluetooth_Process(void);
uint8_t Bluetooth_GetCommand(BluetoothCommand *cmd);
void Bluetooth_SendString(const char *str);

#endif