/* Project to better understand the UART protocol 
Get better grasp of register manipulation and how to apply bit manipulation patterns. In this program after defining
all of our clock and registers we enable the clocks and use our knowledge of the UART protocol to transmit 
data and see UART communication in real time.
Author:Diego Klish 
*/
#include <stdint.h>//main library 

// register for our clock and GPIO registers 
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR     (*(volatile uint32_t*)0x40023840)//we have to enable clocks for both busses 
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)
#define GPIOA_AFRL      (*(volatile uint32_t*)0x40020020)  

// UART2 registers
#define USART2_SR       (*(volatile uint32_t*)0x40004400)  //Status register
#define USART2_DR       (*(volatile uint32_t*)0x40004404)  //Data register
#define USART2_BRR      (*(volatile uint32_t*)0x40004408)  //Baud rate register
#define USART2_CR1      (*(volatile uint32_t*)0x4000440C)  //Control register 1

void uart_init(void) {
    // Step 1:Enable clocks
    RCC_AHB1ENR |= (1 << 0);   //Enable GPIOA clock
    RCC_APB1ENR |= (1 << 17);  //Enable USART2 clock (bit 17)
    
    //Step 2:Configure PA2, PA3 for UART2
    //Set PA2, PA3 to alternate function mode (10)
    GPIOA_MODER &= ~(0xF << 4);//Clear bits for PA2, PA3
    GPIOA_MODER |= (0xA << 4);//Set PA2, PA3 to alternate function (10)
    
    // Set alternate function to AF7 (UART2)
    GPIOA_AFRL &= ~(0xFF << 8);// Clear AF for PA2, PA3
    GPIOA_AFRL |= (0x77 << 8);// Set AF7 for PA2, PA3
    
    // Step 3: Configure UART2
    // Baud rate: 9600 @ 16MHz system clock
    // BRR = 16,000,000 / 9600 = 1666 (0x682)
    USART2_BRR = 0x682;
    
    // Step 4: Enable UART2
    USART2_CR1 |= (1 << 13);  // UE: UART Enable
    USART2_CR1 |= (1 << 3);   // TE: Transmitter Enable
    USART2_CR1 |= (1 << 2);   // RE: Receiver Enable
}

void uart_send_char(char c) {
    // Wait for transmit data register empty (TXE)
    while(!(USART2_SR & (1 << 7)));
    
    // Send character
    USART2_DR = c;
}

void uart_send_string(const char* str) {
    while(*str) {
        uart_send_char(*str++);
    }
}

int main(void) {
    // Initialize UART
    uart_init();
    
    // Send greeting messages
    uart_send_string("STM32F401 Initalized\r\n");
    
    int counter = 0;
    while(1) {
        // Send periodic messages
        uart_send_string("Counter: ");
        
        // Simple integer to string conversion
        char buffer[10];
        int temp = counter;
        int i = 0;
        
        // Convert number to string (simple method)
        if(temp == 0) {
            buffer[i++] = '0';
        } else {
            if(temp >= 1000) { buffer[i++] = '0' + (temp/1000); temp %= 1000; }
            if(temp >= 100)  { buffer[i++] = '0' + (temp/100);  temp %= 100; }
            if(temp >= 10)   { buffer[i++] = '0' + (temp/10);   temp %= 10; }
            if(temp >= 1)    { buffer[i++] = '0' + temp; }
        }
        buffer[i] = '\0';
        
        uart_send_string(buffer);
        uart_send_string("\r\n");
        
        counter++;
        
        // Simple delay
        for(volatile int j = 0; j < 1600000; j++);
    }
    
    return 0;
}