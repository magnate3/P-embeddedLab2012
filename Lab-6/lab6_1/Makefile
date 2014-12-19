EXECUTABLE=lab6_1.elf
BIN_IMAGE=lab6_1.bin

LIB_STM=../discoveryF4/libstm/

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-g -O2 -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-ffreestanding -nostdlib

#usb_conf.h
CFLAGS += -DUSE_USB_OTG_FS=1

# to run from FLASH
CFLAGS+=-Wl,-T,stm32_flash.ld

CFLAGS+=-I./

# stm32f4_discovery lib
CFLAGS+=-I$(LIB_STM)/STM32F4xx_StdPeriph_Driver/inc
CFLAGS+=-I$(LIB_STM)/STM32F4xx_StdPeriph_Driver/inc/device_support
CFLAGS+=-I$(LIB_STM)/STM32F4xx_StdPeriph_Driver/inc/core_support

#STM32_USB_Device_Library
CFLAGS+=-I$(LIB_STM)/STM32_USB_Device_Library/Class/cdc/inc
CFLAGS+=-I$(LIB_STM)/STM32_USB_Device_Library/Core/inc

#STM32_USB_OTG_Driver
CFLAGS+=-I$(LIB_STM)/STM32_USB_OTG_Driver/inc

#STM32F4xx_StdPeriph_Driver\inc
CFLAGS+=-I$(LIB_STM)/STM32F4xx_StdPeriph_Driver/inc

#Utilities
CFLAGS+=-I$(LIB_STM)/Utilities/STM32F4-Discovery

all: $(BIN_IMAGE)

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

#stm32f4xx_it.c
$(EXECUTABLE): main.c system_stm32f4xx.c startup_stm32f4xx.s stm32fxxx_it.c \
	usb_bsp.c usbd_desc.c usbd_usr.c usb_core.c usbd_cdc_vcp.c\
	$(LIB_STM)/Utilities/STM32F4-Discovery/stm32f4_discovery.c \
	$(LIB_STM)/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
	$(LIB_STM)/STM32_USB_OTG_Driver/src/usb_dcd.c \
	$(LIB_STM)/STM32_USB_Device_Library/Core/src/usbd_core.c \
	$(LIB_STM)/STM32_USB_Device_Library/Core/src/usbd_req.c \
	$(LIB_STM)/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
	$(LIB_STM)/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c
	$(CC) $(CFLAGS) $^ -o $@  \
		-L$(LIB_STM)/STM32F4xx_StdPeriph_Driver/build -lSTM32F4xx_StdPeriph_Driver \
		-L$(LIB_STM)/STM32F_USB_OTG_Driver/build

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

.PHONY: all clean

