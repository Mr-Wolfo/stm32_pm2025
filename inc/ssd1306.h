#ifndef STM32_TEMPLATE_SSD1306_H
#define STM32_TEMPLATE_SSD1306_H

#include "stm32f10x.h"

void display_cmd(uint8_t cmd);
void display_data(uint8_t data);
void display_init(void);
void display_reset(void);
void display_draw_chessboard(void);

#endif //STM32_TEMPLATE_SSD1306_H