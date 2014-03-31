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

int last_button_state, new_button_state;
int mask_req = 0;
char in_char;
int rx_char = 0;

jmp_buf uart_rx_buf;
jmp_buf uart_tx_buf;
jmp_buf button_rx_buf;

char uart_rx_stack[STACK_SIZE];
char uart_tx_stack[STACK_SIZE];
char button_rx_stack[STACK_SIZE];

void init_button(void){
//{{{	
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO A clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure the button pin as a floating input. */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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
	if(!mask_req){

	    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) {					// rx not empty != 0 (enabled) (RX DR not empty -> int)
	        in_char = USART_ReceiveData(USART2);
			rx_char = 1;
	    }
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
		longjmp(button_rx_buf, 1);

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



void button_rx(){
//{{{	
//	volatile unsigned count = 0;

	/* Start of Routine #2 */
	setjmp(button_rx_buf);

//	printf("[Routine 2] pass %d\n", (FINAL_NUM - ++count));

	// code of process
    new_button_state = GPIOA->IDR & 0x00000001;
//	    USART_SendData(USART2, '[');
//		USART_SendData(USART2, (new_button_state)?'1':'0');
//	    USART_SendData(USART2, '\n');
//	    USART_SendData(USART2, '\r');

    if(new_button_state) {
		if(!mask_req){
			USART_SendData(USART2, '1');
		    USART_SendData(USART2, '\n');

			mask_req = 1;
			// clear buf
        }else{
			USART_SendData(USART2, '0');
		    USART_SendData(USART2, '\n');

			mask_req = 0;
		}
		GPIOA->IDR &= 0xfffffffe;
		new_button_state = 0;
    }
    last_button_state = new_button_state;

//	if (count < FINAL_NUM)
		longjmp(uart_rx_buf, 1);

//	exit(0);
//}}}		
}

void create_button_rx(const void *stackptr){
//{{{	
	register unsigned long savedstack;

	SAVE_STACK_POINTER(savedstack, stackptr);

	if (setjmp(button_rx_buf) == 0) {
		RESTORE_STACK(savedstack);
	}
	else {
		last_button_state = 0;
		mask_req = 0;

		button_rx();
	}
//}}}	
}



int main(void){


    init_button();
	init_rs232();
    USART_Cmd(USART2, ENABLE);
	enable_rs232();



//	create_uart_rx((char *) malloc(STACK_SIZE) + STACK_SIZE);
//	create_uart_tx((char *) malloc(STACK_SIZE) + STACK_SIZE);
//	create_button_rx((char *) malloc(STACK_SIZE) + STACK_SIZE);

	create_uart_rx( uart_rx_stack + STACK_SIZE);
	create_uart_tx( uart_tx_stack + STACK_SIZE);
	create_button_rx( button_rx_stack + STACK_SIZE);
//
//
	longjmp(button_rx_buf, 1);

	return 0;
}



