#ifndef API_H
#define API_H

#include <stdint.h>

void lcd_initialize(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_display_text(const char *text);
void lcd_activate_left(void);
void lcd_activate_right(void);

#endif // API_H

