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

