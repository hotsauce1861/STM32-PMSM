#include "position.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


static void pos_rcc_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);		
}

static void pos_pin_init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

uint8_t get_pos_pha(void){
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
}

uint8_t get_pos_phb(void){
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
}

uint8_t get_pos_phc(void){
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);
}


void pos_init(void){
	pos_rcc_init();
	pos_pin_init();
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


