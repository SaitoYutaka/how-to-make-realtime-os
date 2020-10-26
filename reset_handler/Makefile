ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0
COPS = -Wall -g -O0 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m0

all : step001.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list

#---------------------------------

# flash.o : flash.s
# 	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

step001.o : step001.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c step001.c -o step001.o

step001.bin : nrf51x22xxac.ld step001.o
	$(ARMGNU)-ld -o step001.elf -T nrf51x22xxac.ld step001.o
	$(ARMGNU)-objdump -D step001.elf > step001.list
	$(ARMGNU)-objcopy step001.elf step001.hex -O ihex
