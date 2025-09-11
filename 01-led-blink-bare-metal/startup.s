.syntax unified
.cpu cortex-m4
.thumb

.global Reset_Handler

/* Vector table */
.section .isr_vector,"a"
.word 0x20018000    /* Stack pointer (end of 96K SRAM) */
.word Reset_Handler /* Reset handler address */

/* Reset handler - what runs when STM32 starts */
.section .text.Reset_Handler
Reset_Handler:
    bl main         /* Call your main() function */
    b .            /* Infinite loop if main returns */
