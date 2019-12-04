#ifndef _CURRENT_H
#define _CURRENT_H
#include <stdint.h>

#if 1
#define IA_CHANNEL_DRI 			ADC_Channel_0
//#define IA_INJECT_CHANNEL_DRI	ADC_InjectedChannel_1
#define IA_INJECT_CHANNEL_DRI	ADC_InjectedChannel_1
#define IB_CHANNEL_DRI 			ADC_Channel_1
//#define IB_INJECT_CHANNEL_DRI	ADC_InjectedChannel_2
#define IB_INJECT_CHANNEL_DRI	ADC_InjectedChannel_2
#else
#define IA_CHANNEL_DRI ADC_Channel_3
#define IB_CHANNEL_DRI ADC_Channel_2
#endif

#define RES_IA	1024	//采样电阻A
#define RES_IB	1024	//采样电阻B

/**
 *			OC1 ---> Ic	
 *			OC2 ---> Ib ---> PA0/ADC0
 *			OC3 ---> Ia ---> PA1/ADC1
 * Ib--->PA0/ADC0
 *	|------------>PA2/ADC2
 * Ia--->PA1/ADC1
 *	|------------>PA3/ADC3
 */

void cur_fbk_init(void);

uint16_t cur_fbk_get_Ia(void);
uint16_t cur_fbk_get_Ia_avl(uint8_t sample_times);
uint16_t cur_fbk_get_Ib(void);
uint16_t cur_fbk_get_Ib_avl(uint8_t sample_times);

uint16_t get_inject_ia(void);
uint16_t get_inject_ib(void);

void set_inject_ib(uint16_t ib);
void set_inject_ia(uint16_t ia);

int16_t cur_fbk_get_theta(void);

#endif
