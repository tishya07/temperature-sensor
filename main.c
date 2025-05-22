#include "stm32l476xx.h"
#include "I2C.h"
#include "SysClock.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void) {
	System_Clock_Init(); // System Clock = 80 MHz

	// Initialize UART -- change the argument depending on the part you are working on
	Init_USARTx(2); //1 is bluetooth, 2 is USB
	
	printf("Connected\n");
	
	int i;
	uint8_t SecondaryAddress;
	uint8_t Data_Receive;
	uint8_t Data_Send;
	
	// Initialize I2C
	I2C_GPIO_Init();
	I2C_Initialization();
	
	while(1) {	
		// Determine Secondary Address
		
		// Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
		SecondaryAddress = 0b1001000 << 1; // CHECK -> I think it is this, bc AO in lab manual is 1001000
		
		// [TODO] -> Done, need to test
		//getting temp function

		Data_Send = 0; // command 00h tells the sensor to read the temp
		
		I2C_SendData(I2C1, SecondaryAddress, &Data_Send, 1); //sending command to read temp
		printf("Command sent.\n");
		
		I2C_ReceiveData(I2C1, SecondaryAddress, &Data_Receive, 1); //receiving read temp
		
		//printing temp function to termite
		
		// [TODO] - Print Temperature to Termite
		printf("The temperature is: %d\n", Data_Receive);
		
		
		// Some delay
		for(i = 0; i < 50000; ++i); 
	}
}
