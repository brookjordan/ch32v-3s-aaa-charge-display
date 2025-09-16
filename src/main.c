#include "ch32v00x.h"
#include <stdio.h>

// Configuration variables
#define UART_BAUD_RATE 9600
#define ADC_SAMPLE_RATE_HZ 10
#define PRINT_INTERVAL_SAMPLES 10
#define EMA_SHIFT_BITS 6  // 1/64 smoothing
#define ADC_SAMPLE_TIME ADC_SampleTime_43Cycles

// Hardware pin assignments
#define UART_TX_PIN GPIO_Pin_5
#define ADC_GPIO_PIN GPIO_Pin_2
#define ADC_CHANNEL ADC_Channel_3

// Calibration points
#define RAW_A 700
#define VOLTAGE_A 3690
#define RAW_B 580
#define VOLTAGE_B 3120

// Buffer size
#define ADC_BUFFER_SIZE 100

void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms; i++) {
        for(volatile uint32_t j = 0; j < 8000; j++);
    }
}

void uart_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);
    
    // PD5 as UART1 TX
    GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    USART_InitStructure.USART_BaudRate = UART_BAUD_RATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    USART_Cmd(USART1, ENABLE);
}

void uart_putc(char c) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, c);
}

void uart_puts(const char* str) {
    while(*str) {
        uart_putc(*str++);
    }
}

void adc_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_ADC1, ENABLE);
    
    // PD2 as analog input
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // ADC config
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_Cmd(ADC1, ENABLE);
    
    // Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint8_t buffer_index = 0;
uint8_t buffer_full = 0;

uint16_t adc_read_single(void) {
    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SAMPLE_TIME);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

void add_adc_sample(uint16_t sample) {
    adc_buffer[buffer_index] = sample;
    buffer_index++;
    if(buffer_index >= ADC_BUFFER_SIZE) {
        buffer_index = 0;
        buffer_full = 1;
    }
}

uint32_t ema_value = 0;
uint8_t ema_initialized = 0;

uint16_t get_adc_smooth(uint16_t new_sample) {
    if(!ema_initialized) {
        ema_value = new_sample << 8; // Scale up for precision
        ema_initialized = 1;
        return new_sample;
    }
    
    // EMA with configurable smoothing
    ema_value = ema_value - (ema_value >> EMA_SHIFT_BITS) + (new_sample << (EMA_SHIFT_BITS - 4));
    return ema_value >> 8;
}

void print_number(uint32_t num) {
    char buf[12];
    int i = 0;
    
    if(num == 0) {
        uart_putc('0');
        return;
    }
    
    while(num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while(i > 0) {
        uart_putc(buf[--i]);
    }
}

int main(void) {
    SystemInit();
    
    uart_init();
    adc_init();
    
    uart_puts("CH32V003 Battery Monitor\r\n");
    
    uint32_t sample_counter = 0;
    
    while(1) {
        // Take ADC sample at 10Hz
        uint16_t sample = adc_read_single();
        uint16_t adc_smooth = get_adc_smooth(sample);
        sample_counter++;
        
        // Print at configured interval
        if(sample_counter % PRINT_INTERVAL_SAMPLES == 0) {
            
            // Linear interpolation using calibration points
            uint32_t battery_mv = VOLTAGE_B + ((adc_smooth - RAW_B) * (VOLTAGE_A - VOLTAGE_B)) / (RAW_A - RAW_B);
            
            uart_puts("Raw=");
            print_number(sample);
            uart_puts(" Smooth=");
            print_number(adc_smooth);
            uart_puts(" Battery=");
            print_number(battery_mv);
            uart_puts("mV\r\n");
        }
        
        delay_ms(1000 / ADC_SAMPLE_RATE_HZ); // Configurable sample rate
    }
}