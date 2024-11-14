# Documentación del Proyecto
---
**Nombre del estudiante**:  Jose Carlos Herrera Castro 
**ID**: 000269913

## Unidad 4 

El codigo llamado Led_blinky es el la actividad extra , la cual es un un led que tiene un tiempo de apagado que ingrese el usuario y su conteo lo genera el RTC(Real Time Clock)

## Explicacion de codigo 

## Proyecto LCD Control - Documentación
Este proyecto implementa una interfaz de control para un LCD a través de una estructura modular en C, utilizando archivos de encabezado y fuentes para organizar las funcionalidades de alto y bajo nivel. A continuación, se describe el propósito y funcionamiento de cada archivo en el proyecto.

## 1. `API.c`

**Propósito**: Este archivo implementa la capa de alto nivel del programa, proporcionando funciones que abstraen las complejidades del hardware y permiten interactuar con el LCD de manera sencilla. La interfaz en `API.c` permite inicializar el LCD, limpiar la pantalla, establecer la posición del cursor, y mostrar texto.

```c
#include "API.h"       // Incluye la API que define las funciones de alto nivel para controlar el LCD
#include "HAL.h"       // Incluye funciones de bajo nivel para controlar los pines del microcontrolador
#include "LOW_LEVEL.h" // Incluye funciones específicas de control para enviar comandos y datos al LCD
#include <stdint.h>    // Proporciona definiciones de tipos de datos estándar de tamaño fijo, como uint8_t
#include "fsl_gpio.h"  // Librería específica del microcontrolador para manipular pines GPIO
#include "fsl_common.h"// Librería común del microcontrolador que incluye funciones de utilidad
```

### 1.1  `lcd_initialize()`

Esta función prepara el LCD para recibir comandos y datos inicializando varios pines y configuraciones.

```c
void lcd_initialize(void)
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
```

-	**Reset del LCD:** Se coloca el pin `RST` en bajo y luego en alto para reiniciar el LCD.
-	**Desactivación temporal del LCD:** Se lleva el pin `EN` a bajo, permitiendo que el LCD se estabilice.
-	**Configuración en modo comandos:** El pin `RS` en bajo indica que se enviarán comandos.
-	**Configuración en modo escritura:** Se lleva el pin `RW` a bajo.
-	**Activación de controladores:** Activa ambos controladores (`lcd_activate_left()` y `lcd_activate_right()`) para inicializar la pantalla.

### 1.2 `lcd_clear()`

Limpia el contenido de la pantalla del LCD enviando el comando `0x01`.

```c
void lcd_clear(void)
{
    lcd_send_command_byte(0x01);  // Envía el comando `0x01` para limpiar la pantalla del LCD
    HAL_Delay(2);                 // Espera 2 ms para permitir que el LCD termine de procesar el comando de limpieza
}
```
-	**Envío del comando de limpieza:** `lcd_send_command_byte (0x01) ` borra la pantalla.
-	**Retraso:** Espera de 2 ms para que el LCD complete la operación.

### 1.3  `lcd_set_cursor()`

Posiciona el cursor en una ubicación específica de la pantalla.

```c
void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? col : (0x40 + col); // Calcula la dirección de la memoria de video (DDRAM) según la fila y la columna
    lcd_send_command_byte(0x80 | address);  // Envía el comando `0x80 | address` para establecer la posición del cursor en la DDRAM
}
```

-	**Calculo de la dirección de memoria:** Dependiendo de la fila, ajusta la dirección DDRAM para posicionar el cursor.
-	**Envío del comando:** Se configura la posición del cursor en la memoria de video.


### 1.4 `lcd_display_text()`

Muestra una cadena de texto en la posición actual del cursor en la pantalla.

```c

void lcd_display_text(const char *text)
{
    while (*text) // Bucle que recorre cada carácter en la cadena de texto
    {
        lcd_send_data_byte(*text++); // Envía cada carácter al LCD uno por uno en modo de datos
    }
}
```
-	**Bucle de caracteres:** Recorre la cadena de texto y envía cada carácter al LCD.
-	**Envío de datos:** Usa `lcd_send_data_byte()` para enviar cada carácter como datos.

