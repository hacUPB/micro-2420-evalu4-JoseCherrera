#include "API.h"
#include "HAL.h"
#include "LOW_LEVEL.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "pin_mux.h"

// Inicialización del sistema
void System_Init(void);

int main(void)
{
    // Configuración del sistema
    System_Init();

    // Inicialización del LCD
    lcd_initialize();

    // Limpia la pantalla
    lcd_clear();

    // Coloca el cursor en la posición inicial
    lcd_set_cursor(0, 0);

    // Muestra un mensaje
    lcd_display_text("Hola, Mundo!");

    // Coloca el cursor en la segunda línea y muestra otro mensaje
    lcd_set_cursor(1, 0);
    lcd_display_text("LCD Iniciado");

    // Bucle infinito
    while (1)
    {
        // Aquí puedes agregar código para actualizar el LCD
    }

    return 0;
}

void System_Init(void)
{
    // Inicializa los pines de la placa
    BOARD_InitBootPins();
    BOARD_InitBootClocks();           //  ¿estas funciones estén disponibles?
    BOARD_InitBootPeripherals();      //  ¿están definidas en los archivos de configuración de la placa?
}


