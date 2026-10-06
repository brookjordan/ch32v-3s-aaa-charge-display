#include "ch32v00x.h"

// Pin mapping based on your test results
// PC0=pin0, PC1=pin1, PC2=pin2, PC3=pin3, PC4=pin4, PC6=pin5, PC7=pin6, PD0=pin7, PD1=pin8, PD3=pin9

void lcd_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void show_digit(uint8_t digit, uint8_t value) {
    // Clear all pins
    GPIOC->BCR = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIOD->BCR = GPIO_Pin_3 | GPIO_Pin_4;
    
    uint16_t pc_pins = 0, pd_pins = 0;
    
    // Select digit COM
    if(digit == 0) pc_pins |= GPIO_Pin_0;      // pin0 = COM0
    else if(digit == 1) pc_pins |= GPIO_Pin_5; // pin5 = COM1  
    else if(digit == 2) pc_pins |= GPIO_Pin_6; // pin6 = COM2
    
    // Set segments based on value
    switch(value) {
        case 0: // segments A,B,C,D,E,F
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; // AF,BG,CE,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 1: // segments B,C
            pc_pins |= GPIO_Pin_2 | GPIO_Pin_3; // BG,CE
            break;
        case 2: // segments A,B,D,E,G
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4; // AF,BG,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 3: // segments A,B,C,D,G
            pc_pins |= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; // BG,CE,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 4: // segments B,C,F,G
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // AF,BG,CE
            break;
        case 5: // segments A,C,D,F,G
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4; // AF,CE,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 6: // segments A,C,D,E,F,G
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4; // AF,CE,D
            break;
        case 7: // segments A,B,C
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 8: // all segments
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; // AF,BG,CE,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
        case 9: // segments A,B,C,D,F,G
            pc_pins |= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; // AF,BG,CE,D
            pd_pins |= GPIO_Pin_4; // ABC
            break;
    }
    
    GPIOC->BSHR = pc_pins;
    GPIOD->BSHR = pd_pins;
}

int main(void) {
    SystemInit();
    lcd_init();
    
    uint8_t digit = 0;
    uint8_t counter = 0;
    uint8_t phase = 0;
    
    while(1) {
        show_digit(digit, counter % 10);
        
        // AC drive - invert all signals every few ms
        if(phase++ > 100) {
            phase = 0;
            // Invert all active pins
            GPIOC->OUTDR ^= (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
            GPIOD->OUTDR ^= (GPIO_Pin_0 | GPIO_Pin_1);
        }
        
        for(volatile int i = 0; i < 500; i++); // Display time
        
        digit = (digit + 1) % 3;
        if(digit == 0) counter++;
    }
}