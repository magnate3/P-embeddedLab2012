EXECUTABLE=main.elf
BIN_IMAGE=main.bin

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OPENOCD_BIN ?= /opt/openocd/bin/openocd

CFLAGS=-g -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-ffreestanding


# to run from FLASH
CFLAGS+=-Wl,-T,stm32_flash.ld

CFLAGS+=-I./

# stm32f4_discovery lib
CFLAGS+=-I../../Lab-6/discoveryF4/libstm/STM32F4xx_StdPeriph_Driver/inc
CFLAGS+=-I../../Lab-6/discoveryF4/libstm/STM32F4xx_StdPeriph_Driver/inc/device_support
CFLAGS+=-I../../Lab-6/discoveryF4/libstm/STM32F4xx_StdPeriph_Driver/inc/core_support


#STM32F4xx_StdPeriph_Driver\inc
CFLAGS+=-I../../Lab-6/discoveryF4/libstm/STM32F4xx_StdPeriph_Driver/inc

#Utilities
CFLAGS+=-I../../Lab-6/discoveryF4/libstm/Utilities/STM32F4-Discovery

all: openocd $(BIN_IMAGE)

openocd:
	$(OPENOCD_BIN) -f board/stm32f4discovery.cfg & echo "$$!" > "openocd.pid"

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

$(EXECUTABLE): main.c system_stm32f4xx.c startup_stm32f4xx.s stm32f4xx_it.c syscalls.c\
	../../Lab-6/discoveryF4/libstm/Utilities/STM32F4-Discovery/stm32f4_discovery.c 
	$(CC) $(CFLAGS) $^ -o $@  -L../../Lab-6/discoveryF4/libstm/STM32F4xx_StdPeriph_Driver/build \
		-lSTM32F4xx_StdPeriph_Driver 

auto:
	@echo "Remove this line and do your homework!"
	arm-none-eabi-gdb -x test.script
	kill -9 `cat openocd.pid`
	rm -f openocd.pid

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

flash:
	st-flash write $(BIN_IMAGE) 0x8000000

.PHONY: all clean
