#include "led.h"

#include "main.h"

static uint8_t s_brightness = 0;

static void LED_TIM4_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();

  GPIO_InitStruct.Pin = PB9_MF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PB9_MF_GPIO_Port, &GPIO_InitStruct);

  /* TIM4 CH4 PWM: 72MHz/(71+1)/(999+1) = 1kHz */
  TIM4->PSC = 71U;
  TIM4->ARR = 999U;
  TIM4->CCR4 = 0U;

  TIM4->CCMR2 &= ~(TIM_CCMR2_OC4M | TIM_CCMR2_OC4PE);
  TIM4->CCMR2 |= (6U << TIM_CCMR2_OC4M_Pos);  /* PWM mode 1 */
  TIM4->CCMR2 |= TIM_CCMR2_OC4PE;

  TIM4->CCER &= ~TIM_CCER_CC4P;
  TIM4->CCER |= TIM_CCER_CC4E;
  TIM4->CR1 |= TIM_CR1_ARPE;
  TIM4->EGR = TIM_EGR_UG;
  TIM4->CR1 |= TIM_CR1_CEN;
}

void LED_Init(void) {
  LED_TIM4_Init();
  LED_SetBrightness(0);
}

void LED_SetBrightness(uint8_t percent) {
  uint32_t pulse = 0;
  uint32_t period = TIM4->ARR;

  if (percent > 100U) {
    percent = 100U;
  }

  if (percent >= 100U) {
    pulse = period + 1U;
  } else {
    pulse = ((period + 1U) * percent) / 100U;
  }

  TIM4->CCR4 = pulse;
  s_brightness = percent;
}

uint8_t LED_GetBrightness(void) {
  return s_brightness;
}