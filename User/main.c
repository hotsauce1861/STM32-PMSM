/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
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
#include "stm32f10x.h"
#include <stdio.h>
//#include "display_driver.h"
#include "usart_driver.h"
#include "gw_fifo.h"
#include "svpwm_module.h"
#include "SDS.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static __IO uint32_t TimingDelay;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	
	volatile uint16_t i = 0;
	//uint8_t tmp_data = 0x00;
	
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	*/     	
	/*
		hardware init
	*/
	timer_base_config();
	usart_init();
	svpwm_init();

	/*
		software init
	*/
	gw_event_fifo_init();	

	
	
	while (1)
	{			
		gw_execute_event_task();
		//Delay(5);
		#if 0
		svpwm.UAlpha = sine_table[i%SINE_TABLE_SIZE]; 
		svpwm.UBeta = sine_table[(i+90)%SINE_TABLE_SIZE];//需要限制幅度作处理
		#else
//		psvpwm->UAlpha = sine_table[i%SINE_TABLE_SIZE]; 
//		psvpwm->UBeta = consine_table[(i+270)%SINE_TABLE_SIZE];//需要限制幅度作处理			
		#endif		
		//svpwm_nofloat_run(psvpwm);
		//test_data[0] = svpwm_nofloat_get_sector(&svpwm);
		//tmp_data = svpwm_nofloat_get_sector(&svpwm);			
		//pwm_reset_duty_cnt(1, psvpwm->Tcm1);
		//pwm_reset_duty_cnt(2, psvpwm->Tcm2);
		//pwm_reset_duty_cnt(3, psvpwm->Tcm3);

		//test_data[0] = psvpwm->Tcm1;
		//test_data[1] = psvpwm->Tcm2;
		//test_data[2] = psvpwm->Tcm3;
		//test_data[3] = psvpwm->sector*100;
		
		//test_data[0] = (int32_t)(tmp_data);
		//test_data[1] = 0;
		//test_data[2] = 0;
		//test_data[3] = 0;
		//printf("%d psvpwm->Tcm1 	: is %d\n",i,psvpwm->Tcm1);
		//printf("%d psvpwm->Tcm2 	: is %d\n",i,psvpwm->Tcm2);
		//printf("%d psvpwm->Tcm3 	: is %d\n",i,psvpwm->Tcm3);
		//usart_send_char(0x55);
		//SDS_OutPut_Data_INT(test_data);
		//i++;	
	}	
}


void Delay(__IO uint32_t nTime);

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
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


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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
