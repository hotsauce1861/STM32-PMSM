#include "current.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include <stdint.h>

#define IA_CHANNEL_DRI ADC_Channel_0
#define IB_CHANNEL_DRI ADC_Channel_1

static void cur_fbk_irq_init(void){

}

static void cur_fbk_adc_init(void){

	ADC_DeInit(ADC1);
	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
		
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

uint16_t cur_fbk_get_Ia(void){
	return cur_fbk_get_ad_val(IA_CHANNEL_DRI);
}

uint16_t cur_fbk_get_Ib(void){
	return cur_fbk_get_ad_val(IB_CHANNEL_DRI);
}


