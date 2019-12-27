#include <stdio.h>
#include "position.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

int16_t phase_signal_angle_matrix[6] = {0xd556,0x2aaa,0x0000,0x8000,0xaaac,0x5554};

volatile uint8_t first_startup_flag = 1;
volatile PFUNC callback = NULL;
volatile void* pargs = NULL;

static void pos_rcc_init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);	
	
}

static void pos_pin_init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PHA | PHB | PHC;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

static void pos_irq_init(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

uint8_t get_pos_pha(void){
	return GPIO_ReadInputDataBit(GPIOB, PHA);
}

uint8_t get_pos_phb(void){
	return GPIO_ReadInputDataBit(GPIOB, PHB);
}

uint8_t get_pos_phc(void){
	return GPIO_ReadInputDataBit(GPIOB, PHC);
}


void pos_init(void){
	pos_rcc_init();
	pos_pin_init();
	pos_irq_init();
}

uint8_t get_pos_rotor(void){
	int8_t a,b,c;
	a = get_pos_pha();
	b = get_pos_phb();
	c = get_pos_phc();
	return 4*c+b*2+a;
}

uint8_t get_pos_rotor_2(void){
	int8_t a,b,c,sector;
	a = get_pos_pha();
	b = get_pos_phb();
	c = get_pos_phc();
	sector = 4*c+b*2+a;

	switch (sector)
		{
		case 1:
			sector = 2;			
			break;
		case 2:
			sector = 6;
			break;
		case 3:
			sector = 1;
			break;
		case 4:
			sector = 4;
			break;
		case 5:
			sector = 3;
			break;
		case 6:
			sector = 5;
			break;			
		}	
	return sector;
}

uint8_t get_pos_rotor_3(void){
	int8_t a,b,c,sector;
	a = get_pos_pha();
	b = get_pos_phb();
	c = get_pos_phc();
	sector = 4*c+b*2+a;

	switch (sector)
		{
		case 1:
			sector = 6;			
			break;
		case 2:
			sector = 2;
			break;
		case 3:
			sector = 1;
			break;
		case 4:
			sector = 4;
			break;
		case 5:
			sector = 5;
			break;
		case 6:
			sector = 3;
			break;			
		}	
	return sector;
}

void pos_set_cbk(PFUNC p,void *pargs){
	callback = p;
	pargs = pargs;
}
void EXTI0_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line0) == SET){ 
		//只在启动的时候开启
		if(first_startup_flag == 1){
					
			if(get_pos_pha() == INIT_PHA_VALUE 
			&& get_pos_phb() == INIT_PHB_VALUE
			&& get_pos_phc() == INIT_PHC_VALUE){
				if(callback != NULL){
					#if USE_POLL_MECH
					first_startup_flag = 0;
					callback((void *)pargs);
					#endif
				}				
			}	
		}
		// TODO disable exti here
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

