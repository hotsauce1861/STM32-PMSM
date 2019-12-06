#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>

#define ALIGNMENT_ANGLE       	(uint16_t) 0 //Degrees [0..359] 
#define SPEED_SAMPLING_FREQ		200
#define ENCODER_ONE_CIRCLE_CNT	2160L
#define ENCODER_ZERO_CIRCLE_CNT	1580L
#define SAMPLE_FRQ 				10000L
#define SYS_FRQ					72000000L
#define ENCODER_MAX_CNT			0xFFFF
#define ENCODER_ZERO_VAL 		(int16_t)((int32_t)(ALIGNMENT_ANGLE)*65536/360)
//#define ENCODER_ZERO_VAL		0
#define START_SECTOR	5

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

/**
 *	@brief get angular position
 *	@retval angle in q1.15 format
 */
int16_t encoder_get_angular_pos(void);


void encoder_set_to_zero_position(void);

uint8_t encoder_get_zero_pos_flag(void);

int16_t encoder_get_first_zero_cnt(void);

int16_t encoder_get_timecounter_cnt(void);

void encoder_set_zero_pos_flag(uint8_t flag);

void encoder_clear_timercounter(void);

/**
 * @brief convert encoder count to angular posiong
 * @retval angle in format Q15
 */
int16_t encoder_conv_angular_pos(int16_t enc_cnt);

int16_t encoder_get_m_theta(void);

int16_t encoder_get_e_theta(void);

void encoder_reset_aligment(void);

void encoder_reset_zero(void);
	
void enconder_get_rpm(int16_t *pdata);

int16_t enconder_calc_rot_speed(void);

int16_t enconder_get_ave_speed(void);

#endif
