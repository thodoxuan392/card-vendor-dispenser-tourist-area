#include <Device/genericio.h>

#include <Hal/gpio.h>

static GPIO_info_t GENERICIO_table[] = {
    [GENERICIO_ID_1] = {GPIOD,
                        {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
    [GENERICIO_ID_2] = {GPIOD,
                        {GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
    [GENERICIO_ID_3] = {GPIOD,
                        {GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
    [GENERICIO_ID_4] = {GPIOD,
                        {GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
    [GENERICIO_ID_5] = {GPIOD,
                        {GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
    [GENERICIO_ID_6] = {GPIOD,
                        {GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP,
                         GPIO_SPEED_FREQ_LOW}},
};

void GENERICIO_init(void) {
  for (size_t id = 0; id < GENERICIO_ID_MAX; id++) {
    HAL_GPIO_Init(GENERICIO_table[id].port, &GENERICIO_table[id].init_info);
  }
}

void GENERICIO_set(GENERICIO_Id id, bool enable) {
  HAL_GPIO_WritePin(GENERICIO_table[id].port, GENERICIO_table[id].init_info.Pin,
                    enable);
}