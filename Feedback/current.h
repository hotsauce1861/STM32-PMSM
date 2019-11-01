#ifndef _CURRENT_H
#define _CURRENT_H
#include <stdint.h>

#define RES_IA	1024	//采样电阻A
#define RES_IB	1024	//采样电阻B

/**
 * Ia--->PA0/ADC0
 *	|------------>PA2/ADC2
 * Ib--->PA1/ADC1
 *	|------------>PA3/ADC3
 */

void cur_fbk_init(void);

uint16_t cur_fbk_get_Ia(void);
uint16_t cur_fbk_get_Ia_avl(uint8_t sample_times);
uint16_t cur_fbk_get_Ib(void);
uint16_t cur_fbk_get_Ib_avl(uint8_t sample_times);

uint16_t get_inject_ia(void);
uint16_t get_inject_ib(void);


int16_t cur_fbk_get_theta(void);

#endif
