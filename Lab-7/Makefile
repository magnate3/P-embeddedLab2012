OPENOCD_BIN ?= /opt/openocd/bin/openocd
CROSS_COMPILE=arm-none-eabi-

all: openocd factorial

openocd:
	$(OPENOCD_BIN) -f stm32f4-discovery.cfg

factorial: factorial.c
	$(CROSS_COMPILE)gcc -v -mcpu=cortex-m3 -mthumb \
		-o factorial factorial.c \
		-T generic-hosted.ld

auto:
	@echo "Remove this line and do your homework!"

clean:
	rm -f factorial
