#ifndef SVPWM_DRIVER_H
#define	SVPWM_DRIVER_H
#include <stdint.h>

#define PWM_DUTY	20L		// 0--100
#define PWM_FRQ 	5000L
#define SYS_FRQ		72000000L

void pwm_init(void);
void pwm_reset_duty_cnt(uint8_t index, int16_t cnt);
int32_t get_pwm_period(void);
void pwm_disable(void);
void pwm_enable(void);

#endif
