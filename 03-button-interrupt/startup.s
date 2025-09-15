.syntax unified
.cpu cortex-m4
.thumb

.global Reset_Handler

/* Minimal vector table with just what we need */
.section .isr_vector,"a"
vector_table:
    .word 0x20018000                    /* Stack pointer */
    .word Reset_Handler                 /* Reset handler */
    .skip 4 * 38                       /* Skip to IRQ 40 */
    .word EXTI15_10_IRQHandler         /* IRQ 40: EXTI15_10 */

.section .text.Reset_Handler
Reset_Handler:
    bl main
    b .

/* Default interrupt handler */
.section .text.EXTI15_10_IRQHandler
.weak EXTI15_10_IRQHandler
EXTI15_10_IRQHandler:
    /* Toggle LED immediately to show interrupt fired */
    ldr r0, =0x40020014  /* GPIOA_ODR address */
    ldr r1, [r0]         /* Read current ODR value */
    eor r1, r1, #0x20    /* XOR with bit 5 (PA5) */
    str r1, [r0]         /* Write back */
    
    /* Clear EXTI pending bit */
    ldr r0, =0x40013C14  /* EXTI_PR address */
    mov r1, #0x2000      /* Bit 13 */
    str r1, [r0]         /* Clear pending */
    
    bx lr                /* Return from interrupt */