#include "ch32v00x.h"

int main(void) {
    SystemInit();
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    while(1) {
        GPIOC->BSHR = GPIO_Pin_0;  // HIGH
        for(volatile int i = 0; i < 1000; i++);  // ~5ms
        
        GPIOC->BCR = GPIO_Pin_0;   // LOW
        for(volatile int i = 0; i < 1000; i++);  // ~5ms
    }
}