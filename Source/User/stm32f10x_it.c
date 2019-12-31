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
#include "current.h"
#include "task_motor_control.h"

extern foc_mod_t foc_obj;


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

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
void ADC1_2_IRQHandler(void)
{
	Curr_Components cur_ab;
	int32_t wia = 0, wib = 0;
	if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET){
		#if 0
		cur_ab.qI_Component1 = ADC_GetInjectedConversionValue(ADC1, IA_INJECT_CHANNEL_DRI);
		cur_ab.qI_Component2 = ADC_GetInjectedConversionValue(ADC1, IB_INJECT_CHANNEL_DRI);
		set_inject_ia(cur_ab.qI_Component1); 
		set_inject_ib(cur_ab.qI_Component2); 
		#else
		ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
		set_inject_ia(IA_VALUE_REG); 
		set_inject_ib(IB_VALUE_REG);
		
		/* 
			ST坐标 a/b			
		
			a
			^
			|
			|
			|
			--------------> b		
		*/
		wia = (int32_t)((int32_t)(IA_VALUE_REG - foc_obj.feedback.cur_offset.qI_Component1)*Q14/2048*165/454*5);
		wib = (int32_t)((int32_t)(IB_VALUE_REG - foc_obj.feedback.cur_offset.qI_Component2)*Q14/2048*165/454*5);
		
		cur_ab.qI_Component1 = (int16_t)wib;
		cur_ab.qI_Component2 = (int16_t)wia;
		
		if(stm_get_cur_state(&motor_state) == TEST){
		
		}
		if(stm_get_cur_state(&motor_state) == START_UP){
			task_motor_startup_05(cur_ab,50000);
			//task_motor_startup_04(cur_ab,50000);
			//task_motor_startup_03(cur_ab,50000);
			//task_motor_startup_02(cur_ab,5000);
			//task_motor_startup(cur_ab,5000);
		}else if(stm_get_cur_state(&motor_state) == RUN){
			task_motor_cur_loop(cur_ab);
		}else if (stm_get_cur_state(&motor_state) == STOP){
			task_motor_stop();
		}
		ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
		#endif
	}
	
	/* Clear ADC1 JEOC pending interrupt bit */
	ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
}


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
