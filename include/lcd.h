#pragma once

#include <stdint.h>

void lcd_init(void);
void lcd_display_number(uint16_t number);
void lcd_clear(void);
void lcd_refresh(void);