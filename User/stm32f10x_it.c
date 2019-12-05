/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f10x_it.h"
#include "user_config.h"
#include "foc.h"
#include "user_state_machine.h"

extern foc_mod_t foc_obj;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
		//printf("HardFault_Handler\n");
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
	
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{	
	//timer_add(&global_timer);
	static int32_t time_cricle = 0;
	#if USE_TASK_LIST
		gw_task_schedule();
	#else
	if(time_cricle++%1 == 0){
		gw_poll_event_task();		
	}	
	#endif
	gw_global_timer_add();
	gw_hal_dec();
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
#include "current.h"
/*
void TIM1_UP_IRQHandler(void){
	//static uint32_t i=0;
	if(TIM_GetFlagStatus(TIM1, TIM_IT_Update) == SET){
		//set_inject_ia(cur_fbk_get_Ia());
		//set_inject_ib(cur_fbk_get_Ib());
	}	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}	
*/
volatile int32_t wia = 0,wib=0,wic=0;
volatile int16_t ia_tmp = 0, ib_tmp = 0;
volatile int16_t hmul = 0x02, hdiv = 0x02;
extern void task_motor_cur_loop(Curr_Components cur_ab);
extern void task_motor_startup(Curr_Components cur_ab,uint16_t timeout);
void ADC1_2_IRQHandler(void)
{
	Curr_Components cur_ab;
	
	if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET){
		#if 0
		cur_ab.qI_Component1 = ADC_GetInjectedConversionValue(ADC1, IA_INJECT_CHANNEL_DRI);
		cur_ab.qI_Component2 = ADC_GetInjectedConversionValue(ADC1, IB_INJECT_CHANNEL_DRI);
		set_inject_ia(cur_ab.qI_Component1); 
		set_inject_ib(cur_ab.qI_Component2); 
		#else
		ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
		set_inject_ia(ADC1->JDR1); 
		set_inject_ib(ADC1->JDR2);
		foc_obj.feedback.ia_asc = ADC1->JDR3;
		foc_obj.feedback.ib_asc = ADC1->JDR4;
		//cur_ab.qI_Component1 = (int32_t)(ADC1->JDR1 - 2048)*Q15*38/2048;
		//cur_ab.qI_Component2 = (int32_t)(ADC1->JDR2 - 2048)*Q15*38/2048;
		
		//wic = (int32_t)((ADC1->JDR1 - 2048)*165*5/2048)*Q13/454/2; //0-1 ==> 0-2A Q15 FORMAT
		//wib = (int32_t)((ADC1->JDR2 - 2048)*165*5/2048)*Q13/454/2; //0-1 ==> 0-2A Q15 FORMAT
		//wia = -(wib+wic);
		
		//cur_ab.qI_Component1 = (int16_t)wia;
		//cur_ab.qI_Component2 = (int16_t)wib;
		
		//wia = (int32_t)((ADC1->JDR1 - 2048)*4)*Q13/2048; //0-1 ==> -4A~4A Q13 FORMAT
		//wib = (int32_t)((ADC1->JDR2 - 2048)*4)*Q13/2048; //0-1 ==> -4A~4A Q13 FORMAT
		
		// Uadc = Uin*4.54
		
		wia = (int32_t)((int32_t)(ADC1->JDR1 - foc_obj.feedback.cur_offset.qI_Component1)*Q14/2048*165/454*5);
		wib = (int32_t)((int32_t)(ADC1->JDR2 - foc_obj.feedback.cur_offset.qI_Component2)*Q14/2048*165/454*5);
		
		ia_tmp = (int16_t)((int32_t)(ADC1->JDR1 - foc_obj.feedback.cur_offset.qI_Component1)*Q14/2048*165/454*5000/Q14);
		ib_tmp = (int16_t)((int32_t)(ADC1->JDR2 - foc_obj.feedback.cur_offset.qI_Component2)*Q14/2048*165/454*5000/Q14);
		
		//wia = (int32_t)((ADC1->JDR1 - 2048)*165*5/2048)*Q15/454*hmul/hdiv; //0-1 ==> 0-2A Q15 FORMAT
		//wib = (int32_t)((ADC1->JDR2 - 2048)*165*5/2048)*Q15/454*hmul/hdiv; //0-1 ==> 0-2A Q15 FORMAT
		//wic = (int32_t)(4096 - ADC1->JDR1+ADC1->JDR2)*165*5/2048*Q14/454*hmul/hdiv;
		
		cur_ab.qI_Component1 = (int16_t)wib;
		cur_ab.qI_Component2 = (int16_t)wia;
		
		if(stm_get_cur_state(&motor_state) == IDLE){
			task_motor_startup(cur_ab,5000);
		}else if(stm_get_cur_state(&motor_state) == RUN){
			task_motor_cur_loop(cur_ab);
		}
		//task_motor_cur_loop(cur_ab);
		ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
		#endif
	}
	
	/* Clear ADC1 JEOC pending interrupt bit */
	//task_motor_cur_loop(cur_ab);
	ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
}


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
