# CH32V003 AAA Battery Charge Display

A battery voltage monitor with 3-digit 7-segment LCD display using the CH32V003 microcontroller.

## Features
- Real-time battery voltage monitoring
- 3-digit 7-segment LCD display
- UART output for debugging
- Exponential moving average filtering

## Hardware Connections

### LCD Display (3-digit, 7-segment)
- **COM0** (Digit 1) → PC0
- **COM1** (Digit 2) → PC1  
- **COM2** (Digit 3) → PC2
- **SEG A** → PC3
- **SEG B** → PC4
- **SEG C** → PC6
- **SEG D** → PC7
- **SEG E** → PD0
- **SEG F** → PD1
- **SEG G** → PD3

### Other Connections
- **Battery ADC** → PD2 (ADC Channel 3)
- **UART TX** → PD5 (9600 baud)

## Display Format
The LCD shows battery voltage in format: **X.YZ** (e.g., 3.65V displays as "365")

## Build & Upload
```bash
pio run -t upload
```
