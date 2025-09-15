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
#define RCC_APB2ENR     (*(volatile uint32_t*)0x40023844)//EXTI

//GPIO registers 
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)//Pin modes 
#define GPIOA_ODR       (*(volatile uint32_t*)0x40020014)//Ouput state port A
#define GPIOC_MODER     (*(volatile uint32_t*)0x40020800)//Pin modes port C 

//SYSCFG Register
#define SYSCFG_EXTICR4  (*(volatile uint32_t*)0x40013814)//GPIO pins to interrupt lines 

//EXTI Registers 
#define EXTI_IMR        (*(volatile uint32_t*)0x40013C00)//Controls interrupt lines 
#define EXTI_FTSR       (*(volatile uint32_t*)0x40013C0C)//falling edge 
#define EXTI_PR         (*(volatile uint32_t*)0x40013C14)//shows which interrupt clear with 1

//NVIC Register 
#define NVIC_ISER1      (*(volatile uint32_t*)0xE000E104)//core ARM interrupt enable  

//Define our state that can be changed whenever given a button press
volatile int led_pattern = 0; //0 = 1HZ, 1 = 5HZ, 2 = off 

//quick function to send our UART string message to console 
void hardware_delay(void) {
    for(volatile int i = 0; i < 1000; i++) {
        for(volatile int j = 0; j < 10000; j++) {
            // Slow delay
        }
    }
}

void fast_delay(void) {
    for(volatile int i = 0; i < 200; i++) {
        for(volatile int j = 0; j < 1000; j++) {
            // Fast delay
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if(EXTI_PR & (1 << 13)) {
        EXTI_PR |= (1 << 13);  // Clear pending bit
        led_pattern = (led_pattern + 1) % 3;  // Cycle states
    }
}

void setup(){
    //first we need to enable all our clocks 
    RCC_AHB1ENR |= (1 << 0) | (1 << 2);// GPIOA, GPIOC turn on  
    RCC_APB2ENR |= (1 << 14);// SYSCFG turn on 

    //configure LED for pin 5 
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |= (1 << 10);
 
    // Configure button 
    GPIOC_MODER &= ~(3 << 26);//sets PC13 as a input 

    //Configure NVIC Register 
    SYSCFG_EXTICR4 &= ~(0xF << 4); 
    SYSCFG_EXTICR4 |= (0x2 << 4);

    // Configure EXTI13 
    EXTI_IMR |= (1 << 13);     // Enable interrupt mask
    EXTI_FTSR |= (1 << 13);    // Set falling edge trigger

    //Enable EXTI15_10 in NVIC
    NVIC_ISER1 |= (1 << 8);
     
}


int main(){
    setup();
    while(1) {    //infinite loop 
        if(led_pattern == 0) {
            GPIOA_ODR |= (1 << 5);   // LED ON
            hardware_delay();
            GPIOA_ODR &= ~(1 << 5);  // LED OFF
            hardware_delay();
        }
        else if(led_pattern == 1){
            GPIOA_ODR |= (1 << 5);   // LED ON
            fast_delay();
            GPIOA_ODR &= ~(1 << 5);  // LED OFF
            fast_delay();
        }
        else{
            GPIOA_ODR &= ~(1 << 5);  // LED OFF
            hardware_delay();
        }
                
    }
    return 0;
}



