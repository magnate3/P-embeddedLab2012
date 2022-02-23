# slove bugs
![image](https://github.com/magnate3/P-embeddedLab2012/blob/master/pic/lab3_1.png)

# run

```
 ~/arm/embeddedLab/qemu_stm32/arm-softmmu/qemu-system-arm  -M stm32-p103  -kernel main.bin
VNC server running on `::1:5902'
LED Off
LED Off
LED On
LED Off
LED On
LED Off
LED On
^Z
[3]+  Stopped                 ~/arm/embeddedLab/qemu_stm32/arm-softmmu/qemu-system-arm -M stm32-p103 -kernel main.bin


```

![image](https://github.com/magnate3/P-embeddedLab2012/blob/master/pic/lab3_1-run.png)

 
# debug

```
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# gdb main.elf
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "aarch64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from main.elf...done.
(gdb) target remote:1234
Remote debugging using :1234
Reset_Handler () at libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s:67
67        movs  r1, #0
(gdb) b NVIC_Init
Breakpoint 1 at 0x1e88: file libraries/STM32F10x_StdPeriph_Driver/src/misc.c, line 114.
(gdb) c
Continuing.

Breakpoint 1, NVIC_Init (NVIC_InitStruct=0x20004fd4) at libraries/STM32F10x_StdPeriph_Driver/src/misc.c:114
114       uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
(gdb) bt
#0  NVIC_Init (NVIC_InitStruct=0x20004fd4) at libraries/STM32F10x_StdPeriph_Driver/src/misc.c:114
#1  0x00003fc0 in enable_button_interrupts () at stm32_p103.c:70
#2  0x0000433a in main () at main.c:202
(gdb) c
Continuing.

Breakpoint 1, NVIC_Init (NVIC_InitStruct=0x20004fdc) at libraries/STM32F10x_StdPeriph_Driver/src/misc.c:114
114       uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
(gdb) bt
#0  NVIC_Init (NVIC_InitStruct=0x20004fdc) at libraries/STM32F10x_StdPeriph_Driver/src/misc.c:114
#1  0x0000407a in enable_rs232_interrupts () at stm32_p103.c:117
#2  0x00004342 in main () at main.c:205
(gdb) c
Continuing.
^C^Z
[1]+  Stopped                 gdb main.elf
```