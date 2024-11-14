#include "API.h"       // Incluye la API que define las funciones de alto nivel para controlar el LCD
#include "HAL.h"       // Incluye funciones de bajo nivel para controlar los pines del microcontrolador
#include "LOW_LEVEL.h" // Incluye funciones específicas de control para enviar comandos y datos al LCD
#include <stdint.h>    // Proporciona definiciones de tipos de datos estándar de tamaño fijo, como uint8_t
#include "fsl_gpio.h"  // Librería específica del microcontrolador para manipular pines GPIO
#include "fsl_common.h"// Librería común del microcontrolador que incluye funciones de utilidad

void lcd_initialize(void) // Inicializa el LCD para que esté listo para recibir comandos y datos
{
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RST, 0); // Lleva el pin de reinicio (RST) a bajo (0) para resetear el LCD
    HAL_Delay(1000); // Espera 1 ms para que el LCD complete el reset

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RST, 1); // Lleva el pin de reinicio a alto (1) para finalizar el reset
    HAL_Delay(1000); // Espera 1 ms para asegurarse de que el LCD está listo

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Lleva el pin Enable (E) a bajo para desactivar el LCD temporalmente
    HAL_Delay(1000); // Espera 1 ms para estabilizar el estado del LCD

    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, 0); // Lleva el pin RS (Registro Seleccionado) a bajo para indicar que se enviarán comandos (no datos)
    HAL_Delay(1000); // Espera 1 ms para estabilizar el pin RS

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 0); // Lleva el pin RW (Lectura/Escritura) a bajo para configurar el LCD en modo escritura
    HAL_Delay(1000); // Espera 1 ms para estabilizar el pin RW

    lcd_activate_left();  // Activa el controlador izquierdo del LCD para habilitar la primera mitad de la pantalla
    lcd_activate_right(); // Activa el controlador derecho del LCD para habilitar la segunda mitad de la pantalla
}

void lcd_clear(void) // Borra el contenido de la pantalla del LCD
{
    lcd_send_command_byte(0x01);  // Envía el comando `0x01` para limpiar la pantalla del LCD
    HAL_Delay(2);                 // Espera 2 ms para permitir que el LCD termine de procesar el comando de limpieza
}

void lcd_set_cursor(uint8_t row, uint8_t col) // Posiciona el cursor en una ubicación específica en la pantalla del LCD
{
    uint8_t address = (row == 0) ? col : (0x40 + col); // Calcula la dirección de la memoria de video (DDRAM) según la fila y la columna
    lcd_send_command_byte(0x80 | address);  // Envía el comando `0x80 | address` para establecer la posición del cursor en la DDRAM
}

void lcd_display_text(const char *text) // Muestra una cadena de texto en la posición actual del cursor
{
    while (*text) // Bucle que recorre cada carácter en la cadena de texto
    {
        lcd_send_data_byte(*text++); // Envía cada carácter al LCD uno por uno en modo de datos
    }
}

void lcd_activate_left(void) // Activa el controlador izquierdo del LCD, que controla la mitad izquierda de la pantalla
{
    lcd_send_command_byte(0x3F);  // Envía el comando `0x3F` para encender el controlador izquierdo del LCD
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOA, BOARD_INITPINS_LCD_CS2, 1); // Activa el pin CS2 para seleccionar el controlador izquierdo
    HAL_Delay(1000); // Espera 1 ms para estabilizar el controlador

    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, 0); // Configura el pin RS en modo comando para enviar comandos al controlador
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa el pin Enable (E) en alto para comenzar la operación de envío
    HAL_Delay(1000); // Espera 1 ms para permitir que el LCD procese el comando

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva el pin Enable (E) para finalizar la operación de envío
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOA, BOARD_INITPINS_LCD_CS2, 0); // Desactiva el pin CS2 para deseleccionar el controlador izquierdo
}

void lcd_activate_right(void) // Activa el controlador derecho del LCD, que controla la mitad derecha de la pantalla
{
    lcd_send_command_byte(0x3F);  // Envía el comando `0x3F` para encender el controlador derecho del LCD
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOA, BOARD_INITPINS_LCD_CS1, 1); // Activa el pin CS1 para seleccionar el controlador derecho
    HAL_Delay(1000); // Espera 1 ms para estabilizar el controlador

    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, 0); // Configura el pin RS en modo comando para enviar comandos al controlador
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa el pin Enable (E) en alto para comenzar la operación de envío
    HAL_Delay(1000); // Espera 1 ms para permitir que el LCD procese el comando

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva el pin Enable (E) para finalizar la operación de envío
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOA, BOARD_INITPINS_LCD_CS1, 0); // Desactiva el pin CS1 para deseleccionar el controlador derecho
}
