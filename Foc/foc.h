#ifndef FOC_H
#define FOC_H
#include <stdint.h>
#include "pid_regulator.h"
#include "svpwm_module.h"
#include "svpwm_math.h"
#include "feed_forward.h"

#define SPEED_CONTROL           (uint32_t)0x0001
#define FIRST_START             (uint32_t)0x0002
#define START_UP_FAILURE        (uint32_t)0x0004
#define SPEED_FEEDBACK          (uint32_t)0x0008
#define BRAKE_ON                (uint32_t)0x0010
#define OVERHEAT                (uint32_t)0x0100
#define OVER_CURRENT            (uint32_t)0x0200
#define OVER_VOLTAGE            (uint32_t)0x0400
#define UNDER_VOLTAGE           (uint32_t)0x0800

//

int16_t set_up_current(int16_t c);

/* default values for Torque control loop */
#define PID_TORQUE_REFERENCE   (int16_t)set_up_current(0) //(N.b: that's the reference init
				//value in both torque and speed control)
//Vq
#define PID_TORQUE_KP_DEFAULT  (int16_t)1
#define PID_TORQUE_KI_DEFAULT  (int16_t)1
#define PID_TORQUE_KD_DEFAULT  (int16_t)10

/* default values for Flux control loop */
//Vd
#define PID_FLUX_REFERENCE   (int16_t)set_up_current(500)

#define PID_FLUX_KP_DEFAULT  (int16_t)1
#define PID_FLUX_KI_DEFAULT  (int16_t)1
#define PID_FLUX_KD_DEFAULT  (int16_t)10

// Toruqe/Flux PID  parameter dividers
#define TF_KPDIV ((uint16_t)(0))
#define TF_KIDIV ((uint16_t)(0))
#define TF_KDDIV ((uint16_t)(8192))

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
	
	int16_t ia_asc;
	int16_t ib_asc;
	
	Curr_Components cur_offset;
};
typedef struct fdbk_mod fdbk_mod_t;

struct foc_mod {

	PID_Handle_t speed_pi;				// speed loop Id controller
	PID_Handle_t cur_d_pi;				// current loop Id controller
	PID_Handle_t cur_q_pi;				// current loop Iq controller
	PID_Handle_t position_pi;			// positon loop pi controller

	ff_mod_t	 ffc;					// feed forward controller
	
	Curr_Components cur_pre_set_dq;		// current loop reference dq value
	Curr_Components cur_park_dq;		// current loop feedback ab---park--->dq value	
	Curr_Components cur_clark_ab;		// current loop feedback AB---clark->ab value	
	Curr_Components cur_ab;				// current loop feedback AB value	
	
	Volt_Components vol_pre_set_dq;
	Volt_Components vol_ab;
	Volt_Components vol_dq;
	Volt_Components vol_pi_out;
	svpwm_mod_t svpwm;					// svpwm module
	motor_param_t motor_cfg;
	fdbk_mod_t feedback;
	int16_t angle_cnt;
	int16_t angle_cnt_detal;	
	int16_t rpm_speed_set;
	int16_t position_set;
	int16_t e_theta;
	
};
typedef struct foc_mod foc_mod_t;

void foc_get_feedback(fdbk_mod_t * const pfbk);

void foc_set_rpm_ref(foc_mod_t *p, int16_t data);
int16_t foc_get_rpm_ref(foc_mod_t *p);
void foc_set_position_ref(foc_mod_t *p, int16_t data);
int16_t foc_get_position_ref(foc_mod_t *p);

void foc_motor_start(foc_mod_t * const foc,uint16_t timeout,
					void (* const time_cbk)(int16_t));
void foc_start_up(foc_mod_t *foc,uint16_t timeout,
					void (* const time_cbk)(int16_t));

void foc_start_up_02(foc_mod_t *foc,uint16_t timeout,void (* const time_cbk)(int16_t));

#endif