### 1.5 `lcd_activate_left()`

Activa el controlador izquierdo del LCD, responsable de la mitad izquierda de la pantalla.

```c
void lcd_activate_left(void)
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
```

-	**Envío de comando de activación:** Se envía el comando `0x3F` para encender el controlador izquierdo.
-	**Selección del controlador:** Se usa `CS2` para activar la mitad izquierda de la pantalla.
-	**Habilitación del pin `EN`:** Activa `EN` en alto para iniciar la operación de envío, y luego lo desactiva.

### 1.6 `lcd_activate_right()`

Similar a `lcd_activate_left()`, pero para el controlador derecho de la pantalla.

```c
void lcd_activate_right(void)
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
```


 ## 2. `HAL.c`

- **Propósito**: Este archivo es la Capa de Abstracción de Hardware (HAL, por sus siglas en inglés). Define funciones para controlar pines GPIO y manipular directamente el hardware del microcontrolador. Estas funciones permiten escribir valores en pines, configurar su dirección (entrada o salida), y generar retardos.

```c
#include "API.h"       // Incluye la API para manejar funciones de alto nivel del LCD.
#include "HAL.h"       // Incluye funciones de bajo nivel para manipular los pines GPIO.
#include "LOW_LEVEL.h" // Incluye funciones específicas de control del LCD.
#include <stdint.h>    // Proporciona definiciones de tipos de datos estándar.
#include "fsl_gpio.h"  // Biblioteca para controlar pines GPIO del microcontrolador.
#include "fsl_common.h"// Incluye funciones comunes del microcontrolador.
#include "pin_mux.h"   // Configuración de los pines (define los pines del sistema).
```

### 2.1  `HAL_GPIO_WritePin()`
Esta función cambia el estado de un pin específico, colocándolo en alto (encendido) o en bajo (apagado), según el valor de `output`.

```c
void HAL_GPIO_WritePin(GPIO_Type *base, uint32_t pin, uint8_t output)
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
```
-	**Parámetros:** base es la base del puerto GPIO, pin es el número de pin y output define el estado deseado (0 o 1).
-	**Configuración del pin:** Si output es 0, el registro `PCOR` coloca el pin en bajo; si es 1, el registro `PSOR` lo coloca en alto.

### 2.2 `HAL_GPIO_OutputConfig()`

Configura un pin específico como entrada o salida.

```c
void HAL_GPIO_OutputConfig(GPIO_Type *base, uint32_t pin, uint8_t estado)
{
    base->PDDR = GPIO_FIT_REG(estado << pin); // Cambia la dirección del pin en el registro PDDR
}
```
-	**Parámetros:** base es la base del puerto GPIO, pin es el número de pin y estado define la dirección deseada (0 para entrada, 1 para salida).
-	**Configuración de la dirección:** Modifica el registro `PDDR` para establecer el pin como entrada o salida.

### `2.3 DATA_OUTPUT()`

Configura los pines de datos (DB0 a DB7) como salidas.

```c
void DATA_OUTPUT(void)
{
    for (int pin = BOARD_INITPINS_LCD_DB0; pin <= BOARD_INITPINS_LCD_DB7; pin++) // Itera desde DB0 hasta DB7
    {
        HAL_GPIO_OutputConfig(GPIO_DATA_GPIOD, pin, 1); // Configura cada pin de datos como salida (1)
    }
}
```

-	**Iteración sobre pines de datos:** Recorre los pines desde `DB0` hasta `DB7`.
-	**Configuración como salidas:** Llama a `HAL_GPIO_OutputConfig()` con el valor 1 para cada pin, configurándolos como salidas.

### `2.4 DATA_INPUT()`

Configura los pines de datos (DB0 a DB7) como entradas.

```c
void DATA_INPUT(void)
{
    for (int pin = BOARD_INITPINS_LCD_DB0; pin <= BOARD_INITPINS_LCD_DB7; pin++) // Itera desde DB0 hasta DB7
    {
        HAL_GPIO_OutputConfig(GPIO_DATA_GPIOD, pin, 0); // Configura cada pin de datos como entrada (0)
    }
}
```

