/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "debug_support.h"

//Library config for this project!!!!!!!!!!!
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
  
uint16_t PrescalerValue = 0;

__IO uint32_t TimingDelay;
__IO uint8_t DemoEnterCondition = 0x00;
__IO uint8_t UserButtonPressed = 0x00;
__IO int8_t X_Offset, Y_Offset, Z_Offset  = 0x00;
uint8_t Buffer[6];

/* Private function prototypes -----------------------------------------------*/
static void Demo_Exec(void);

/* Private functions ---------------------------------------------------------*/
int factorial(int n)
{
	int c;
	int result = 1;
	for (c = 1; c <= n; c++)
		result *= c;
	return result;
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    
    /* Initialize LEDs and User_Button on STM32F4-Discovery --------------------*/
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 
    
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
    Demo_Exec();
}

/**
  * @brief  Execute the demo application.
  * @param  None
  * @retval None
  */
static void Demo_Exec(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  uint8_t togglecounter = 0x00;
  int i=0, n=0;  

  printf("Hello World\n");

  while(1)
  {
    DemoEnterCondition = 0x00;
    
    /* Reset UserButton_Pressed variable */
    UserButtonPressed = 0x00;
    
    /* Initialize LEDs to be managed by GPIO */
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);  
    
    /* Turn OFF all LEDs */
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    
    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x00)
    {
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED4);
      Delay(10);
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED3);
      Delay(10);
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED5);
      Delay(10);
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED6);
      Delay(10);
      togglecounter ++;
      if (togglecounter == 0x10)
      {
        togglecounter = 0x00;
        while (togglecounter < 0x10)
        {
          STM_EVAL_LEDToggle(LED4);
          STM_EVAL_LEDToggle(LED3);
          STM_EVAL_LEDToggle(LED5);
          STM_EVAL_LEDToggle(LED6);
          Delay(10);
          togglecounter ++;
        }
       togglecounter = 0x00;
      }
	    for (i = 0; i < 10; ++i) {
	    	n = factorial(i);
	    	printf("factorial(%d) = %d\n", i, n);
	    }


    }
    
    /* Waiting User Button is Released */
    while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET)
    {}
    UserButtonPressed = 0x00;
    
  }
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
  if (TimingDelay != 0x00)
  { 
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

/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured during Test program execution */
  if (DemoEnterCondition == 0x00)
  {
    /* Timeout error occured for SPI TXE/RXNE flags waiting loops.*/
    Fail_Handler();    
  }
  /* MEMS Accelerometer Timeout error occured during Demo execution */
  else
  {
    while (1)
    {   
    }
  }
  return 0;  
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
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
