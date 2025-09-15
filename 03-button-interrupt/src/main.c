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

//first well define our clock control registers 
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



