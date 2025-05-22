# Temperature Sensor using the I2C Serial Protocol

This project demonstrates I2C communication using the STM32L4 Nucleo board to interface with the TC74 digital temperature sensor. Temperature data is read from the sensor and printed to a terminal using UART.

## Features
- I2C communication using I2C1 peripheral
- Real-time temperature reading from TC74 sensor
- Data output to PC terminal via UART
- Periodic temperature polling (1 second delay)

## Usage
1. Connect the TC74 Temperature Sensor:
   - SDA → PB7 (I2C1 SDA)
   - SCL → PB6 (I2C1 SCL)
   - VDD → 3.3V
   - GND → GND
   - Add pull-up resistors (typically 4.7k–10kΩ) on SDA and SCL lines

2. Configure UART to print data to Termite or another terminal.

3. Run the program:
   - The temperature sensor is polled once per second.
   - The received 8-bit temperature value is printed via `printf()` over UART.
