#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

//#include <stdio.h>
#include <setjmp.h>
//#include <stdlib.h>

//{{{
#define STACK_SIZE 4096

/*
 * Change SP prior to calling setjmp so that longjmp will
 * start the routine with 'stackptr'.
 */
#define SAVE_STACK_POINTER_ASM(savedstack, stackptr) \
	"mov sp, %[savedstack]\n" /* savedstack <- SP */ \
	"mov %[stackptr], sp"    /* SP <- stackptr */

#define SAVE_STACK_POINTER(savedstack, stackptr) \
do { \
	asm volatile ( SAVE_STACK_POINTER_ASM(savedstack, stackptr) \
	: [savedstack] "=r" (savedstack): [stackptr] "r" (stackptr) ); \
} while (0)

/* Restore "normal" stack prior to return */
#define RESTORE_STACK_ASM(savedstack) \
	"mov %[savedstack], sp"

#define RESTORE_STACK(savedstack) \
do { \
	asm volatile ( RESTORE_STACK_ASM(savedstack) \
	: : [savedstack] "r" (savedstack)); \
} while (0)	
//}}}

int mask_req = 0;
char in_char;
int rx_char = 0;

jmp_buf uart_rx_buf;
jmp_buf uart_tx_buf;

char uart_rx_stack[STACK_SIZE];
char uart_tx_stack[STACK_SIZE];

void init_button(void){
//{{{	
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO A clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure the button pin as a floating input. */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
//}}}	
}

void init_rs232(void){
//{{{	
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable peripheral clocks. */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART2 Rx pin as floating input. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx as alternate function push-pull. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure the USART2 */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
//}}}	
}

void enable_rs232(void){
    /* Enable the RS232 port. */
    USART_Cmd(USART2, ENABLE);
}
void enable_button_interrupts(void){
//{{{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the AFIO clock.  GPIO_EXTILineConfig sets registers in
     * the AFIO.
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* Connect EXTI Line 0 to the button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    /* Configure the EXTI line to generate an interrupt when the button is
     * pressed.  The button pin is high when pressed, so it needs to trigger
     * when rising from low to high. */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
//}}}	
}


char ROT13(char c){
//{{{	
	static char uc_rot13_map[] = "NOPQRSTUVWXYZABCDEFGHIJKLM";
	static char lc_rot13_map[] = "nopqrstuvwxyzabcdefghijklm";
	if(c >= 'A' && c<= 'Z'){
		return uc_rot13_map[ (c-'A') ];
	}else if( c>='a' && c<='z'){
		return lc_rot13_map[ (c-'a') ];
	}else
		return c;
//}}}	
}

void uart_rx(){
//{{{	
//	volatile unsigned count = 0;
	/* Start of Routine #1 */
	setjmp(uart_rx_buf);

//	printf("[Routine 1] pass %d\n", ++count);

	// code of process
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) {					// rx not empty != 0 (enabled) (RX DR not empty -> int)
	    in_char = USART_ReceiveData(USART2);
		if(!mask_req)
			rx_char = 1;
		else
			rx_char = 0;
	}
	longjmp(uart_tx_buf, 1);
//}}}	
}

void create_uart_rx(const void *stackptr){
//{{{	
	register unsigned long savedstack;

	SAVE_STACK_POINTER(savedstack, stackptr);

	if (setjmp(uart_rx_buf) == 0) {
		RESTORE_STACK(savedstack);
		rx_char = 0;
	}
	else {
		/* We got here through longjmp */
		uart_rx();
	}
//}}}	
}

#define FINAL_NUM 5
void uart_tx(){
//{{{	
//	volatile unsigned count = 0;
	char rot13_char;
	/* Start of Routine #2 */
	setjmp(uart_tx_buf);

//	printf("[Routine 2] pass %d\n", (FINAL_NUM - ++count));
		// code of process	
	if(rx_char){
		rot13_char = ROT13(in_char);
	    USART_SendData(USART2, rot13_char);

	    /* Loop until USART2 DR register is empty */
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		rx_char = 0;
	}


//	if (count < FINAL_NUM)
		longjmp(uart_rx_buf, 1);

//	exit(0);
//}}}		
}

void create_uart_tx(const void *stackptr){
//{{{	
	register unsigned long savedstack;

	SAVE_STACK_POINTER(savedstack, stackptr);

	if (setjmp(uart_tx_buf) == 0) {
		RESTORE_STACK(savedstack);
	}
	else {
		uart_tx();
	}
//}}}	
}


/* Handler predefined by STM32 library to handle interrupts on EXTI line 0
 * (which is mapped to the button on GPIO A Line 0).
 */
void EXTI0_IRQHandler(void)
{
    /* Make sure the line has a pending interrupt
     * (should this always be true if we are inside the interrupt handle?) */
    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Toggle  the LED */
		if(!mask_req){
			mask_req = 1;
			rx_char = 0;
        }else{
			mask_req = 0;
		}
	

        /* Clear the pending interrupt flag that triggered this interrupt.
         * If DO_NOT_CLEAR_IT_PENDING_FLAG is defined, this part is skipped.
         * This will cause the interrupt handler to repeatedly run in an
         * infinite loop (which will cause the LED to repeatedly toggle too
         * fast to be seen).
         */
#ifndef DO_NOT_CLEAR_IT_PENDING_FLAG
        EXTI_ClearITPendingBit(EXTI_Line0);
#endif
    }
}


int main(void){


    init_button();
    enable_button_interrupts();

	init_rs232();
    USART_Cmd(USART2, ENABLE);
	enable_rs232();



//	create_uart_rx((char *) malloc(STACK_SIZE) + STACK_SIZE);
//	create_uart_tx((char *) malloc(STACK_SIZE) + STACK_SIZE);
//	create_button_rx((char *) malloc(STACK_SIZE) + STACK_SIZE);

	create_uart_rx( uart_rx_stack + STACK_SIZE);
	create_uart_tx( uart_tx_stack + STACK_SIZE);
//
//
	longjmp(uart_rx_buf, 1);

	return 0;
}



