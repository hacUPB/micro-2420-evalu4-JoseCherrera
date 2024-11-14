/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_rtc.h"         // Biblioteca para manejo del RTC
#include "fsl_gpio.h"        // Biblioteca GPIO para control de pines
#include "fsl_port.h"        // Biblioteca PORT para configuración de pines
#include "pin_mux.h"         // Biblioteca de inicialización de pines
#include "clock_config.h"    // Configuración del reloj
#include "board.h"           // Archivo del SDK para configuración de la placa

#define BOARD_INITPINS_LED_RED_GPIO GPIOE /*!<@brief GPIO device name: GPIOE */
#define BOARD_INITPINS_LED_RED_PORT PORTE /*!<@brief PORT device name: PORTE */
#define BOARD_INITPINS_LED_RED_PIN 9U     /*!<@brief PORTE pin index: 9 */

void RTC_IRQHandler(void) {
    /* Apaga el LED cuando se genera la interrupción del RTC */
    GPIO_PinWrite(BOARD_INITPINS_LED_RED_GPIO, BOARD_INITPINS_LED_RED_PIN, 1U);  // Apaga el LED

    /* Limpia la bandera de interrupción */
    RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);
}

void initLED(void);
void initRTC(void);

int main(void) {
    /* Inicializar el hardware del sistema */
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Inicializar el LED y el RTC */
    initLED();
    initRTC();

    /* Enciende el LED al inicio */
    GPIO_PinWrite(BOARD_INITPINS_LED_RED_GPIO, BOARD_INITPINS_LED_RED_PIN, 0U);

    while (1) {
        /* El programa principal puede permanecer en espera, ya que el RTC manejará el apagado */
    }
}

void initLED(void) {
    /* Configura el puerto del LED */
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Habilita el reloj para el puerto del LED y configura el pin */
    CLOCK_EnableClock(kCLOCK_PortE);
    PORT_SetPinMux(BOARD_INITPINS_LED_RED_PORT, BOARD_INITPINS_LED_RED_PIN, kPORT_MuxAsGpio);
    GPIO_PinInit(BOARD_INITPINS_LED_RED_GPIO, BOARD_INITPINS_LED_RED_PIN, &led_config);
}

void initRTC(void) {
    /* Configura los pines PTC3 y PTC2 para el cristal externo de 32 kHz */
    CLOCK_EnableClock(kCLOCK_PortC);
    PORT_SetPinMux(PORTC, 2U, kPORT_PinDisabledOrAnalog);  // PTC2 como EXTAL32
    PORT_SetPinMux(PORTC, 3U, kPORT_PinDisabledOrAnalog);  // PTC3 como XTAL32

    /* Configura el RTC */
    rtc_config_t rtcConfig;
    RTC_GetDefaultConfig(&rtcConfig);

    /* Habilita el oscilador externo de 32 kHz para el RTC */
    CLOCK_EnableClock(kCLOCK_Rtc0);      // Habilita el reloj para el módulo RTC
    RTC_Init(RTC, &rtcConfig);
    RTC->CR |= RTC_CR_OSCE_MASK;         // Activa el cristal de 32 kHz

    /* Aumenta el tiempo de estabilización del oscilador */
    for (volatile uint32_t i = 0; i < 0x900000; i++) {
        __NOP();
    }

    /* Configuración del tiempo de alarma en 1 minuto */
    rtc_datetime_t date;
    RTC_GetDatetime(RTC, &date);   // Obtén la fecha y hora actual

    date.second += 10;             // Agrega 60 segundos (1 minuto) a la hora actual
    if (date.second >= 60) {
        date.second -= 60;
        date.minute++;
        if (date.minute >= 60) {
            date.minute = 0;
            date.hour++;
            if (date.hour >= 24) {
                date.hour = 0;
                date.day++;        // Ajuste adicional si cambia el día, pero se asume que no cambia aquí
            }
        }
    }

    /* Establece la alarma en el RTC */
    RTC_SetAlarm(RTC, &date);

    /* Habilita la interrupción de alarma del RTC */
    RTC_EnableInterrupts(RTC, kRTC_AlarmInterruptEnable);
    EnableIRQ(RTC_IRQn);
}


