#include "ch32v00x.h"
#include "lcd.h"

// Pin definitions - adjust these to your actual wiring
#define COM0_PIN GPIO_Pin_0  // PC0 - Digit 0
#define COM1_PIN GPIO_Pin_1  // PC1 - Digit 1  
#define COM2_PIN GPIO_Pin_2  // PC2 - Digit 2

#define SEG_A_PIN GPIO_Pin_3  // PC3
#define SEG_B_PIN GPIO_Pin_4  // PC4
#define SEG_C_PIN GPIO_Pin_6  // PC6
#define SEG_D_PIN GPIO_Pin_7  // PC7
#define SEG_E_PIN GPIO_Pin_0  // PD0
#define SEG_F_PIN GPIO_Pin_1  // PD1
#define SEG_G_PIN GPIO_Pin_3  // PD3

// 7-segment patterns for digits 0-9
static const uint8_t digit_patterns[10] = {
    0x3F, // 0: ABCDEF
    0x06, // 1: BC
    0x5B, // 2: ABDEG
    0x4F, // 3: ABCDG
    0x66, // 4: BCFG
    0x6D, // 5: ACDFG
    0x7D, // 6: ACDEFG
    0x07, // 7: ABC
    0x7F, // 8: ABCDEFG
    0x6F  // 9: ABCDFG
};

static uint8_t display_buffer[3] = {0, 0, 0};

void lcd_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    
    // COM pins (PC0, PC1, PC2)
    GPIO_InitStructure.GPIO_Pin = COM0_PIN | COM1_PIN | COM2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // Segment pins on PC (PC3, PC4, PC6, PC7)
    GPIO_InitStructure.GPIO_Pin = SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | SEG_D_PIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // Segment pins on PD (PD0, PD1, PD3)
    GPIO_InitStructure.GPIO_Pin = SEG_E_PIN | SEG_F_PIN | SEG_G_PIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    lcd_clear();
}

static void set_segments(uint8_t pattern) {
    // Clear all segments first
    GPIOC->BCR = SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | SEG_D_PIN;
    GPIOD->BCR = SEG_E_PIN | SEG_F_PIN | SEG_G_PIN;
    
    // Set segments based on pattern
    if (pattern & 0x01) GPIOC->BSHR = SEG_A_PIN;  // A
    if (pattern & 0x02) GPIOC->BSHR = SEG_B_PIN;  // B
    if (pattern & 0x04) GPIOC->BSHR = SEG_C_PIN;  // C
    if (pattern & 0x08) GPIOC->BSHR = SEG_D_PIN;  // D
    if (pattern & 0x10) GPIOD->BSHR = SEG_E_PIN;  // E
    if (pattern & 0x20) GPIOD->BSHR = SEG_F_PIN;  // F
    if (pattern & 0x40) GPIOD->BSHR = SEG_G_PIN;  // G
}

void lcd_display_number(uint16_t number) {
    if (number > 999) number = 999;
    
    display_buffer[0] = digit_patterns[number / 100];
    display_buffer[1] = digit_patterns[(number / 10) % 10];
    display_buffer[2] = digit_patterns[number % 10];
    
    // Hide leading zeros
    if (number < 100) display_buffer[0] = 0;
    if (number < 10) display_buffer[1] = 0;
}

void lcd_clear(void) {
    display_buffer[0] = 0;
    display_buffer[1] = 0;
    display_buffer[2] = 0;
}

void lcd_refresh(void) {
    static uint8_t digit = 0;
    
    // Turn off all COMs
    GPIOC->BCR = COM0_PIN | COM1_PIN | COM2_PIN;
    
    // Set segments for current digit
    set_segments(display_buffer[digit]);
    
    // Turn on current COM
    switch(digit) {
        case 0: GPIOC->BSHR = COM0_PIN; break;
        case 1: GPIOC->BSHR = COM1_PIN; break;
        case 2: GPIOC->BSHR = COM2_PIN; break;
    }
    
    digit = (digit + 1) % 3;
}