-	**Iteración sobre pines de datos:** Igual que en `DATA_OUTPUT()`, recorre los pines de DB0 a DB7.
-	**Configuración como entradas:** Llama a `HAL_GPIO_OutputConfig()` con el valor 0 para cada pin, configurándolos como entradas.

### 2.5  `HAL_Delay()`

Genera un retardo en nanosegundos.

```c
void HAL_Delay(uint32_t tiempo)
{
    float x = tiempo / 5.952; // Calcula el número de ciclos necesarios en función del tiempo deseado
    int num = ceil(x); // Redondea hacia arriba para garantizar el retardo completo
    for (int i = 0; i <= num; i++) // Bucle que ejecuta "no operation" para el retardo
    {
        asm("nop"); // "nop" significa "no operation"; mantiene el procesador ocupado
    }
}
```
-	**Cálculo del ciclo de retardo:** Divide el tiempo deseado por 5.952 (factor basado en el hardware) para obtener el número de ciclos necesarios.
-	**Redondeo y bucle de espera:** Usa `ceil()` para redondear el valor y realiza un bucle que ejecuta `nop` (sin operación) para crear el retardo.


## 3. `LOW_LEVEL.c`

- **Propósito:** `LOW_LEVEL.c` implementa funciones específicas para enviar comandos y datos al LCD a nivel de bits. Este archivo convierte cada byte en bits individuales y maneja la configuración de los pines para el modo de comando o de datos en el LCD.

```c
Copiar código
#include "API.h"       // Incluye la API para manejar el LCD.
#include "HAL.h"       // Incluye funciones de bajo nivel para manipular los pines.
#include "LOW_LEVEL.h" // Incluye funciones específicas para el LCD.
#include <stdint.h>    // Proporciona tipos de datos de tamaño fijo.
#include "pin_mux.h"   // Configuración de pines (define los pines del sistema).
```
### 3.1  `lcd_send_command_byte()`

Esta función envía un comando de 8 bits al LCD. Cada bit del comando se extrae de `byte` y se pasa a la función `lcd_send_command()`.

```c
void lcd_send_command_byte(uint8_t byte)
{
    lcd_send_command(
        (byte >> 7) & 1, (byte >> 6) & 1, (byte >> 5) & 1,
        (byte >> 4) & 1, (byte >> 3) & 1, (byte >> 2) & 1,
        (byte >> 1) & 1, byte & 1
    );
}
```
-	**Desplazamiento y extracción de bits:** Usa desplazamientos y operaciones AND para extraer cada bit del byte.
-	**Envío del comando:** Llama a `lcd_send_command()` con los bits separados como parámetros, representando el comando completo.

### 3.2  `lcd_send_data_byte()`

Esta función envía un byte de datos al LCD, con un funcionamiento similar al de `lcd_send_command_byte()`.

```c
void lcd_send_data_byte(uint8_t byte)
{
    lcd_send_data(
        (byte >> 7) & 1, (byte >> 6) & 1, (byte >> 5) & 1,
        (byte >> 4) & 1, (byte >> 3) & 1, (byte >> 2) & 1,
        (byte >> 1) & 1, byte & 1
    );
}
```

-	**Desplazamiento y extracción de bits:** Similar al caso de `lcd_send_command_byte()`.
-	**Envío del dato:** Llama a `lcd_send_data()` con los bits individuales del dato.

### 3.3  `lcd_send_command()`

Envía un comando de 8 bits al LCD utilizando un arreglo `command` que contiene cada bit.

```c
void lcd_send_command(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0)
{
    int command[8] = {b0, b1, b2, b3, b4, b5, b6, b7}; // Arreglo con cada bit del comando
    send_data_or_command(command, 0); // Modo comando (RS = 0)
}
```

-	**Almacenamiento de bits:** Los bits se almacenan en el arreglo command.
-	**Modo de comando:** Llama a send_data_or_command() con is_data igual a 0 para indicar que es un comando.

### 3.4 `lcd_send_data()`

