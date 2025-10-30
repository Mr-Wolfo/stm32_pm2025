#include "stm32f10x.h"

#define EXP_MIN   (-6)
#define EXP_MAX   ( 6)

static volatile int8_t exp_power = 0; // начальная частота 2^0 = 1 Hz

// задержка ~1ms при 72 MHz
// Пробовал через код на ассемблере, но работает не очень
void delay_ms(uint32_t ms)
{
    volatile uint32_t j = 0;
    while (ms--) {
        for (volatile uint32_t i = 0; i < 9000u; i++) __NOP();
        j++;
    }
}


int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // тактирование порта C

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13)) | GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;

    // Настройка PC14 и PC15
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14)) | GPIO_CRH_CNF14_1;
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15)) | GPIO_CRH_CNF15_1;
    GPIOC->ODR |= (GPIO_ODR_ODR14 | GPIO_ODR_ODR15);

    while (1)
    {
        if (!(GPIOC->IDR & GPIO_ODR_ODR14)) {          // PC14 нажата
            delay_ms(15);
            if (!(GPIOC->IDR & GPIO_ODR_ODR14)) {
                while (!(GPIOC->IDR & GPIO_ODR_ODR14)) delay_ms(3); // ждём отпускания
                delay_ms(15);
                if (exp_power < EXP_MAX) exp_power++;
            }
        }

        if (!(GPIOC->IDR & GPIO_ODR_ODR15)) {          // PC15 нажата
            delay_ms(15);
            if (!(GPIOC->IDR & GPIO_ODR_ODR15)) {
                while (!(GPIOC->IDR & GPIO_ODR_ODR15)) delay_ms(3);
                delay_ms(15);
                if (exp_power > EXP_MIN) exp_power--;
            }
        }

       // Период мигания
        uint32_t period_ms;
        if (exp_power >= 0)
            period_ms = 1000u >> exp_power;
        else
            period_ms = 1000u << (-exp_power);

        uint32_t half_ms = (period_ms / 2u) ? (period_ms / 2u) : 1u;

        // Само мигание
        GPIOC->ODR &= ~GPIO_ODR_ODR13;  // включаем LED
        delay_ms(half_ms);

        GPIOC->ODR |= GPIO_ODR_ODR13;   // выключаем LED
        delay_ms(half_ms);
    }
}
