/* PWM signal timer with LED
 
Author:Diego Klish 
*/
#include <stdint.h>

// Clock registers
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR     (*(volatile uint32_t*)0x40023840)

// GPIO registers
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)
#define GPIOA_AFRL      (*(volatile uint32_t*)0x40020020)

// Timer 2 registers
#define TIM2_CR1        (*(volatile uint32_t*)0x40000000)
#define TIM2_CCMR1      (*(volatile uint32_t*)0x40000018)
#define TIM2_CCER       (*(volatile uint32_t*)0x40000020)
#define TIM2_PSC        (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR        (*(volatile uint32_t*)0x4000002C)
#define TIM2_CCR1       (*(volatile uint32_t*)0x40000034)

void delay_cycles(volatile uint32_t cycles) {
    for(volatile uint32_t i = 0; i < cycles; i++);
}

int main(){
    // Enable clocks
    RCC_AHB1ENR |= (1 << 0);   // GPIOA clock
    RCC_APB1ENR |= (1 << 0);   // TIM2 clock
    
    // Configure PA5 for Timer 2 Channel 1 (AF1)
    GPIOA_MODER &= ~(3 << 10);  // Clear PA5 mode
    GPIOA_MODER |= (2 << 10);   // Set to alternate function
    GPIOA_AFRL &= ~(0xF << 20); // Clear AF for PA5
    GPIOA_AFRL |= (0x1 << 20);  // Set AF1 (TIM2_CH1)
    
    // Configure Timer 2 for PWM
    TIM2_PSC = 15;              // Prescaler: 16MHz / 16 = 1MHz
    TIM2_ARR = 999;             // Period: 1MHz / 1000 = 1kHz PWM
    
    // Configure PWM mode on Channel 1
    TIM2_CCMR1 &= ~(0xFF << 0); // Clear channel 1 config
    TIM2_CCMR1 |= (0x60 << 0);  // PWM mode 1 (0110)
    
    // Enable channel 1 output
    TIM2_CCER |= (1 << 0);      // CC1E: Enable channel 1
    
    // Start timer
    TIM2_CR1 |= (1 << 0);       // CEN: Counter enable
    
    uint32_t brightness = 0;
    uint32_t direction = 1;     // 1 = increasing, 0 = decreasing
    
    while(1) {
        // Set PWM duty cycle (0-999 for 0-100% brightness)
        TIM2_CCR1 = brightness;
        
        // Fade in/out
        if(direction) {
            brightness += 10;
            if(brightness >= 999) direction = 0;
        } else {
            brightness -= 10;
            if(brightness <= 0) direction = 1;
        }
        
        // Delay for visible fading effect
        delay_cycles(100000);
    }
    
    return 0;
}