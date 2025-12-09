#include "stm32f10x.h"

#define EXP_MIN   (-6)
#define EXP_MAX   ( 6)

static volatile int8_t exp_power = 0;

#define PSC_BASE 999
#define ARR_BASE 35999



void debounce_wait(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++) {
        __NOP();
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {

        // Сбрасываем флаг прерывания
        TIM2->SR &= ~TIM_SR_UIF;

        // Переключаем состояние светодиода PC13
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
}



void timer_init(void)
{
    // Тактирование TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Сброс TIM2
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

    // Настройка предделителя и предельного значения
    TIM2->PSC = PSC_BASE;
    TIM2->ARR = ARR_BASE;

    // Включение прерывания по обновлению
    TIM2->DIER |= TIM_DIER_UIE;

    // Настройка NVIC
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);

    // Запуск таймера
    TIM2->CR1 |= TIM_CR1_CEN;
}

void gpio_init(void)
{
    // Тактирование порта C
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13)) | GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14)) | GPIO_CRH_CNF14_1;
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15)) | GPIO_CRH_CNF15_1;

    GPIOC->ODR |= (GPIO_ODR_ODR14 | GPIO_ODR_ODR15);
}



int main(void)
{
    gpio_init();
    timer_init();

    // Переменная для отслеживания состояния кнопок
    uint32_t button_mask_pc14 = GPIO_IDR_IDR14;
    uint32_t button_mask_pc15 = GPIO_IDR_IDR15;


    while (1)
    {
        if (!(GPIOC->IDR & button_mask_pc14)) {
            debounce_wait(1000);

            if (!(GPIOC->IDR & button_mask_pc14)) {

                if (exp_power < EXP_MAX) {
                    exp_power++;

                    if (TIM2->PSC > 0) {
                        TIM2->PSC = TIM2->PSC >> 1;
                    }
                }

                while (!(GPIOC->IDR & button_mask_pc14)) {
                    __NOP();
                }
                debounce_wait(1000);
            }
        }

        if (!(GPIOC->IDR & button_mask_pc15)) {
            debounce_wait(1000);

            if (!(GPIOC->IDR & button_mask_pc15)) {

                // Проверка границ
                if (exp_power > EXP_MIN) {
                    exp_power--;

                    if (TIM2->PSC <= (0xFFFF / 2)) {
                        TIM2->PSC = TIM2->PSC << 1;
                    } else {
                        exp_power = EXP_MIN;
                        TIM2->PSC = 0xFFFF;
                    }
                }

                // Ждём отпускания кнопки
                while (!(GPIOC->IDR & button_mask_pc15)) {
                    __NOP();
                }
                debounce_wait(1000);
            }
        }
    }
}