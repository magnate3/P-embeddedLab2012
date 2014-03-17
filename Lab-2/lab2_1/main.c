#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

int main(void)
{

// init_rs232 in stm32_p103.c
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
//
//	// should successful to connect to remote
//    /* Enable peripheral clocks. */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	// stm32f10x_rcc.c
//	void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
//	{
//	  /* Check the parameters */
//	  assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));			// use call
//	  assert_param(IS_FUNCTIONAL_STATE(NewState));
//	  if (NewState != DISABLE)
//	  {
//	    RCC->APB2ENR |= RCC_APB2Periph;
//	  }
//	  else
//	  {
//	    RCC->APB2ENR &= ~RCC_APB2Periph;
//	  }
//	}

//    /* Configure USART2 Rx pin as floating input. */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//    /* Configure USART2 Tx as alternate function push-pull. */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//    /* Configure the USART2 */
//    USART_InitStructure.USART_BaudRate = 9600;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    USART_InitStructure.USART_Parity = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//    USART_Init(USART2, &USART_InitStructure);
//    USART_Cmd(USART2, ENABLE);


//enable rs232 in stm32_p103.c
    /* Enable the RS232 port. */
//    USART_Cmd(USART2, ENABLE);


//	USART_SendData(USART2, 'H');
//	USART_SendData(USART2, 'e');
//	USART_SendData(USART2, 'l');
//	USART_SendData(USART2, 'l');
//	USART_SendData(USART2, 'o');
//	USART_SendData(USART2, '!');
//	USART_SendData(USART2, '\n');


	return 0;
}
