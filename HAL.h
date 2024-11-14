#ifndef HAL_H
#define HAL_H

#include "fsl_gpio.h"
#include <stdint.h>

// Definición de pines y configuraciones
#define GPIO_DATA_GPIOD GPIOB // Ajustar al puerto GPIO correspondiente
#define BOARD_INITPINS_LCD_RST 10
#define BOARD_INITPINS_LCD_DB0 0
#define BOARD_INITPINS_LCD_DB7 7
#define BOARD_INITPINS_GPIOC GPIOC
#define BOARD_INITPINS_GPIOA GPIOA
#define RS_PIN 1
#define EN_PIN 2
#define BOARD_INITPINS_LCD_RW 3
#define BOARD_INITPINS_LCD_CS1 4
#define BOARD_INITPINS_LCD_CS2 5

void HAL_GPIO_WritePin(GPIO_Type *base, uint32_t pin, uint8_t value);
void HAL_GPIO_ConfigPin(GPIO_Type *base, uint32_t pin, uint8_t direction);
void HAL_Delay(uint32_t time_ns);

#endif // HAL_H

