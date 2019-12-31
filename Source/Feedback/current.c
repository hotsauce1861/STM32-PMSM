#include "current.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"

#include <stdint.h>

static void cur_fbk_irq_init(void){

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure and enable ADC interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

static void cur_fbk_adc_init(void){
	
	ADC_DeInit(ADC1);
	ADC_InitTypeDef ADC_InitStructure;
	//DMA_InitTypeDef   DMA_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 0;

#if 0
	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);

	/* Regular discontinuous mode channel number configuration */
	ADC_DiscModeChannelCountConfig(ADC1, 2);
	/* Enable regular discontinuous mode */
	ADC_DiscModeCmd(ADC1, ENABLE);
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1);
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_RegularConvertedValueTab;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 16;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
#else	
	
	/* Set injected sequencer length */
	ADC_InjectedSequencerLengthConfig(ADC1, 2);
	/* ADC1 injected channel Configuration */ 
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1Cycles5);
	//ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_1Cycles5);
	//ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_1Cycles5);	
	/* ADC1 injected external trigger configuration */
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
	//ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);
	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE); 
	//ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_1,2048);
	//ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_2,2048);
	
	/* Enable automatic injected conversion start after regular one */
	//ADC_AutoInjectedConvCmd(ADC1, ENABLE);
	
	/* Enable ADC1 external trigger */ 
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
#endif
	
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)){
		/**
			TODO
		*/
		//timeout_detect
	}
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)){
		/**
			TODO
		*/
		//timeout_detect
	}	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

static void cur_fbk_rcc_init(void){

	/* ADCCLK = PCLK2/6 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	  
	/* Enable peripheral clocks ------------------------------------------------*/
	/* Enable DMA1 and DMA2 clocks */
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);

	/* Enable ADC1 and GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
}

static void cur_fbk_pin_init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
  
}

static uint16_t cur_fbk_get_ad_val(uint8_t channel){
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5 );
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能软件转换功能
	//等待转换结束
	/*
 		EOC：转换结束位 (End of conversion)
		该位由硬件在(规则或注入)通道组转换结束时设置，由软件清除或由读取ADC_DR时清除
		0：转换未完成；
		1：转换完成。 		
	 */
	while((ADC1->SR & ADC_FLAG_EOC) != ADC_FLAG_EOC){}
	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )){}
	
	return ADC_GetConversionValue(ADC1); //返回最近一次 ADC1 规则组的转换结果
}
	
void cur_fbk_init(void){
	cur_fbk_rcc_init();
	cur_fbk_pin_init();
	cur_fbk_irq_init();
	cur_fbk_adc_init();
}

uint16_t cur_fbk_get_Ia_avl(uint8_t sample_times){
	uint32_t Ia_sum = 0;
	int8_t i;
	for(; i < sample_times; i++){
		Ia_sum+=cur_fbk_get_Ia();
	}
	return (uint16_t)(Ia_sum/sample_times);
}

uint16_t cur_fbk_get_Ib_avl(uint8_t sample_times){
	uint32_t Ib_sum = 0;
	int8_t i = 0;
	for(;i < sample_times; i++){
		Ib_sum+=cur_fbk_get_Ib();
	}
	return (uint16_t)(Ib_sum/sample_times);
}


uint16_t cur_fbk_get_Ia(void){
	return cur_fbk_get_ad_val(IA_CHANNEL_DRI);
}

uint16_t cur_fbk_get_Ib(void){
	return cur_fbk_get_ad_val(IB_CHANNEL_DRI);
}

int16_t cur_fbk_get_theta(void){
	return 0;
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles ADC1 and ADC2 global interrupts requests.
  * @param  None
  * @retval None
  */
volatile int16_t ad_irq = 0;
volatile uint16_t Ia_val = 0;
volatile uint16_t Ib_val = 0;

void set_inject_ia(uint16_t ia){
	Ia_val = ia;
}
uint16_t get_inject_ia(void){
	return Ia_val;
}

void set_inject_ib(uint16_t ib){
	Ib_val = ib;
}

uint16_t get_inject_ib(void){
	return Ib_val;
}


volatile int16_t ia_offset = 0;
volatile int16_t ib_offset = 0;
volatile int16_t ia_asc_offset = 0;
volatile int16_t ib_asc_offset = 0;

int16_t get_ia_offset(void){
	return ia_offset;
}

int16_t get_ib_offset(void){
	return ib_offset;
}

int16_t get_ia_asc_offset(void){
	return ia_asc_offset;
}

int16_t get_ib_asc_offset(void){
	return ib_asc_offset;
}
