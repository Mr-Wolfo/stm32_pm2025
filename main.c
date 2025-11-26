#include "stm32f10x.h"
#include "inc/gpio.h"
#include "inc/spi.h"
#include "inc/ssd1306.h"

// Пины для управления дисплеем
// DC (Data/Command) -> PA1
// RES (Reset)       -> PA2
// CS (Chip Select)  -> PA4
// CLK (SPI Clock)   -> PA5
// DIN (SPI MOSI)    -> PA7

int main(void)
{
    // Инициализируем GPIO
    GPIO_Init();

    // Инициализируем SPI1
    SPI1_Init();

    // Инициализируем дисплей
    display_init();

    // Отрисовываем картинку
    display_draw_chessboard();

    while (1) { }
}