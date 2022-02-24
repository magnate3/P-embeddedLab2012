# embeddedLab 
homework of http://wiki.csie.ncku.edu.tw/embedded/schedule at section 2012

embedded2013  https://github.com/embedded2013

# os

```
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# uname -a
Linux ubuntu 5.0.0-23-generic #24~18.04.1-Ubuntu SMP Mon Jul 29 16:10:24 UTC 2019 aarch64 aarch64 aarch64 GNU/Linux
```

# qemu
To run the QEMU examples, you need a modifed version of QEMU which contains the
STM32 peripherals.  This can be found at: https://github.com/beckus/qemu_stm32 .

```
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# ~/arm/embeddedLab/qemu_stm32/arm-softmmu/qemu-system-arm -version
QEMU emulator version 2.1.3, Copyright (c) 2003-2008 Fabrice Bellard
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# 
```
# run

```
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# ~/arm/embeddedLab/qemu_stm32/arm-softmmu/qemu-system-arm  -M stm32-p103  -kernel main.bin
VNC server running on `::1:5901'
LED Off
LED Off
LED On
```

# 参考资料
[在樹莓派上開發微型的作業系統(MiniOS for Raspberry Pi)](https://yunlinsong.blogspot.com/2019/05/minios-for-raspberry-pi.html)

[1] https://jsandler18.github.io/

[2] https://github.com/bztsrc/raspi3-tutorial

[3] https://github.com/s-matyukevich/raspberry-pi-os
