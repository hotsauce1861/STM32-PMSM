#include "encoder.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#define SAMPLE_FRQ 	7500L
#define SYS_FRQ		72000000L



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
__IO uint16_t IC3OldValue1 = 0, IC3OldValue2 = 0;

__IO uint16_t CaptureNumber = 0;
__IO uint32_t Capture = 0;
__IO uint32_t TIM3Freq = 0;
__IO uint8_t circle_reset = 0;
__IO uint16_t circle_count = 0;
__IO uint8_t dir = 0; //0:back 1:forward

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void encoder_pin_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void encoder_rcc_init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
}

void encoder_tim_init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure; 		
	
	TIM_TimeBaseStructure.TIM_Period = SYS_FRQ / SAMPLE_FRQ - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ICStructInit(&TIM_ICInitStructure); // 设置默认值

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPrescaler = 0;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPrescaler = 0;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;


	//TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);	
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_Cmd(TIM3, ENABLE);

}

uint8_t encoder_get_reset_signal_flag(void){
	return circle_reset;
}

void encoder_set_reset_signal_flag(uint8_t flag){
	circle_reset = flag;
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void encoder_irq_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
}

void encoder_init(void){
	encoder_rcc_init();
	encoder_pin_init();
	encoder_irq_init();
	encoder_tim_init();
}

void EXTI9_5_IRQHandler(void){

	if(EXTI_GetFlagStatus(EXTI_Line5) == SET){ 
		circle_count++;
		TIM_SetCounter(TIM3,0x00);
	}
	EXTI_ClearITPendingBit(EXTI_Line5);
}


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{ 
	
	static uint8_t i = 0;
//	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
//	if(TIM_GetITStatus(TIM3, TIM_FLAG_CC1) == SET){
		if(TIM3->SR&(0x0001 << 1)){
//		IC3ReadValue2 = TIM_GetCapture2(TIM3);
		if(i++%4 == 0){
			if(CaptureNumber == 0)
		{
		  /* Get the Input Capture value */
		  IC3ReadValue1 = TIM_GetCapture1(TIM3);
		  CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
		  /* Get the Input Capture value */
		  IC3ReadValue2 = TIM_GetCapture1(TIM3); 
		  
		  /* Capture computation */
		  if (IC3ReadValue2 > IC3ReadValue1)
		  {
			Capture = (IC3ReadValue2 - IC3ReadValue1); 
			dir = 0;
		  }
		  else
		  {
			Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
			dir = 1;
		  }
		  /* Frequency computation */ 
		  TIM3Freq = (uint32_t) SystemCoreClock / Capture;
		  CaptureNumber = 0;
		
		}
		
		}		
	}
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM3->SR&=~(1<<1);

#if 0
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	}
	
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) {
		/* Clear TIM3 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		if(CaptureNumber == 0)
		{
			/* Get the Input Capture value */
			IC3ReadValue1 = TIM_GetCapture2(TIM3);
			CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
			/* Get the Input Capture value */
			IC3ReadValue2 = TIM_GetCapture2(TIM3); 

			/* Capture computation */
			if (IC3ReadValue2 > IC3ReadValue1)
			{
				Capture = (IC3ReadValue2 - IC3ReadValue1); 
			}
			else
			{
				Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
			}
			/* Frequency computation */ 
			TIM3Freq = (uint32_t) SystemCoreClock / Capture;
			CaptureNumber = 0;
		}
	}
#endif
}

