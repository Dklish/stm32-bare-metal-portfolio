CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=cortex-m4 -mthumb -O2 -Wall
LDFLAGS = -T stm32f401.ld -nostdlib

TARGET = main
SOURCES = src/main.c startup.s

all: $(TARGET).bin

$(TARGET).elf: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $< 0x8000000

clean:
	rm -f *.o *.elf *.bin

.PHONY: all flash clean
