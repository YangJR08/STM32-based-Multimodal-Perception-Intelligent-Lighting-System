#include "ultrasonic.h"

#include "main.h"

#define ULTRA_READ_TIMEOUT_MS 50U

typedef enum {
  ULTRA_CAPTURE_WAIT_RISING = 0,
  ULTRA_CAPTURE_WAIT_FALLING
} UltraCaptureState;

static volatile UltraCaptureState s_capture_state = ULTRA_CAPTURE_WAIT_RISING;
static volatile uint16_t s_rise_ticks = 0U;
static volatile uint16_t s_fall_ticks = 0U;
static volatile uint8_t s_measure_done = 0U;

static void Ultrasonic_TimerInit(void) {
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();

  /* TIM2 full remap: CH4 maps to PB11 */
  AFIO->MAPR &= ~AFIO_MAPR_TIM2_REMAP;
  AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;

  TIM2->PSC = 71U;      /* 72MHz / (71 + 1) = 1MHz, 1 tick = 1us */
  TIM2->ARR = 0xFFFFU;
  TIM2->CNT = 0U;

  TIM2->CCMR2 &= ~(TIM_CCMR2_CC4S | TIM_CCMR2_IC4F | TIM_CCMR2_IC4PSC);
  TIM2->CCMR2 |= TIM_CCMR2_CC4S_0; /* CC4 mapped to TI4 */

  TIM2->CCER &= ~TIM_CCER_CC4P;
  TIM2->CCER |= TIM_CCER_CC4E;

  TIM2->DIER |= TIM_DIER_CC4IE;
  TIM2->SR = 0U;
  TIM2->CR1 |= TIM_CR1_CEN;

  HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

static void Ultrasonic_DelayUs(uint16_t us) {
  uint16_t start = (uint16_t)TIM2->CNT;
  while ((uint16_t)(TIM2->CNT - start) < us) {
  }
}

static void Ultrasonic_Trigger(void) {
  s_capture_state = ULTRA_CAPTURE_WAIT_RISING;
  s_measure_done = 0U;

  TIM2->CCER &= ~TIM_CCER_CC4P;  /* first capture rising edge */
  TIM2->SR &= ~TIM_SR_CC4IF;

  HAL_GPIO_WritePin(PA6_TRIG_GPIO_Port, PA6_TRIG_Pin, GPIO_PIN_RESET);
  Ultrasonic_DelayUs(2U);
  HAL_GPIO_WritePin(PA6_TRIG_GPIO_Port, PA6_TRIG_Pin, GPIO_PIN_SET);
  Ultrasonic_DelayUs(10U);
  HAL_GPIO_WritePin(PA6_TRIG_GPIO_Port, PA6_TRIG_Pin, GPIO_PIN_RESET);
}

void Ultrasonic_Init(void) {
  Ultrasonic_TimerInit();
  HAL_GPIO_WritePin(PA6_TRIG_GPIO_Port, PA6_TRIG_Pin, GPIO_PIN_RESET);
}

float Ultrasonic_ReadDistanceCm(void) {
  uint32_t start_tick;
  uint16_t pulse_us;

  Ultrasonic_Trigger();
  start_tick = HAL_GetTick();
  while (s_measure_done == 0U) {
    if ((HAL_GetTick() - start_tick) > ULTRA_READ_TIMEOUT_MS) {
      return -1.0f;
    }
  }

  pulse_us = (uint16_t)(s_fall_ticks - s_rise_ticks);
  return (float)pulse_us / 58.0f;
}

void Ultrasonic_TIM_IRQHandler(void) {
  if ((TIM2->SR & TIM_SR_CC4IF) == 0U) {
    return;
  }

  if (s_capture_state == ULTRA_CAPTURE_WAIT_RISING) {
    s_rise_ticks = (uint16_t)TIM2->CCR4;
    s_capture_state = ULTRA_CAPTURE_WAIT_FALLING;
    TIM2->CCER |= TIM_CCER_CC4P;  /* switch to falling edge */
  } else {
    s_fall_ticks = (uint16_t)TIM2->CCR4;
    s_capture_state = ULTRA_CAPTURE_WAIT_RISING;
    s_measure_done = 1U;
    TIM2->CCER &= ~TIM_CCER_CC4P; /* restore rising edge */
  }

  TIM2->SR &= ~TIM_SR_CC4IF;
}