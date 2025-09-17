/* feedback project to familarize myself with SPI

Author:Diego Klish 
*/

#include <stdint.h>//main library 
#include <stm32f4xx.h>//our STM library 

void spi_init_loop(){
    
// two clock enables 
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // GPIO power
RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

//two GPIO configurations for three pins 
//clear pins 
GPIOA->MODER &= ~((3 << 5*2) | (3 << 6*2) | (3 << 7*2));
//set pins to 10
GPIOA->MODER |= ((2 << 5*2) | (2 << 6*2) | (2 << 7*2));
//clear pins 
GPIOA->AFR[0] &= ~((15 << 5*4) | (15 << 6*4) | (15 << 7*4));
//set pins to 0101
GPIOA-> AFR[0] |= ((5 << 5*4) | (5 << 6*4) | (5 << 7*4));

//SPI configurations
//clear control register 
SPI1->CR1 = 0;

SPI1->CR1 |= SPI_CR1_MSTR;    // Bit 2: Master mode (we control clock)
SPI1->CR1 |= SPI_CR1_BR_2;    // Bit 5: Baud rate /32 (slow speed)
SPI1->CR1 |= SPI_CR1_SSM;     // Bit 9: Software slave select
SPI1->CR1 |= SPI_CR1_SSI;     // Bit 8: Internal slave select high
SPI1->CR1 |= SPI_CR1_SPE;     // Bit 6: Enable SPI peripheral

}

void init_led(){
    GPIOA->MODER &= ~(3 << 8*2);  // Clear PA8 mode
    GPIOA->MODER |= (1 << 8*2);   // Set PA8 as output

}

void spi_test(){
    //byte well send from master to slave and back 
    uint8_t test_byte = 0x42;

    SPI1->DR = test_byte;

    //waiting while SPI_SR_BSY flag is still 1 
    while (SPI1->SR & SPI_SR_BSY);

    //received data byte 
    uint8_t received = SPI1->DR;

    //check loopback 
    if (received == test_byte) {
        GPIOC->ODR &= ~(1 << 13); 
    }
    else{
        
    }
}


int main(){
    //intial function to initalize everything 
    spi_init_loop();
    init_led();
    //run test to see if loopback works 
    spi_test();

    while(1){
    
    }
    return 0;
}