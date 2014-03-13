#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"
#include "stm32_p103.h"
#include "string.h"


#define BUF_SIZE 100

int led_st = 1;
int button_cnt = 0;
char in_char;
int ibuf_wptr=0, ibuf_rptr=0; 
int obuf_rptr=0, obuf_wptr=0;	
char uart_ibuf[BUF_SIZE];
char uart_obuf[BUF_SIZE];
char *cmd = "info";
char *arg_led = "led";
char *arg_button_cnt = "button";

//
int is_obuf_full(){
	return ( (obuf_rptr>=obuf_wptr)? ((obuf_rptr-obuf_wptr)==1):((BUF_SIZE-obuf_wptr+obuf_rptr)==1) );
}
int is_obuf_empty(){
	return ( obuf_rptr==obuf_wptr );
}

void adds_to_obuf(const char* str){
	int i=0;
	while( !is_obuf_full() ){
		uart_obuf[obuf_wptr] = str[i];
		if(str[i] == '\0') break;
		obuf_wptr++;
		obuf_wptr%=BUF_SIZE;
		i++;
		i%=BUF_SIZE;
	}
	if(is_obuf_full()){
		uart_obuf[obuf_wptr] = '\0';
	}
}
char getc_from_obuf(){
	char ret='\0';
	if( !is_obuf_empty() ){
		ret = uart_obuf[obuf_rptr];
		obuf_rptr++;
		obuf_rptr%=BUF_SIZE;
	}
	return ret;
}

//
int is_ibuf_full(){
	return ( (ibuf_rptr>=ibuf_wptr)? ((ibuf_rptr-ibuf_wptr)==1):((BUF_SIZE-ibuf_wptr+ibuf_rptr)==1) );
}
int is_ibuf_empty(){
	return ( ibuf_rptr==ibuf_wptr );
}

void adds_to_ibuf(const char* str){
	int i=0;
	while( !is_ibuf_full() ){
		uart_ibuf[ibuf_wptr] = str[i];
		if(str[i] == '\0') break;
		ibuf_wptr++;
		ibuf_wptr%=BUF_SIZE;
		i++;
		i%=BUF_SIZE;
	}
	if(is_ibuf_full()){
		uart_ibuf[ibuf_wptr] = '\0';
	}
}
void addc_to_ibuf(const char c){
	if(!is_ibuf_full()){
		uart_ibuf[ibuf_wptr] = c;

		ibuf_wptr++;
		ibuf_wptr%=BUF_SIZE;
	}else if(c=='\0'){
		uart_ibuf[ibuf_wptr] = c;
	}
}

char getc_from_ibuf(){
	char ret='\0';
	if( !is_ibuf_empty() ){
		ret = uart_ibuf[ibuf_rptr];
		ibuf_rptr++;
		ibuf_rptr%=BUF_SIZE;
	}
	return ret;
}
void atos(int a, char* str, int buf_len){
	int buf_ptr=0;
	if(buf_len==0) return;
	if(a == 0) str[buf_ptr++] = '0';
	for ( ; (buf_ptr<buf_len) && a; a /= 10)
		str[buf_ptr++] = "0123456789"[a%10];
	str[buf_ptr]='\0';
}
//
int parse_cmd(){
	int search_ptr=0, buf_ptr=0, i;
	char buf[BUF_SIZE], uart_dump[BUF_SIZE];
	int tmp_ptr;

	tmp_ptr = ibuf_rptr;
	i=0;
	while( !is_ibuf_empty() ){
		uart_dump[i] = getc_from_ibuf();
		i++;
		tmp_ptr++;
		tmp_ptr%=BUF_SIZE;
	}

	// find space
//	while( (search_ptr != ibuf_wptr) && (uart_ibuf[search_ptr] != '\0') && (uart_ibuf[search_ptr] != ' ')){
	while( (search_ptr != BUF_SIZE) && (uart_dump[search_ptr] != '\0') && (uart_dump[search_ptr] != ' ')){
		search_ptr++;
//		search_ptr%=BUF_SIZE;
	}
				
//	if( !strncmp(uart_ibuf, cmd, search_ptr)){
	if( !strncmp(uart_dump, cmd, search_ptr)){
		// find not space
//		while( (search_ptr != ibuf_wptr) && (uart_ibuf[search_ptr] != '\0') && (uart_ibuf[search_ptr] == ' ')){
		while( (search_ptr != BUF_SIZE) && (uart_dump[search_ptr] != '\0') && (uart_dump[search_ptr] == ' ')){
			search_ptr++;
//			search_ptr%=BUF_SIZE;
		}
		



//		if( !strcmp(arg_led, &uart_ibuf[search_ptr]) ){
		if( !strcmp(arg_led, &uart_dump[search_ptr]) ){
			if(led_st){
//				uart_obuf[0] = 'o';
//				uart_obuf[1] = 'n';
//				uart_obuf[2] = '\n';
//				uart_obuf[2] = '\r';
//				uart_obuf[3] = '\0';
				adds_to_obuf("on\0");
			}else{
//				uart_obuf[0] = 'o';
//				uart_obuf[1] = 'f';
//				uart_obuf[2] = 'f';
//				uart_obuf[3] = '\n';
//				uart_obuf[3] = '\r';
//				uart_obuf[3] = '\0';
				adds_to_obuf("off\0");
			}
			
//		}else if( !strcmp(arg_button_cnt, &uart_ibuf[search_ptr]) ){
		}else if( !strcmp(arg_button_cnt, &uart_dump[search_ptr]) ){
			atos(button_cnt, buf, BUF_SIZE);
//			if(button_cnt == 0) buf[buf_ptr++] = '0';
//			for ( ; button_cnt; button_cnt /= 10)
//				    buf[buf_ptr++] = "0123456789"[button_cnt%10];
//			buf[buf_ptr]='\0';

//			for (i = 0; i < buf_ptr; i++)
//				uart_obuf[i] = buf[buf_ptr - i - 1];
//			uart_obuf[buf_ptr] = '\0';

			adds_to_obuf(buf);
		}else{
			return 0;
		}
	}else{
		return 0;
	}
	return 1;
}
//void send_byte(uint8_t b)
//{
//    /* Send one byte */
//    USART_SendData(USART2, b);
//
//    /* Loop until USART2 DR register is empty */
//    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//}