Envía un dato de 8 bits al LCD, similar a `lcd_send_command()` pero en modo de datos.

```c
void lcd_send_data(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0)
{
    int data[8] = {b0, b1, b2, b3, b4, b5, b6, b7}; // Arreglo con cada bit del dato
    send_data_or_command(data, 1); // Modo datos (RS = 1)
}
```

-	**Almacenamiento de bits:** Los bits se almacenan en el arreglo `data`.
-	**Modo de datos:** Llama a `send_data_or_command()` con `is_data` igual a 1 para indicar que es un dato.

### 3.5  `send_data_or_command()`

Función general para enviar un arreglo de 8 bits al LCD, en modo comando o datos, según `is_data`.

```c
void send_data_or_command(int bits[8], int is_data)
{
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa Enable
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 1); // Modo lectura
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, is_data); // Configura RS: 0 = comando, 1 = dato
    HAL_Delay(500); // Espera para estabilizar
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva Enable
    HAL_Delay(500);

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 0); // Modo escritura
    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, is_data); // RS nuevamente
    HAL_Delay(200); // Espera

    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 1); // Activa Enable
    HAL_Delay(500); // Espera

    DATA_OUTPUT(); // Configura pines de datos como salidas
    for (int i = 0; i < 8; i++) { // Envía cada bit
        HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_DB0 + i, bits[i]);
    }

    HAL_Delay(250); // Espera para procesar
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, EN_PIN, 0); // Desactiva Enable
    HAL_Delay(50); // Espera corto

    HAL_GPIO_WritePin(BOARD_INITPINS_GPIOC, RS_PIN, 1); // RS a datos (por defecto)
    HAL_GPIO_WritePin(GPIO_DATA_GPIOD, BOARD_INITPINS_LCD_RW, 1); // Modo lectura (por defecto)
    DATA_INPUT(); // Pines de datos a entrada
    HAL_Delay(1000); // Tiempo de ciclo para finalizar
}
```
-	**Configuración de pines para escritura:** Primero, coloca el LCD en modo lectura y configura RS en modo comando o datos, según `is_data`.
-	**Estabilización y ciclo de Enable:** Activa y desactiva el pin `Enable` (E) para iniciar el envío de bits.
-	**Escritura de bits:** Llama a `DATA_OUTPUT()` para configurar los pines de datos como salidas y envía cada bit en el arreglo bits.
-	**Finalización de la transmisión:** Vuelve a modo lectura y configura `RS` como datos para restablecer los pines en su estado predeterminado. 


 ## 4. `API.h`

- **Propósito:** Declara las funciones de alto nivel para controlar el LCD, proporcionando una interfaz para inicializar, limpiar, y escribir en el LCD.

### Contenido del Archivo

```c
#ifndef API_H
#define API_H

#include <stdint.h>

void lcd_initialize(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_display_text(const char *str);
void lcd_activate_left(void);
void lcd_activate_right(void);

#endif

```
## 5. `HAL.h`

- **Propósito:** Proporciona las declaraciones de las funciones de abstracción de hardware y define los pines específicos para la configuración del LCD.

### Contenido del Archivo
```c
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

```
## 6. `LOW_LEVEL.h`

- **Propósito:** Define las funciones de bajo nivel para enviar comandos y datos al LCD, así como la función `send_data_or_command`.

### Contenido del Archivo

```c
#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#include <stdint.h>

void lcd_send_command(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0);
void lcd_send_data(int b7, int b6, int b5, int b4, int b3, int b2, int b1, int b0);

// Nuevas funciones para enviar comandos y datos de 1 byte
void lcd_send_command_byte(uint8_t byte);
void lcd_send_data_byte(uint8_t byte);

void Set_Data_Output(void);  // Declaración de la función
void Set_Data_Input(void);   // Declaración de la función

#endif // LOW_LEVEL_H


```
## 7. `LCD.c`

- **Propósito:** Este archivo contiene el main del programa y realiza la configuración inicial del sistema, además de invocar las funciones para interactuar con el LCD.

### Código Explicado

```c
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

```

