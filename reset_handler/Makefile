ARMGNU = arm-none-eabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0
COPS = -Wall -g -O0 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m0

all : step001.hex

clean:
	rm -f *.hex
	rm -f *.o
	rm -f *.elf

step001.o : step001.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c step001.c -o step001.o

step001.hex : nrf51x22xxac.ld step001.o
	$(ARMGNU)-ld -o step001.elf -T nrf51x22xxac.ld step001.o
	$(ARMGNU)-objcopy step001.elf step001.hex -O ihex
