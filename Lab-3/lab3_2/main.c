#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

/* STM32F10x common functions */
#include "stm32_p103.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Our own string function implementation */
#include "string-util.h"

static void setup_hardware();

volatile xQueueHandle serial_str_queue = NULL;
volatile xSemaphoreHandle serial_tx_wait_sem = NULL;
volatile xQueueHandle serial_rx_queue = NULL;

/* Queue structure used for passing messages. */
typedef struct {
	char str[100];
} serial_str_msg;

/* Queue structure used for passing characters. */
typedef struct {
	char ch;
} serial_ch_msg;



//////
typedef struct staStrMsgDef{
	int times;
	char str[100];
	struct staStrMsgDef* pcNext;	
} staStrMsg;

volatile staStrMsg* sta_str_list = NULL;


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


char my_strcmp(char *restrict dest, const char *restrict src){
	unsigned char *s = src;
	unsigned char *d = dest;
	while( (*s!='\0')  && (*d!='\0') && (*s==*d) ){s++; d++;}

	if( (*s=='\0') && (*d=='\0') ) return 0;
	return -1;
}

void save_str_in_list(char* str_ptr){
//{{{	
	staStrMsg* new_sta_str = NULL;
	staStrMsg* read_ptr = NULL;


	read_ptr = sta_str_list;
	while( read_ptr != NULL && (staStrMsg*)read_ptr->pcNext != NULL){
		if( !my_strcmp( read_ptr->str, str_ptr) ){
			read_ptr->times++;
			break;
		}
		read_ptr = read_ptr->pcNext;
	}
	if( read_ptr == NULL || (staStrMsg*)read_ptr->pcNext == NULL ){
		new_sta_str = (staStrMsg*)pvPortMalloc( sizeof(staStrMsg) );
		new_sta_str->pcNext = NULL;
		my_strcpy( new_sta_str->str, str_ptr );
		new_sta_str->times = 1;
		if(read_ptr == NULL)
			sta_str_list = new_sta_str;
		else 
			read_ptr->pcNext = new_sta_str;
	}


//	while( (pcReadFromCpy+ sta_str_queue->uxItemSize) != sta_str_queue->pcWriteTo ){
//		if(!strcpy( (sta_str_msg*)pcReadFromCpy->str, str_ptr->str) ){
//			(sta_str_msg*)pcReadFromCpy->times++;
//			break;
//		}
//
//		pcReadFromCpy += sta_str_queue->uxItemSize;
//		if( pcReadFromCpy == sta_str_queue->pcTail ){
//			pcReadFromCpy = sta_str_queue->pcHead;
//		}
//	}
//	if( (pcReadFromCpy+ pcReadFromCpy->uxItemSize) != sta_str_queue->pcWriteTo){	// not found
//		my_str_cpy( cpy_sta_str.str, str_ptr->str);
//		cpy_sta_str.times = 1;
//		prvCopyDataToQueue( sta_str_queue, cpy_sta_str, queueSEND_TO_BACK );
//	}
//}}}	
}


void save_strtok(serial_str_msg* str_ptr){
//{{{	
	char* tmp_str_ptr = NULL;

	if(!str_ptr) return;

	while(!str_ptr){
		tmp_str_ptr = my_strtok(str_ptr->str, " ");
		save_str_in_list(tmp_str_ptr);
	};
//}}}	
}

portSTACK_TYPE taskDb[2];
/////




/* IRQ handler to handle USART2 interruptss (both transmit and receive
 * interrupts). */
void USART2_IRQHandler(){
//{{{	
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	serial_ch_msg rx_msg;

	/* If this interrupt is for a transmit... */
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		/* "give" the serial_tx_wait_sem semaphore to notfiy processes
		 * that the buffer has a spot free for the next byte.
		 */
		xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);

		/* Diables the transmit interrupt. */
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		/* If this interrupt is for a receive... */
	}
	else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		/* Receive the byte from the buffer. */
		rx_msg.ch = USART_ReceiveData(USART2);

		/* Queue the received byte. */
		if(!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &xHigherPriorityTaskWoken)) {
			/* If there was an error queueing the received byte,
			 * freeze. */
			while(1);
		}
	}
	else {
		/* Only transmit and receive interrupts should be enabled.
		 * If this is another type of interrupt, freeze.
		 */
		while(1);
	}

	if (xHigherPriorityTaskWoken) {
		taskYIELD();
	}
//}}}	
}

void send_byte(char ch){
//{{{	
	/* Wait until the RS232 port can receive another byte (this semaphore
	 * is "given" by the RS232 port interrupt when the buffer has room for
	 * another byte.
	 */
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));

	/* Send the byte and enable the transmit interrupt (it is disabled by
	 * the interrupt).
	 */
	USART_SendData(USART2, ch);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//}}}	
}

char receive_byte(){
//{{{	
	serial_ch_msg msg;

	/* Wait for a byte to be queued by the receive interrupts handler. */
	while (!xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY));

	return msg.ch;
//}}}	
}

// make led shinning
//void led_flash_task(void *pvParameters){
////{{{	
//	while (1) {
//		/* Toggle the LED. */
//		GPIOC->ODR = GPIOC->ODR ^ 0x00001000;
//
//		/* Wait one second. */
//		vTaskDelay(100);
//	}
////}}}	
//}

