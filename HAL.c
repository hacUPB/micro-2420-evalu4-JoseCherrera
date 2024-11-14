#include "API.h"       // Incluye la API para manejar funciones de alto nivel del LCD.
#include "HAL.h"       // Incluye funciones de bajo nivel para manipular los pines GPIO.
#include "LOW_LEVEL.h" // Incluye funciones específicas de control del LCD.
#include <stdint.h>    // Proporciona definiciones de tipos de datos estándar.
#include "fsl_gpio.h"  // Biblioteca para controlar pines GPIO del microcontrolador.
#include "fsl_common.h"// Incluye funciones comunes del microcontrolador.
#include "pin_mux.h"   // Configuración de los pines (define los pines del sistema).

void HAL_GPIO_WritePin(GPIO_Type *base, uint32_t pin, uint8_t output) // Cambia el estado de un pin específico (alto o bajo)
{
    uint32_t u32flag = 1; // Bandera para manipular el pin deseado
    if (output == 0U) // Si el valor de salida es 0...
    {
        base->PCOR = GPIO_FIT_REG(u32flag << pin); // Pone el pin en bajo (apagado) utilizando el registro PCOR
    }
    else // Si el valor de salida es 1...
    {
        base->PSOR = GPIO_FIT_REG(u32flag << pin); // Pone el pin en alto (encendido) utilizando el registro PSOR
    }
}

void HAL_GPIO_OutputConfig(GPIO_Type *base, uint32_t pin, uint8_t estado) // Configura el pin como entrada (0) o salida (1)
{
    base->PDDR = GPIO_FIT_REG(estado << pin); // Cambia la dirección del pin en el registro PDDR
}

void DATA_OUTPUT(void) // Configura los pines de datos (DB0 a DB7) como salidas
{
    for (int pin = BOARD_INITPINS_LCD_DB0; pin <= BOARD_INITPINS_LCD_DB7; pin++) // Itera desde DB0 hasta DB7
    {
        HAL_GPIO_OutputConfig(GPIO_DATA_GPIOD, pin, 1); // Configura cada pin de datos como salida (1)
    }
}

void DATA_INPUT(void) // Configura los pines de datos (DB0 a DB7) como entradas
{
    for (int pin = BOARD_INITPINS_LCD_DB0; pin <= BOARD_INITPINS_LCD_DB7; pin++) // Itera desde DB0 hasta DB7
    {
        HAL_GPIO_OutputConfig(GPIO_DATA_GPIOD, pin, 0); // Configura cada pin de datos como entrada (0)
    }
}

void HAL_Delay(uint32_t tiempo) // Genera un retardo en nanosegundos
{
    float x = tiempo / 5.952; // Calcula el número de ciclos necesarios en función del tiempo deseado
    int num = ceil(x); // Redondea hacia arriba para garantizar el retardo completo
    for (int i = 0; i <= num; i++) // Bucle que ejecuta "no operation" para el retardo
    {
        asm("nop"); // "nop" significa "no operation"; mantiene el procesador ocupado
    }
}
