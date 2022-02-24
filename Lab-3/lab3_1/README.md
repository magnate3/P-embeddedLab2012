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

## startup 

在Cortex M使用FreeRTOS时，
经常会应为start_xxx.s与port.c、portasm.s中的中断函数名不同困惑：
是在start_xxx.s修改为：vPortSVCHandler/xPortPendSVHandler/xPortSysTickHandler？
还是在port.c、portasm.s 修改为SVC_Handler/PendSV_Handler/SysTick_Handler？不管怎么样，都要修改文件，造成移植不便。
为了解决这个不便，可以在FreeRTOSConfig.h文件中增加下列几行：
```
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
```
即修改配置文件，可避免修改芯片厂商提供的库文件和FreeRTOS移植接口文件

### Reset_Handler

```
(gdb) b Reset_Handler
Breakpoint 1 at 0x4718: file libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s, line 67.
(gdb) b xTaskCreate
Function "xTaskCreate" not defined.
Make breakpoint pending on future shared library load? (y or [n]) n
(gdb) b vTaskStartScheduler
Breakpoint 2 at 0x335e: file libraries/FreeRTOS/tasks.c, line 1071.
(gdb) c
The program is not being run.
(gdb) target remote:1234
Remote debugging using :1234
Reset_Handler () at libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s:67
67        movs  r1, #0
(gdb) bt
#0  Reset_Handler () at libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s:67
(gdb) c
Continuing.

```
### xTaskGenericCreate
```
(gdb) bt
#0  xTaskGenericCreate (pxTaskCode=0x38e5 <prvIdleTask>, pcName=0x4760 "IDLE", usStackDepth=128, pvParameters=0x0 <g_pfnVectors>, uxPriority=0, pxCreatedTask=0x0 <g_pfnVectors>, puxStackBuffer=0x0 <g_pfnVectors>, xRegions=0x0 <g_pfnVectors>)
    at libraries/FreeRTOS/tasks.c:406
#1  0x0000337a in vTaskStartScheduler () at libraries/FreeRTOS/tasks.c:1071
#2  0x00004426 in main () at main.c:243
```

### pxPortInitialiseStack

```
(gdb) b pxPortInitialiseStack
Note: breakpoint 4 also set at pc 0x3c9c.
Breakpoint 5 at 0x3c9c: file libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c, line 131.
(gdb) c
Continuing.

Breakpoint 4, pxPortInitialiseStack (pxTopOfStack=0x20003248 <xHeap+12440>, pxCode=0x38e5 <prvIdleTask>, pvParameters=0x0 <g_pfnVectors>) at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:131
131             pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
(gdb) bt
#0  pxPortInitialiseStack (pxTopOfStack=0x20003248 <xHeap+12440>, pxCode=0x38e5 <prvIdleTask>, pvParameters=0x0 <g_pfnVectors>) at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:131
#1  0x00002d86 in xTaskGenericCreate (pxTaskCode=0x38e5 <prvIdleTask>, pcName=0x4760 "IDLE", usStackDepth=128, pvParameters=0x0 <g_pfnVectors>, uxPriority=0, pxCreatedTask=0x0 <g_pfnVectors>, puxStackBuffer=0x0 <g_pfnVectors>, 
    xRegions=0x0 <g_pfnVectors>) at libraries/FreeRTOS/tasks.c:465
#2  0x0000337a in vTaskStartScheduler () at libraries/FreeRTOS/tasks.c:1071
#3  0x00004426 in main () at main.c:243
(gdb) 
```

```
(gdb) list
127     portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
128     {
129             /* Simulate the stack frame as it would be created by a context switch
130             interrupt. */
131             pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
132             *pxTopOfStack = portINITIAL_XPSR;       /* xPSR */
133             pxTopOfStack--;
134             *pxTopOfStack = ( portSTACK_TYPE ) pxCode;      /* PC */
135             pxTopOfStack--;
136             *pxTopOfStack = 0;      /* LR */
(gdb) 
```
```

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
        /* Simulate the stack frame as it would be created by a context switch
        interrupt. */
        pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
        *pxTopOfStack = portINITIAL_XPSR;       /* xPSR */
        pxTopOfStack--;
        *pxTopOfStack = ( portSTACK_TYPE ) pxCode;      /* PC */
        pxTopOfStack--;
        *pxTopOfStack = 0;      /* LR */
        pxTopOfStack -= 5;      /* R12, R3, R2 and R1. */
        *pxTopOfStack = ( portSTACK_TYPE ) pvParameters;        /* R0 */
        pxTopOfStack -= 8;      /* R11, R10, R9, R8, R7, R6, R5 and R4. */

        return pxTopOfStack;
}
```

### xPortStartScheduler
```
(gdb) b xPortStartScheduler
Breakpoint 6 at 0x3d18: file libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c, line 184.
(gdb) c
Continuing.

Breakpoint 6, xPortStartScheduler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:184
184             *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
(gdb) bt
#0  xPortStartScheduler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:184
#1  0x0000339a in vTaskStartScheduler () at libraries/FreeRTOS/tasks.c:1106
#2  0x00004426 in main () at main.c:243
(gdb) 

Breakpoint 6, xPortStartScheduler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:184
184             *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
(gdb) bt
#0  xPortStartScheduler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:184
#1  0x0000339a in vTaskStartScheduler () at libraries/FreeRTOS/tasks.c:1106
#2  0x00004426 in main () at main.c:243
(gdb) s
185             *(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;
(gdb) list
180      */
181     portBASE_TYPE xPortStartScheduler( void )
182     {
183             /* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
184             *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
185             *(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;
186
187             /* Start the timer that generates the tick ISR.  Interrupts are disabled
188             here already. */
189             prvSetupTimerInterrupt();
(gdb) 
```




