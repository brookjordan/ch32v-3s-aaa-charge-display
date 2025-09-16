// Minimal HAL abstraction so you can swap in your preferred CH32V003 library
// (e.g., WCH SPL, ch32v003fun, or direct registers). The provided stub compiles
// but does not touch real hardware. Replace with real GPIO/ADC init and access.

#pragma once

#include <stdint.h>
#include <stdbool.h>

// Logical pins for the HT1621 3-wire interface. Map these to real GPIOs in your HAL.
// Suggested CH32V003 pins (verify with your package/datasheet):
//   - HT1621_CS   -> e.g. PC1
//   - HT1621_WR   -> e.g. PC2
//   - HT1621_DATA -> e.g. PC3
// Define them to any small integers; your HAL implementation should map them to real ports/bits.

enum {
    PIN_HT1621_CS = 1,
    PIN_HT1621_WR = 2,
    PIN_HT1621_DATA = 3,
};

// ADC channel for the battery divider node.
// PC4 (pin 8) = ADC_IN2 - recommended for voltage divider
#ifndef ADC_CHANNEL_BATT
#define ADC_CHANNEL_BATT 2
#endif

// ADC resolution (bits). CH32V003 ADC is commonly 10-bit.
#ifndef HAL_ADC_BITS
#define HAL_ADC_BITS 10
#endif

void hal_init(void);
void hal_delay_ms(uint32_t ms);
void hal_delay_us(uint32_t us);

void hal_gpio_output(uint8_t pin);
void hal_gpio_set(uint8_t pin);
void hal_gpio_clear(uint8_t pin);

int  hal_adc_init(uint8_t channel);
uint16_t hal_adc_read_raw(void);

// ------------------- Direct LCD Drive (no HT1621) -------------------
// If you drive the TN LCD directly, define logical pins for COM and SEG lines here.
// You will wire your glass COM0..COMn and SEG lines to these GPIOs.
// Adjust the counts and pin assignments to your hardware.

#ifndef LCD_NUM_COM
#define LCD_NUM_COM 3   // 3-digit glass typically has 3 commons (one per digit)
#endif
#ifndef LCD_NUM_SEG
#define LCD_NUM_SEG 8   // 7 segments + optional symbol/DP
#endif

// Logical IDs for GPIOs (map these to real pins in your HAL implementation)
enum {
    // COM lines (one per digit: left, middle, right)
    PIN_LCD_COM0 = 10,
    PIN_LCD_COM1 = 11,
    PIN_LCD_COM2 = 12,
    // If your glass has 4 commons, also define PIN_LCD_COM3 and set LCD_NUM_COM=4
    PIN_LCD_COM3 = 13, // optional
    // Segment lines (shared across digits)
    PIN_LCD_SEG_A = 20,
    PIN_LCD_SEG_B = 21,
    PIN_LCD_SEG_C = 22,
    PIN_LCD_SEG_D = 23,
    PIN_LCD_SEG_E = 24,
    PIN_LCD_SEG_F = 25,
    PIN_LCD_SEG_G = 26,
    PIN_LCD_SEG_SYM = 27, // decimal point or percent symbol (optional)
};

// CPU frequency hint for timing (used by software UART delays in stub).
#ifndef HAL_CPU_HZ
#define HAL_CPU_HZ 24000000UL
#endif

// TX pin for software UART logging - PD6 (pin 20)
#ifndef PIN_UART_TX
#define PIN_UART_TX 40  // Maps to PD6
#endif
