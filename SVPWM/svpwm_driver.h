#ifndef SVPWM_DRIVER_H
#define	SVPWM_DRIVER_H
#include <stdint.h>

#define PWM_FRQ 	10000L
#define SYS_FRQ		SystemCoreClock

void pwm_init(void);
void pwm_reset_duty_cnt(uint8_t index, int16_t cnt);
int32_t get_pwm_period(void);

#endif
