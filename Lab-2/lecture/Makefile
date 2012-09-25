all: example1 factorial example2.o

example1:
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb example1.s -o example1.o
	arm-none-eabi-ld -Ttext 0x0 -Tbss 0x20000000 -o example1.out example1.o

factorial:
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb \
		-o factorial factorial.c \
		-T generic-hosted.ld

example2.o:
	arm-none-eabi-gcc -g -mcpu=cortex-m3 -mthumb -c example2.c 
	arm-none-eabi-objdump -d -S example2.o 

clean:
	rm -f *.o example1.out factorial
