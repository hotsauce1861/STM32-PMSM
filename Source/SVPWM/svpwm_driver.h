#ifndef SVPWM_DRIVER_H
#define	SVPWM_DRIVER_H
#include <stdint.h>

#define PWM_DUTY		50L		// 0--100
#define PWM_MAX_LIMIT	98L		
#define PWM_FRQ 		10000L
#define SYS_FRQ			72000000L

void pwm_init(void);
void pwm_reset_duty_cnt(uint8_t index, int16_t cnt);
int32_t get_pwm_period(void);
void pwm_disable(void);
void pwm_enable(void);
uint16_t pwm_get_max_limit(void);
uint16_t get_pwm_limit_period(void);

#endif
