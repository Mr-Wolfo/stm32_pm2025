#include "../inc/ssd1306.h"
#include "../inc/spi.h"

void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 8000; j++) __NOP(); // ~1ms при 72MHz
    }
}

void display_reset(void) {
    GPIOA->ODR &= ~GPIO_ODR_ODR2; // RES=0
    delay_ms(10);
    GPIOA->ODR |= GPIO_ODR_ODR2;  // RES=1
    delay_ms(10);
}

void display_cmd(uint8_t cmd) {
    GPIOA->ODR &= ~GPIO_ODR_ODR4; // CS=0
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // DC=0
    SPI1_Write(cmd);
    GPIOA->ODR |= GPIO_ODR_ODR4;  // CS=1
}

void display_data(uint8_t data) {
    GPIOA->ODR &= ~GPIO_ODR_ODR4; // CS=0
    GPIOA->ODR |= GPIO_ODR_ODR1;  // DC=1
    SPI1_Write(data);
    GPIOA->ODR |= GPIO_ODR_ODR4;  // CS=1
}


void display_init(void) {
    display_reset();

    display_cmd(0xAE);
    display_cmd(0xD5);
    display_cmd(0x80);
    display_cmd(0xA8);
    display_cmd(0x3F);
    display_cmd(0xD3);
    display_cmd(0x00);
    display_cmd(0x40);
    display_cmd(0x8D);
    display_cmd(0x14);
    display_cmd(0x20);
    display_cmd(0x00);
    display_cmd(0xA1);
    display_cmd(0xC8);
    display_cmd(0xDA);
    display_cmd(0x12);
    display_cmd(0x81);
    display_cmd(0xCF);
    display_cmd(0xD9);
    display_cmd(0xF1);
    display_cmd(0xDB);
    display_cmd(0x40);
    display_cmd(0xA4);
    display_cmd(0xA6);
    display_cmd(0xAF); // Display ON
}

void display_draw_chessboard(void) {
    for (uint8_t page = 0; page < 8; page++) {
        display_cmd(0xB0 + page);
        display_cmd(0x00);
        display_cmd(0x10);

        for (uint8_t col = 0; col < 128; col++) {
            // Определяем, в какой квадрат 8x8 мы попали
            if ((page + (col / 8)) % 2 == 0) {
                // Если сумма координат квадрата четная - рисуем БЕЛЫЙ квадрат
                display_data(0xFF);
            } else {
                // Если нечетная - рисуем ЧЕРНЫЙ квадрат
                display_data(0x00);
            }
        }
    }
}