#include <stdint.h>

// STM32F401RE Register definitions
#define RCC_AHB1ENR     (*(volatile uint32_t*)0x40023830)
#define GPIOA_MODER     (*(volatile uint32_t*)0x40020000)
#define GPIOA_ODR       (*(volatile uint32_t*)0x40020014)

int main(void) {
    // Enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);
    
    // Configure PA5 as output
    GPIOA_MODER &= ~(3 << 10);  // Clear bits
    GPIOA_MODER |=  (1 << 10);  // Set as output
    
    // Turn LED ON and leave it on
    GPIOA_ODR |= (1 << 5);
    
    // Infinite loop
    while(1) {
        // Do nothing - LED should stay on
    }
    
    return 0;
}
