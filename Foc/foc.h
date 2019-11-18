#ifndef FOC_H
#define FOC_H
#include <stdint.h>
#include "pid_regulator.h"
#include "svpwm_module.h"
#include "svpwm_math.h"


struct motor_param {
	int16_t Pn;	
	int16_t In;
	int16_t Un;
	int16_t Flux;
};
typedef struct motor_param motor_param_t;

struct fdbk_mod {
	int16_t udc;
	int16_t theta;
	int16_t theta_offset;
	int16_t rpm;
	int16_t sector;

	int16_t ia;
	int16_t ib;		
};
typedef struct fdbk_mod fdbk_mod_t;

struct foc_mod {

	PID_Handle_t speed_pi;
	PID_Handle_t cur_d_pi;
	PID_Handle_t cur_q_pi;

	Curr_Components cur_park_dq;
	Curr_Components cur_clark_ab;
	Curr_Components cur_ab;

	Volt_Components vol_ab;
	Volt_Components vol_dq;
	Volt_Components pre_set_vol_dq;	
	svpwm_mod_t svpwm;
	motor_param_t motor_cfg;
	fdbk_mod_t feedback;
	int16_t angle_cnt;
	int16_t angle_cnt_detal;	
	int16_t rpm_speed_set;
};
typedef struct foc_mod foc_mod_t;

void foc_get_feedback(fdbk_mod_t * const pfbk);
void foc_motor_start(foc_mod_t * const foc,uint16_t timeout,
													void (* const time_cbk)(int16_t));


#endif
