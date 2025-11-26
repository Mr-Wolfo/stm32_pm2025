#include "../inc/gpio.h"

// Инициализация портов ввода-вывода
void GPIO_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1 |
                    GPIO_CRL_CNF2 | GPIO_CRL_MODE2 |
                    GPIO_CRL_CNF4 | GPIO_CRL_MODE4 |
                    GPIO_CRL_CNF5 | GPIO_CRL_MODE5 |
                    GPIO_CRL_CNF7 | GPIO_CRL_MODE7);

    // PA1 (DC), PA2 (RES), PA4 (CS)
    GPIOA->CRL |= GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE4;

    // PA5 (CLK), PA7 (DIN)
    GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1 |
                  GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;
}