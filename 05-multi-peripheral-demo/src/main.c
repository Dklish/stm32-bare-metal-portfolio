/* Multi-Peripheral
-Combining the various basic peripherals we've been using 
Author:Diego Klish 
*/
#include <stdint.h>

// Clock registers
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR     (*(volatile uint32_t*)0x40023840)

// GPIO registers
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)
#define GPIOA_ODR       (*(volatile uint32_t*)0x40020014)
#define GPIOA_AFRL      (*(volatile uint32_t*)0x40020020)
#define GPIOC_MODER     (*(volatile uint32_t*)0x40020800)
#define GPIOC_IDR       (*(volatile uint32_t*)0x40020810)

// Timer registers
#define TIM2_CR1        (*(volatile uint32_t*)0x40000000)
#define TIM2_CCMR1      (*(volatile uint32_t*)0x40000018)
#define TIM2_CCER       (*(volatile uint32_t*)0x40000020)
#define TIM2_PSC        (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR        (*(volatile uint32_t*)0x4000002C)
#define TIM2_CCR1       (*(volatile uint32_t*)0x40000034)

void delay_cycles(volatile uint32_t cycles) {
    for(volatile uint32_t i = 0; i < cycles; i++);
}

void pwm_init(void) {
    // Configure PA5 for Timer 2 PWM
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |= (2 << 10);   // Alternate function
    GPIOA_AFRL &= ~(0xF << 20);
    GPIOA_AFRL |= (0x1 << 20);  // AF1 (TIM2_CH1)
    
    // Configure Timer 2
    TIM2_PSC = 15;              // 1MHz timer clock
    TIM2_ARR = 999;             // 1kHz PWM frequency
    TIM2_CCMR1 |= (0x60 << 0);  // PWM mode 1
    TIM2_CCER |= (1 << 0);      // Enable output
    TIM2_CR1 |= (1 << 0);       // Start timer
}

void set_brightness(uint32_t percent) {
    // Convert 0-100% to 0-999 timer value
    TIM2_CCR1 = (percent * 999) / 100;
}

int main(){
    // Enable clocks
    RCC_AHB1ENR |= (1 << 0) | (1 << 2);  // GPIOA, GPIOC
    RCC_APB1ENR |= (1 << 0);             // TIM2
    
    // Configure PC13 (button) as input for polling
    GPIOC_MODER &= ~(3 << 26);
    
    // Initialize PWM
    pwm_init();
    
    uint32_t brightness_level = 0;  // 0%, 25%, 50%, 75%, 100%
    uint32_t button_pressed = 0;
    
    while(1) {
        // Button polling with debouncing
        if(!(GPIOC_IDR & (1 << 13))) {  // Button pressed
            if(!button_pressed) {
                button_pressed = 1;
                
                // Cycle brightness levels
                brightness_level = (brightness_level + 1) % 5;
                
                switch(brightness_level) {
                    case 0: set_brightness(0);   break;  // Off
                    case 1: set_brightness(25);  break;  // 25%
                    case 2: set_brightness(50);  break;  // 50%
                    case 3: set_brightness(75);  break;  // 75%
                    case 4: set_brightness(100); break;  // 100%
                }
                
                delay_cycles(500000);  // Debounce delay
            }
        } else {
            button_pressed = 0;  // Button released
        }
        
        delay_cycles(10000);  // Small polling delay
    }
    
    return 0;
}