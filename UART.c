#include "UART.h"

void UART1_Init(void) {
	// [TODO] -> Done
	//REF 8.4
	// enable the USART1 clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable gpio clock
	
	//select the system clock as the USART1 clock source in the peripheral independent config register
	RCC->CCIPR &=~(RCC_CCIPR_USART1SEL); //clearing clock selection
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0; // (01) system clock selected as USART1 clock
}

void UART2_Init(void) {
	// [TODO] -> Done
	//REF 8.4
	// enable the USART2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	//select the system clock as the USART2 clock source in the peripheral independent clock config register
	RCC->CCIPR &=~(RCC_CCIPR_USART2SEL); //clearing clock selection
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; // (01) system clock selected as USART2 clock
}

void UART1_GPIO_Init(void) {
	// [TODO] -> Done
	//REF 9.4
	//config PB6 as USART1_TX
	GPIOB->MODER &=~GPIO_MODER_MODE6; //clear 6
	GPIOB->MODER |= GPIO_MODER_MODE6_1; //set as AF mode (10)
	
	GPIOB->AFR[0] &=~GPIO_AFRL_AFRL6; //clearing
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2); //USART1_TX is AF7 = 0111
	
	//config PB7 as USART1_RX
	GPIOB->MODER &=~GPIO_MODER_MODE7; //clear 7
	GPIOB->MODER |= GPIO_MODER_MODE7_1; //set as AF mode (10)
	
	GPIOB->AFR[0] &=~GPIO_AFRL_AFRL7; //clearing
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_2); //USART1_RX is AF7 = 0111
	
	//both operate at very high speed
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6; // (11) = high speed set for PB6
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED7; // (11) = high speed set for PB7
	
	//both set to push/pull
	GPIOB->OTYPER &=~GPIO_OTYPER_OT6; // set 0 push pull for PB6
	GPIOB->OTYPER &=~GPIO_OTYPER_OT7; // set 0 push pull for PB7
	
	//both to pull-up
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD6; // clearing
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0; // (01) is pull-up
	
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD7; // clearing
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_0; // (01) is pull-up
	
}

void UART2_GPIO_Init(void) {
	// [TODO] -> Done
	//REF 9.4
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//config PA2 as UART transmitter
	GPIOA->MODER &=~GPIO_MODER_MODE2; //clear 2
	GPIOA->MODER |= GPIO_MODER_MODE2_1; //set as AF mode (10)
	
	GPIOA->AFR[0] &=~GPIO_AFRL_AFRL2; //clearing
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2); //USART2_TX is AF7 = 0111
	
	//config PA3 as UART receiver
	
	GPIOA->MODER &=~GPIO_MODER_MODE3; //clear 3
	GPIOA->MODER |= GPIO_MODER_MODE3_1; //set as AF mode (10)
	
	GPIOA->AFR[0] &=~GPIO_AFRL_AFRL3; //clearing
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2); //USART2_RX is AF7 = 0111
	
	//both operate at very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2; // (11) = high speed set for PA2
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED3; // (11) = high speed set for PA3
	
	//both set to push/pull
	GPIOA->OTYPER &=~GPIO_OTYPER_OT2; // set 0 push pull for PA2
	GPIOA->OTYPER &=~GPIO_OTYPER_OT3; // set 0 push pull for PA3
	
	//config both to use pull-up for i/o
	GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD2; // clearing
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0; // (01) is pull-up
	
	GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD3; // clearing
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0; // (01) is pull-up
	
}

void USART_Init(USART_TypeDef* USARTx) {
	// [TODO] -> Done
	//use USARTx->[register] !!!
	//REF 36.8
	//disable USART
	USARTx->CR1 &=~USART_CR1_UE; // (0) USART prescaler and outputs disabled
	//in control register set word length to 8 bits
	USARTx->CR1 &=~USART_CR1_M; // M[1:0] = 00 Start bit, 8 data bits, n stop bits
	//in control register set oversampling mode to oversample by 16
	USARTx->CR1 &=~USART_CR1_OVER8; // (1) oversampling by 16
	//in control register set the number of stop bits to 1
	USARTx->CR2 &=~USART_CR2_STOP; // (00) = 1 stop bit
	//set baud rate to 9600, write into USARTx_BRR 8333
	//baud_rate = f_clk / (usartdiv = the variable to input into USARTx_BRR)
	USARTx->BRR = 8333;
	//in control register, enable both the transmitter and receiver
	USARTx->CR1 |= USART_CR1_TE; // (1) = transmitter enabled
	USARTx->CR1 |= USART_CR1_RE; // (1) = receiver enabled
	//enable USART in the control registers
	USARTx->CR1 |= USART_CR1_UE; // (1) USART enabled
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300); //comment out for part 2
		//USART_Delay(40000); //comment out for part 1
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
