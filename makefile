GCC_DEVICE  = attiny85
DUDE_DEVICE = t85
CLOCK       = 1000000            # 1Mhz
PROGRAMMER  = -c usbtiny -P usb  # For using Sparkfun Pocket AVR Programmer
FUSES      = -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m  # settings as taken from http://www.engbedded.com/fusecalc/ and the default settings when I got my ATTiny85. avrdude thinks they're "safe".  I think they need to be changed (e.g. 'divide internal clock by 8???').

AVRDUDE = avrdude $(PROGRAMMER) -p $(DUDE_DEVICE) -e
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(GCC_DEVICE)

all: setup main.hex

setup:
	mkdir -p -- build

clean:
	rm build/*

build/%.o : src/%.c
	$(COMPILE) -c $< -o $@

build/%.o : src/%.S
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

main.elf: build/main.o
	$(COMPILE) -o build/main.elf build/main.o

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex build/main.elf build/main.hex
	avr-size --format=avr --mcu=$(GCC_DEVICE) build/main.elf

disasm: main.elf
	avr-objdump -d build/main.elf

upload: main.hex
	$(AVRDUDE) -U flash:w:build/main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: upload
