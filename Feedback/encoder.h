#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>
/*
QPEA--->PA6/TIM3C1
QPEB--->PA7/TIM3C1
---------------------------
TIM3_UPDATE_IRQ
EXTI_PA5
---------------------------
*/
typedef enum{
	FORWARD = 0,
	BACK
}MOTO_DIR;

/**
 * 	@brief init encoder pin for pha pab and zero
 *		   and interrpts		
 */
void encoder_init(void);

/**
 *	@brief get encoder capture signal counts
 */
int32_t encoder_get_signal_cnt(void);

/**
 *	@brief get encoder running direction
 */
MOTO_DIR encoder_get_motor_dir(void); 

#endif
