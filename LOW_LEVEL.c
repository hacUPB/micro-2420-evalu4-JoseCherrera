#include "API.h"       // Incluye la API para manejar el LCD.
#include "HAL.h"       // Incluye funciones de bajo nivel para manipular los pines.
#include "LOW_LEVEL.h" // Incluye funciones específicas para el LCD.
#include <stdint.h>    // Proporciona tipos de datos de tamaño fijo.
#include "pin_mux.h"   // Configuración de pines (define los pines del sistema).

void lcd_send_command_byte(uint8_t byte) // Envía un comando de un byte al LCD
{
    lcd_send_command(
        (byte >> 7) & 1, (byte >> 6) & 1, (byte >> 5) & 1, // Desplaza y extrae cada bit
        (byte >> 4) & 1, (byte >> 3) & 1, (byte >> 2) & 1,
        (byte >> 1) & 1, byte & 1
    ); // Llama a lcd_send_command con cada bit individual para enviar el comando completo
}

void lcd_send_data_byte(uint8_t byte) // Envía un dato de un byte al LCD
{
    lcd_send_data(
        (byte >> 7) & 1, (byte >> 6) & 1, (byte >> 5) & 1, // Desplaza y extrae cada bit
        (byte >> 4) & 1, (byte >> 3) & 1, (byte >> 2) & 1,
        (byte >> 1) & 1, byte & 1
    ); // Llama a lcd_send_data con cada bit individual para enviar el dato completo
}

void lcd_send_command(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0) // Envía un comando de 8 bits
{
    int command[8] = {b0, b1, b2, b3, b4, b5, b6, b7}; // Arreglo que contiene cada bit del comando
    send_data_or_command(command, 0); // Modo comando (RS = 0)
}

void lcd_send_data(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0) // Envía un dato de 8 bits
{
    int data[8] = {b0, b1, b2, b3, b4, b5, b6, b7}; // Arreglo que contiene cada bit del dato
    send_data_or_command(data, 1); // Modo datos (RS = 1)
}

void send_data_or_command(int bits[8], int is_data) // Envía un arreglo de 8 bits al LCD, ya sea comando o dato
{
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa el pin Enable (E) para iniciar la operación
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 1); // Configura el LCD en modo lectura temporalmente
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, is_data); // Configura RS: 0 = comando, 1 = dato
    HAL_Delay(500); // Espera para estabilizar
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva Enable
    HAL_Delay(500); // Espera para permitir la operación de escritura

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 0); // Configura en modo escritura
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, is_data); // Configura RS de nuevo
    HAL_Delay(200); // Espera para que el LCD lea el estado

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa Enable de nuevo
    HAL_Delay(500); // Espera

    DATA_OUTPUT(); // Configura los pines de datos como salidas
    for (int i = 0; i < 8; i++) { // Recorre los bits y envía cada uno
        HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_DB0 + i, bits[i]);
    }

    HAL_Delay(250); // Espera a que el LCD procese
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva Enable al final de la transmisión
    HAL_Delay(50); // Espera corto

    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, 1); // Devuelve RS al modo de datos (por defecto)
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 1); // Configura en modo lectura por defecto
    DATA_INPUT(); // Cambia pines de datos a entrada
    HAL_Delay(1000); // Tiempo de ciclo para completar la operación
}
