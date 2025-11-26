#include "../inc/spi.h"

void SPI1_Init(void) {
    // Тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Конфигурирование SPI1
    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA |
                 SPI_CR1_MSTR | SPI_CR1_BR |
                 SPI_CR1_SSM | SPI_CR1_SPE;
}

// Отправка байта по SPI
void SPI1_Write(uint8_t data) {
    while(!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
}

// Прием байта по SPI
uint8_t SPI1_Read(void) {
    SPI1->DR = 0;
    while(!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}