```

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
        /* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
        *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
        *(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

        /* Start the timer that generates the tick ISR.  Interrupts are disabled
        here already. */
        prvSetupTimerInterrupt();

        /* Initialise the critical nesting count ready for the first task. */
        uxCriticalNesting = 0;

        /* Start the first task. */
        prvPortStartFirstTask();

        /* Should not get here! */
        return 0;
}
```

### prvPortStartFirstTask

```
(gdb) bt
#0  prvPortStartFirstTask () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:166
#1  0x00003d3e in xPortStartScheduler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:195
#2  0x0000339a in vTaskStartScheduler () at libraries/FreeRTOS/tasks.c:1106
#3  0x00004426 in main () at main.c:243
(gdb) list
161     }
162     /*-----------------------------------------------------------*/
163
164     static void prvPortStartFirstTask( void )
165     {
166             __asm volatile(
167                                             " ldr r0, =0xE000ED08   \n" /* Use the NVIC offset register to locate the stack. */
168                                             " ldr r0, [r0]                  \n"
169                                             " ldr r0, [r0]                  \n"
170                                             " msr msp, r0                   \n" /* Set the msp back to the start of the stack. */
(gdb) 
```

```
static void prvPortStartFirstTask( void )
{
        __asm volatile(
                                        " ldr r0, =0xE000ED08   \n" /* Use the NVIC offset register to locate the stack. */
                                        " ldr r0, [r0]                  \n"
                                        " ldr r0, [r0]                  \n"
                                        " msr msp, r0                   \n" /* Set the msp back to the start of the stack. */
                                        " cpsie i                               \n" /* Globally enable interrupts. */
                                        " svc 0                                 \n" /* System call to start first task. */
                                        " nop                                   \n"
                                );
}
```

### SVC_Handler

```
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# grep SVC_Handler -rn * | grep define
FreeRTOSConfig.h:59:#define vPortSVCHandler SVC_Handler
root@ubuntu:~/arm/embeddedLab/Lab-3/lab3_1# 
```

```
(gdb) b SVC_Handler
Breakpoint 8 at 0x3ce0: file libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c, line 147.
(gdb) b PendSV_Handler
Breakpoint 9 at 0x3dc0: file libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c, line 237.
(gdb) b USART1_IRQHandler
Breakpoint 10 at 0x475c: file libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s, line 110.
(gdb) b USART2_IRQHandler
Breakpoint 11 at 0x40a2: file main.c, line 35.
(gdb) b USART3_IRQHandler
Note: breakpoint 10 also set at pc 0x475c.
Breakpoint 12 at 0x475c: file libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s, line 110.
(gdb) c
Continuing.

Breakpoint 8, SVC_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:147
147             __asm volatile (
(gdb) bt
#0  SVC_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:147
#1  0xfffffff8 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) list
142     }
143     /*-----------------------------------------------------------*/
144
145     void vPortSVCHandler( void )
146     {
147             __asm volatile (
148                                             "       ldr     r3, pxCurrentTCBConst2          \n" /* Restore the context. */
149                                             "       ldr r1, [r3]                                    \n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
150                                             "       ldr r0, [r1]                                    \n" /* The first item in pxCurrentTCB is the task top of stack. */
151                                             "       ldmia r0!, {r4-r11}                             \n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
(gdb) c
```

### PendSV_Handler

```
Breakpoint 9, PendSV_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:237
237             __asm volatile
(gdb) bt
#0  PendSV_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:237
#1  0xfffffffc in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) list
232
233     void xPortPendSVHandler( void )
234     {
235             /* This is a naked function. */
236
237             __asm volatile
238             (
239             "       mrs r0, psp                                                     \n"
240             "                                                                               \n"
241             "       ldr     r3, pxCurrentTCBConst                   \n" /* Get the location of the current TCB. */
(gdb) c
Continuing.

```


### SysTick_Handler

```
Breakpoint 13, SysTick_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:274
274                     *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
(gdb) bt
#0  SysTick_Handler () at libraries/FreeRTOS/portable/GCC/ARM_CM3/port.c:274
#1  0xfffffffc in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) list
269     {
270     unsigned long ulDummy;
271
272             /* If using preemption, also force a context switch. */
273             #if configUSE_PREEMPTION == 1
274                     *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
275             #endif
276
277             ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
278             {
(gdb) c
Continuing.
```

###  USART2_IRQHandler
```
(gdb) b USART2_IRQHandler
Breakpoint 11 at 0x40a2: file main.c, line 35.
```

```
(gdb) bt
#0  USART2_IRQHandler () at main.c:35
#1  0xfffffffc in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) list
30      {
31              static signed portBASE_TYPE xHigherPriorityTaskWoken;
32              serial_ch_msg rx_msg;
33
34              /* If this interrupt is for a transmit... */
35              if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
36                      /* "give" the serial_tx_wait_sem semaphore to notfiy processes
37                       * that the buffer has a spot free for the next byte.
38                       */
39                      xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);
(gdb) 
```