// get string from queue and send
void rs232_xmit_msg_task(void *pvParameters){
//{{{
	serial_str_msg msg;
	int curr_char;

	while (1) {
		/* Read from the queue.  Keep trying until a message is
		 * received.  This will block for a period of time (specified
		 * by portMAX_DELAY). */
		while (!xQueueReceive(serial_str_queue, &msg, portMAX_DELAY));

		/* Write each character of the message to the RS232 port. */
		curr_char = 0;
		while (msg.str[curr_char] != '\0') {
			send_byte( ROT13(msg.str[curr_char]) );
			curr_char++;
		}
		send_byte( '\n' );
		send_byte( '\r' );
	}
//}}}	
}


/* Repeatedly queues a string to be sent to the RS232.
 *   delay - the time to wait between sending messages.  A delay of 1 means
 *           wait 1/100th of a second.
 */
//void queue_str_task(const char *str, int delay){
////{{{	
//	serial_str_msg msg;
//
//	/* Prepare the message to be queued. */
//	my_strcpy(msg.str, str);
//
//	while (1) {
//		/* Post the message.  Keep on trying until it is successful. */
//		while (!xQueueSendToBack(serial_str_queue, &msg,
//		       portMAX_DELAY));
//
//		/* Wait. */
//		vTaskDelay(delay);
//	}
////}}}	
//}
//
//void queue_str_task1(void *pvParameters){
//	queue_str_task("Hello 1\n", 200);
//}
//
//void queue_str_task2(void *pvParameters){
//	queue_str_task("Hello 2\n", 50);
//}

// copy output message to message queue
void serial_readwrite_task(void *pvParameters){
//{{{	
	serial_str_msg msg;
	char ch;
	int curr_char;
	int done;

	/* Prepare the response message to be queued. */
//	my_strcpy(msg.str, "Got:");

	while (1) {
//		curr_char = 4;
		curr_char = 0;
		done = 0;
		do {
			/* Receive a byte from the RS232 port (this call will
			 * block). */
			ch = receive_byte();

			/* If the byte is an end-of-line type character, then
			 * finish the string and inidcate we are done.
			 */
			if ((ch == '\r') || (ch == '\n')) {
//				msg.str[curr_char] = '\n';
//				msg.str[curr_char+1] = '\0';
				msg.str[curr_char] = '\0';
				done = -1;
				/* Otherwise, add the character to the
				 * response string. */
				if(!my_strcmp(msg.str, "END") ){	// stop
					vTaskDelete(taskDb[0]);
					vTaskDelete(taskDb[1]);
					vTaskEndScheduler ();
				}
				save_strtok(&msg);
			}
			else {
				msg.str[curr_char++] = ch;
			}
		} while (!done);

		/* Once we are done building the response string, queue the
		 * response to be sent to the RS232 port.
		 */
		while (!xQueueSendToBack(serial_str_queue, &msg,
		                         portMAX_DELAY));
	}
//}}}	
}

int main(){
//{{{	
	init_led();

	init_button();
	enable_button_interrupts();

	init_rs232();
	enable_rs232_interrupts();
	enable_rs232();

	/* Create the queue used by the serial task.  Messages for write to
	 * the RS232. */
	// message passing
	serial_str_queue = xQueueCreate(10, sizeof(serial_str_msg));
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_ch_msg));
//	sta_str_queue = (sta_str_msg*)pvPortMalloc(1000 * sizeof(sta_str_msg));

	/* Create a task to flash the LED. */
//	xTaskCreate(led_flash_task,
//	            (signed portCHAR *) "LED Flash",
//	            512 /* stack size */, NULL,
//	            tskIDLE_PRIORITY + 5, NULL);

	/* Create tasks to queue a string to be written to the RS232 port. */
//	xTaskCreate(queue_str_task1,
//	            (signed portCHAR *) "Serial Write 1",
//	            512 /* stack size */, NULL,
//	            tskIDLE_PRIORITY + 10, NULL );
//	xTaskCreate(queue_str_task2,
//	            (signed portCHAR *) "Serial Write 2",
//	            512 /* stack size */,
//	            NULL, tskIDLE_PRIORITY + 10, NULL);

	/* Create a task to write messages from the queue to the RS232 port. */
	xTaskCreate(rs232_xmit_msg_task,
	            (signed portCHAR *) "Serial Xmit Str",
	            512 /* stack size */, NULL, tskIDLE_PRIORITY + 2, NULL);
	taskDb[0] = (portSTACK_TYPE) rs232_xmit_msg_task;
	/* Create a task to receive characters from the RS232 port and echo
	 * them back to the RS232 port. */
	xTaskCreate(serial_readwrite_task,
	            (signed portCHAR *) "Serial Read/Write",
	            512 /* stack size */, NULL,
	            tskIDLE_PRIORITY + 10, NULL);
	taskDb[1] = (portSTACK_TYPE) serial_readwrite_task;

	/* Start running the tasks. */
	vTaskStartScheduler();

	// print most frequent
	// print longest str


	return 0;
//}}}	
}

void vApplicationTickHook()
{
}