/* Handler predefined by STM32 library to handle interrupts on EXTI line 0
 * (which is mapped to the button on GPIO A Line 0).
 */
void EXTI0_IRQHandler(void)
{
//{{{	
    /* Make sure the line has a pending interrupt
     * (should this always be true if we are inside the interrupt handle?) */
    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Toggle  the LED */
        GPIOC->ODR = GPIOC->ODR ^ 0x00001000;

        /* Clear the pending interrupt flag that triggered this interrupt.
         * If DO_NOT_CLEAR_IT_PENDING_FLAG is defined, this part is skipped.
         * This will cause the interrupt handler to repeatedly run in an
         * infinite loop (which will cause the LED to repeatedly toggle too
         * fast to be seen).
         */
		led_st ^= 1;
		button_cnt++;

//		send_byte('H');
//        send_byte('e');
//        send_byte('l');
//        send_byte('l');
//        send_byte('o');
//        send_byte('\n');
//        send_byte('\r');

		adds_to_obuf("Hello\0");
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);


#ifndef DO_NOT_CLEAR_IT_PENDING_FLAG
        EXTI_ClearITPendingBit(EXTI_Line0);
#endif
    }
//}}}	
}
/* Handler predefined by STM32 library to handle USART2 interrupts. */
void USART2_IRQHandler(void){
//{{{	
	char out_char;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {					// rx not empty != 0 (enabled) (RX DR not empty -> int)
        in_char = USART_ReceiveData(USART2);
//		if(ibuf_wptr < BUF_SIZE){
//		if(!is_ibuf_full() ){
//			uart_ibuf[ibuf_wptr] = in_char;
//			addc_to_ibuf(in_char);
			if(in_char == '\r'){
	            USART_SendData(USART2, '\n');	
	            USART_SendData(USART2, '\r');	
//				uart_ibuf[ibuf_wptr] = '\0';
				addc_to_ibuf('\0');

				if( parse_cmd() ){

//					obuf_rptr = 0;
					USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

				}else{
//					ibuf_wptr = 0;
				}
			}else{
//	            USART_SendData(USART2, uart_ibuf[ibuf_wptr]);	
//				ibuf_wptr++;
				if(!is_ibuf_full()){
					addc_to_ibuf(in_char);
		            USART_SendData(USART2, in_char);	
				}

			}
//		}
    }

    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {					// tx empty != 0 (enabled) ( TX DR empty -> int) 
//		if(uart_obuf[obuf_rptr] == '\0'){
//	        USART_SendData(USART2, '\n');	
//	        USART_SendData(USART2, '\r');	
//            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
//			ibuf_wptr = 0;
//		}else{
//            USART_SendData(USART2, uart_obuf[obuf_rptr++]);
//		}
		if((out_char=getc_from_obuf()) == '\0'){
	        USART_SendData(USART2, '\n');	
	        USART_SendData(USART2, '\r');	
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}else{
            USART_SendData(USART2, out_char);
		}


    }
//}}}	
}


int main(void)
{
    int last_button_state, new_button_state;

    init_led();
    init_button();
	init_rs232();
    USART_Cmd(USART2, ENABLE);
	enable_button_interrupts();
	enable_rs232_interrupts();
	enable_rs232();
    /* Infinite loop - when the button changes state, toggle the LED.
     */

	ibuf_wptr = 0;
	while(1);
}
