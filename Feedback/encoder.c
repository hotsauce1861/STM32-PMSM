#include "encoder.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#define SAMPLE_FRQ 	10000L
#define SYS_FRQ		72000000L
#define ENCODER_MAX_CNT	0xFFFF
#define ENCODER_ZERO_VAL 0x7FFF

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile int32_t N = 0;
volatile uint32_t EncCnt = 0;

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
	
	TIM_TimeBaseStructure.TIM_Period = ENCODER_MAX_CNT;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12, 
									TIM_ICPolarity_Rising, 
									TIM_ICPolarity_Rising);

	
/*
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
*/
	//must clear it flag before enabe interrupt
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);	
	
	//TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	TIM_SetCounter(TIM3,ENCODER_ZERO_VAL);
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	
//	TIM3->CCMR1 |= 0x0001;
//	TIM3->CCMR2 |= 0x0001;
//	TIM3->CCER &= ~(0x0001<<1);
//	TIM3->CCER &= ~(0x0001<<5);
//	TIM3->SMCR |= 0x0003;
//	TIM3->CR1 |= 0x0001;

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

	if(EXTI_GetITStatus(EXTI_Line5) == SET){ 
		
	}
	EXTI_ClearITPendingBit(EXTI_Line5);
}

MOTO_DIR encoder_get_motor_dir(void)
{
	if((TIM3->CR1 & 0x0010) == 0x0010){
		return FORWARD;
	}else{
		return BACK;
	}
}

int32_t encoder_get_signal_cnt(void){
	int32_t cnt = 0;
	if(TIM3->CNT > ENCODER_ZERO_VAL){
		EncCnt = cnt = TIM3->CNT - ENCODER_ZERO_VAL;	
	}else{
		EncCnt = cnt = ENCODER_ZERO_VAL - TIM3->CNT;	
	}
	TIM_SetCounter(TIM3,ENCODER_ZERO_VAL);
	return cnt;
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
	uint16_t flag = 0x0001 << 4;
	if(TIM3->SR&(TIM_FLAG_Update)){		
		//down mode
		if((TIM3->CR1 & flag) == flag){
			N--;
		}else{
			//up mode
			N++;
		}
	}	
	TIM3->SR&=~(TIM_FLAG_Update);		
}

