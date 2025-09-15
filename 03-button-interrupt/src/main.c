/* Expirementing with Interrupt via Button 
-Need UART now for sending messages, GPIOC for connecting to button pin, SYSCFG for connecting
button pin to interrupt and GPIOA for LED to allow us to see the state changes
-Our LED in state 0 should start blinking at 1HZ before a button press and a UART message is sent
which then takes us into state 1 which is blinking at 5HZ before the final state state 2 which 
will turn off the LED until the button is pushed again and were back to state 0
-This will show me how to use the interrupt register 
Author:Diego Klish 
*/
#include <stdint.h>//just our main library 

//first we'll define our clock control registers 
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830)//for GPIOA (LED)
#define RCC_APB1ENR     (*(volatile uint32_t*)0x40023840)//UART 
#define RCC_APB2ENR     (*(volatile uint32_t*)0x40023844)//EXTI

//GPIO registers 
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)//Pin modes 
#define GPIOA_ODR       (*(volatile uint32_t*)0x40020014)//Ouput state port A 
#define GPIOA_AFRL      (*(volatile uint32_t*)0x40020020)//function port A
#define GPIOC_MODER     (*(volatile uint32_t*)0x40020800)//Pin modes port C 

//SYSCFG Register
#define SYSCFG_EXTICR4  (*(volatile uint32_t*)0x40013814)//GPIO pins to interrupt lines 

//EXTI Registers 
#define EXTI_IMR        (*(volatile uint32_t*)0x40013C00)//Controls interrupt lines 
#define EXTI_FTSR       (*(volatile uint32_t*)0x40013C0C)//falling edge 
#define EXTI_PR         (*(volatile uint32_t*)0x40013C14)//shows which interrupt clear with 1

//NVIC Register 
#define NVIC_ISER1      (*(volatile uint32_t*)0xE000E104)//core ARM interrupt enable 

//UART Registers 
#define USART2_SR       (*(volatile uint32_t*)0x40004400)//Status 
#define USART2_DR       (*(volatile uint32_t*)0x40004404)//Data 
#define USART2_BRR      (*(volatile uint32_t*)0x40004408)//Baud rate
#define USART2_CR1      (*(volatile uint32_t*)0x4000440C)//Control 

//Define our state that can be changed whenever given a button press
volatile int led_pattern = 0; //0 = 1HZ, 1 = 5HZ, 2 = off 

void setup(){
    //first we need to enable all our clocks 
    RCC_AHB1ENR |= (1 << 0) | (1 << 2);// GPIOA, GPIOC turn on 
    RCC_APB1ENR |= (1 << 17);// USART2 turn on 
    RCC_APB2ENR |= (1 << 14);// SYSCFG turn on 

    //configure LED for pin 5 
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |= (1 << 10);

    //configure PA2, PA3 for UART
    GPIOA_MODER &= ~(0xF << 4);//clears mode bits 
    GPIOA_MODER |= (0xA << 4);//sets to alternate function 0101(A)
    GPIOA_AFRL &= ~(0xFF << 8);//Clears the alternate function
    GPIOA_AFRL |= (0x77 << 8);//Sets alternate function  7

    // Configure UART2
    USART2_BRR = 0x682;
    USART2_CR1 |= (1 << 13) | (1 << 3) | (1 << 2);

    // Configure button 
    GPIOC_MODER &= ~(3 << 26);//sets PC13 as a input 

    //Configure NVIC Register 
    SYSCFG_EXTICR4 &= ~(0xF << 4);//enable input 
    SYSCFG_EXTICR4 |= (0x2 << 4);//falling edge trigger 

    //Enable EXTI15_10 in NVIC
    NVIC_ISER1 |= (1 << 8);
    //uart_send_string("Everything is enabled button is primed"); //Let's user know program is ready 
}

/*quick function to send our UART string message to console 
void uart_send_string(const char* str) {
    while(*str) {
        while(!(USART2_SR & (1 << 7)));
        USART2_DR = *str++;
    }
}    
*/

//delay function 
void delay_ms(volatile uint32_t ms) {
    for(volatile uint32_t i = 0; i < ms * 16000; i++);
}

/*
void EXTI15_10_IRQHandler(void) {
    if(EXTI_PR & (1 << 13)) {
        EXTI_PR |= (1 << 13);  // Clear pending bit
        
        led_pattern = (led_pattern + 1) % 3;  // Cycles states
        
        switch(led_pattern) {
            case 0: uart_send_string("LED: SLOW blink\r\n"); break;
            case 1: uart_send_string("LED: FAST blink\r\n"); break;
            case 2: uart_send_string("LED: OFF\r\n"); break;
        }
    }
}
*/

int main(){
    setup();
    while(1) {    //infinite loop 
        switch(led_pattern) {
            case 0:  // Slow blink (1Hz)
                GPIOA_ODR ^= (1 << 5);
                delay_ms(500);
                break;
                
            case 1:  // Fast blink (5Hz)
                GPIOA_ODR ^= (1 << 5);
                delay_ms(100);
                break;
                
            case 2:  // Off
                GPIOA_ODR &= ~(1 << 5);
                delay_ms(200);
                break;
        }
    }
    return 0;

}