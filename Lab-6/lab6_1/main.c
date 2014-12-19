/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

#include "stm32f4xx_conf.h"

/** @addtogroup STM32F4-Discovery_Demo
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

__IO uint32_t TimingDelay;


/* Private function prototypes -----------------------------------------------*/

void USB_Test(void);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{

	/* Initialize LEDs and User_Button on STM32F4-Discovery --------------------*/

	__IO uint32_t i = 0;  
	uint8_t buf[255];
	uint8_t len;

	USBD_Init(&USB_OTG_dev,     
			USB_OTG_FS_CORE_ID, 
			&USR_desc, 
			&USBD_CDC_cb, 
			&USR_cb);

	VCP_send_str("TEST START\n");

	VCP_send_str("TEST USB\n");
    USB_Test();
	VCP_send_str("USB PASS\n");


	VCP_send_str("TEST END\n");
	return 0;    
}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 10 ms.
 * @retval None
 */
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00) { 
		TimingDelay--;
	}
}

/**
 * @brief  This function handles the test program fail.
 * @param  None
 * @retval None
 */
void Fail_Handler(void)
{
	/* Erase last sector */ 
	FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
	/* Write FAIL code at last word in the flash memory */
	FLASH_ProgramWord(TESTRESULT_ADDRESS, ALLTEST_FAIL);

	while(1)
	{
		/* Toggle Red LED */
		STM_EVAL_LEDToggle(LED5);
		Delay(5);
	}
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {
	}
}
#endif

/**
 * @}
 */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


/**
  * @brief Test USB Hardware.
  *   The main objectif of this test is to check the hardware connection of the 
  *   Audio and USB peripheral.
  * @param None
  * @retval None
  */
void USB_Test(void)
{
//{{{    
  GPIO_InitTypeDef GPIO_InitStructure;

  /******************************** USB Test **********************************/
  
  /*----------------- Part1: without cables connected ------------------------*/ 
  
  /* GPIOA, GPIOC and GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | \
                         RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* GPIOD Configuration: Pins 5 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Turn LED8 ON using PD5 */                              // USB LED
  GPIO_ResetBits(GPIOD, GPIO_Pin_5);
  
  /* GPIOC Configuration: Pin 0 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 9 in input pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

// power on STMPS2141 power switch. This will place 5V on the PA9 serial line whenever PC0 is set low.
  
  /* Turn LED7 ON using PC0 (5v) */                     // usb VBUS        , see user manual         
  GPIO_ResetBits(GPIOC, GPIO_Pin_0); 
  
  /* Waiting delay 10ms */
  Delay(1);
  
  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_RESET)
  {
    Fail_Handler();
  }  
  
  /* GPIOA Configuration: Pins 10 in output push-pull */                    // OTG_FS_ID
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check the ID level without cable connected */
  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == Bit_RESET)
  {
    Fail_Handler();
  }
 
  /* Turn LED7 OFF using PC0 */
  GPIO_SetBits(GPIOC, GPIO_Pin_0);  
  
  /* GPIOA Configuration: Pins 11, 12 in input pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 9 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                                 // VBUS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_9);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check PA11 and PA12 level without cable connected */
  if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_RESET) || \
      (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET))
  {
    Fail_Handler();
  }
  
  /* GPIOA Configuration: Pins 12 in input pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 11 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_11);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check PA12 level without cable connected */
  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET)
  {
    Fail_Handler();
  }
  
  /* GPIOA Configuration: Pins 11 in input pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 12 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_12);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check PA12 level without cable connected */
  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_RESET)
  {
    Fail_Handler();
  }
  
  /* GPIOA Configuration: Pins 9 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Turn LED7 ON using PA9 */
  GPIO_SetBits(GPIOA, GPIO_Pin_9);
  
  /* Turn Green LED ON: signaling Audio USB Test part1 PASS */
  STM_EVAL_LEDOn(LED4);
  
  /* Waiting User Button is pressed */
  
  /* Waiting User Button is Released */
  
  /* Turn Green LED OFF: signaling the end of Audio USB Test part1 and switching to 
  the part2 */
  STM_EVAL_LEDOff(LED4);
  
  /* Turn LED7 OFF using PA9 */
  GPIO_ResetBits(GPIOA, GPIO_Pin_9);
  
  /* Turn LED8 OFF using PD5 */
  GPIO_SetBits(GPIOD, GPIO_Pin_5);
  
  /*--------------- Part2: with Audio USB cables connected  ------------------*/ 
  
  /*********************************** USB Test *******************************/
  /* Check the ID level with cable connected */
  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) != Bit_RESET)
  {
    Fail_Handler();
  }
  
  /* GPIOA Configuration: Pins 11, 12 in input pull-down */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 9 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_9);
  
  /* Waiting delay 10ms */
  Delay(1);
 
  /* Check PA11 and PA12 level with cable connected */
  if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_RESET) || \
      (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET))
  {
    Fail_Handler();
  }  
  
  /* GPIOA Configuration: Pins 9, 12 in input pull-down */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 11 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_11);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check PA9 and PA12 level with cable connected */
  if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_RESET)|| \
      (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET))
  {
    Fail_Handler();
  }
  
  /* GPIOA Configuration: Pins 9, 11 in input pull-down */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 12 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
  
  /* Waiting delay 10ms */
  Delay(1);
  
  /* Check PA9 and PA12 level with cable connected */
  if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_RESET)|| \
      (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_RESET))
  {
    Fail_Handler();
  }

  /* GPIOA Configuration: Pins 11, 12 in input pull-down */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* GPIOA Configuration: Pin 9 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Turn LED7 OFF using PA9 */
  GPIO_ResetBits(GPIOA, GPIO_Pin_9);
//}}}  
}

