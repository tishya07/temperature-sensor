#include "I2C.h"

extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode
	
//===============================================================================
//                        I2C GPIO Initialization
//===============================================================================
void I2C_GPIO_Init(void) {
	// [TODO] -> Done, need to check
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //8.4.17 enable gpio b clock
	
	//config PB6 as I2C1_SCL
	GPIOB->MODER &=~GPIO_MODER_MODE6; //clear 6
	GPIOB->MODER |= GPIO_MODER_MODE6_1; //set as AF mode (10)
	
	GPIOB->AFR[0] &=~GPIO_AFRL_AFRL6; //clearing
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2; //I2C1_SCL is AF4 0100
	
	//config PB7 as I2C1_SDA
	GPIOB->MODER &=~GPIO_MODER_MODE7; //clear 7
	GPIOB->MODER |= GPIO_MODER_MODE7_1; //set as AF mode (10)
	
	GPIOB->AFR[0] &=~GPIO_AFRL_AFRL7; //clearing
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2; //I2C1_SDA is AF4 0100
	
	//both operate at very high speed
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6; // (11) = high speed set for PB6
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED7; // (11) = high speed set for PB7
	
	//both set to open-drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT6; // set 1 open-drain for PB6
	GPIOB->OTYPER |= GPIO_OTYPER_OT7; // set 1 open-drain for PB7
	
	//both to pull-up
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD6; // clearing
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0; // (01) is pull-up
	
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPD7; // clearing
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_0; // (01) is pull-up
}
	
#define I2C_TIMINGR_PRESC_POS	28
#define I2C_TIMINGR_SCLDEL_POS	20
#define I2C_TIMINGR_SDADEL_POS	16
#define I2C_TIMINGR_SCLH_POS	8
#define I2C_TIMINGR_SCLL_POS	0

//===============================================================================
//                          I2C Initialization
//===============================================================================
void I2C_Initialization(void){
	uint32_t OwnAddr = 0x52;
	
	// [TODO] -> Done, need to check
	
	//STEP 1, REF 8.4
	// enable the I2C1 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; //enable I2C1 clock
	
	//set system clock as clock source for I2C1 in peripherals independent clock config register
	RCC->CCIPR &=~(RCC_CCIPR_I2C1SEL); //clearing clock selection
	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0; // (01) system clock selected as I2C1 clock
	
	//reset I2C1 in the peripheral reset register
	RCC-> APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST; // (1) reset I2C1
	//clear I2C1 in the peripheral reset register
	RCC-> APB1RSTR1 &=~(RCC_APB1RSTR1_I2C1RST); // (0) no effect, clearing
	
	//STEP 2, REF 35.7
	//disable I2C
	I2C1->CR1 &=~(I2C_CR1_PE); // (0) peripheral disable
	//enable analog noise filter
	I2C1->CR1 &=~(I2C_CR1_ANFOFF); // (0) analog noise filter enabled
	//disable digital noise filter
	I2C1->CR1 &=~(I2C_CR1_DNF); // (0000) digital filter disabled
	//enable error interrupts
	I2C1->CR1 |= I2C_CR1_ERRIE; // (1) error detection interrupts enabled
	//enable clock stretching
	I2C1->CR1 &=~(I2C_CR1_NOSTRETCH); // (0) clock stretching enabled
	//set master to operate in 7-bit addressing mode
	I2C1->CR2 &=~(I2C_CR2_ADD10); // (0) master operates in 7 bit addressing mode
	//enable automatic end mode
	I2C1->CR2 |= I2C_CR2_AUTOEND; // (1) automatic end mode: a STOP condition is automatically sent when NBYTES data are transferred
	//enable NACK generation
	I2C1->CR2 |= I2C_CR2_NACK; // (1) a NACK is sent after current received byte
	
	//TIMING REGISTER
	//set PRESC to 7
	I2C1->TIMINGR &=~(I2C_TIMINGR_PRESC); //clearing bits
	I2C1->TIMINGR |= (7UL<<28); //PRESC[3:0] = 28-31
	//set SCLDEL to 9
	I2C1->TIMINGR &=~(I2C_TIMINGR_SCLDEL); //clearing bits
	I2C1->TIMINGR |= (9UL<<20); //SCLDEL[3:0] = 20-23
	//set SDADEL to 12 (11.5 rounded up)
	I2C1->TIMINGR &=~(I2C_TIMINGR_SDADEL); //clearing bits
	I2C1->TIMINGR |= (12UL<<16); //SDADEL[3:0] = 16-19
	//set SCLH to 39
	I2C1->TIMINGR &=~(I2C_TIMINGR_SCLH); //clearing bits
	I2C1->TIMINGR |= (39UL<<8); //SCLH[7:0] = 8-15
	//set SCLL to 46
	I2C1->TIMINGR &=~(I2C_TIMINGR_SCLL); //clearing bits
	I2C1->TIMINGR |= 46UL; //SCLL[7:0] = 0-7
	
	//OWN ADDRESS REGISTERS
	I2C1->OAR1 &=~(I2C_OAR1_OA1EN); // (0) own address 1 disabled
	I2C1->OAR2 &=~(I2C_OAR2_OA2EN); // (0) own address 2 disabled

	//set own address to 7-bit mode
	I2C1->OAR1 &=~(I2C_OAR1_OA1MODE); // (0) own address 1 is a 7-bit address
	
	//write the own address
	I2C1->OAR1 |= (OwnAddr<<1); //52 in binary = 0110100, shift 1 bc of interface address bit
	
	I2C1->OAR1 |= I2C_OAR1_OA1EN; // (1) own address 1 enabled
	
	//enable I2C in control register
	I2C1->CR1 |= I2C_CR1_PE; // (1) peripheral enable
}

//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Secondary address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SECONDARY)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Secondary
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Secondary
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SECONDARY) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		// while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SECONDARY); // 0 = sending data to the secondary, 1 = receiving data from the secondary.
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}
