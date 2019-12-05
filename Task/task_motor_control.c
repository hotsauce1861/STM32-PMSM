#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "task_motor_control.h"
#include "user_config.h"
#include "gw_tasks.h"

#include "svpwm_module.h"
#include "svpwm_driver.h"
#include "svpwm_driver.h"
#include "svpwm_module.h"
#include "svpwm_math.h"

#include "usart_driver.h"

#include "pid_regulator.h"
#include "encoder.h"
#include "current.h"
#include "position.h"
#include "foc.h"
#include "SDS.h"

#include "user_state_machine.h"

#define USE_PID 		1
#define USE_CUR_PID		1
#define USE_SPEED_PID	0
#define PN				2

foc_mod_t foc_obj;

static void task_motor_run(void);
static void task_motor_open_loop(void);
static void task_motor_speed_loop(void);
static void task_motor_run_idzero(void);
static void task_motor_run_iqzero(void);
static void task_motor_run_iqidharf(void);
static void task_motor_run_iq1id2(void);
static void task_motor_run_iq2id1(void);
static void task_motor_config(void);
static void task_motor_park_clark(void);

void task_motor_control_init(void);
void task_motor_control(void* args);

void task_motor_control_create(){
	gw_task_args_create("motor_control", TASK_ID_MOTOR, 
					TYPE_POLL, GW_DISABLE, 5,
					task_motor_control_init,
					task_motor_control);
}
/**
 * @brief Initialize motor control task 
 */
void task_motor_control_init(void){
	//hardware module 
	Log_d("motor","start init motor control task");
	
	state_machine_init(&motor_state);
	
	svpwm_init();		
	
	//sofeware init
	task_motor_config();
	foc_obj.feedback.cur_offset.qI_Component1 = 2020;
	foc_obj.feedback.cur_offset.qI_Component2 = 2048;
	encoder_init();
	encoder_reset_zero();
	cur_fbk_init();
	
}
void task_motor_control(void* args){
    struct gw_event *pev = (struct gw_event *)args;
    static int16_t time_cnt = 0;
	gw_msg_fifo_t * const pmsg_fifo = &msg_fifo;
    gw_msg_t msg;
    if(gw_msg_receive(pev->id, pmsg_fifo, &msg) == GW_TRUE){
        /**
            TODO
        */               
        printf("%016d: name:[%s] id:[%d] msg:[%s]\n", pev->g_timer->timestamp,
               pev->name, pev->id,(uint8_t*)msg.pstr);
    }
	//task_motor_run();
	// 100 ms get speed
	if(time_cnt++ > 200){
		time_cnt = 0;
		enconder_get_rpm(&foc_obj.feedback.rpm);
		foc_obj.feedback.rpm = foc_obj.feedback.rpm*60;
	}
	task_motor_open_loop();
	
	//task_motor_park_clark();
	//task_motor_speed_loop();
	//task_motor_run_idzero();
	//task_motor_run_iqzero();
	//task_motor_run_iqidharf();
	//task_motor_run_iq2id1();
	//task_motor_run_iq1id2();
	//foc_obj.feedback.sector = get_pos_rotor_2();
	//foc_obj.feedback.sector = get_pos_rotor();
	
    pev->msg.pstr = "MSG from task_04: Hello I am task 04";
    if(gw_msg_send_msg(TASK_ID_DISPLAY, pmsg_fifo, &pev->msg) == GW_TRUE){
        /**
            TODO
        */
    }
}
int16_t Kp = 10;
int16_t Kp_div = 1;
int16_t Ki = 1;
int16_t Ki_div = 1;
static void task_motor_config(void){
	int16_t	tmp_cnt = 0;
	/*
		software init		
	*/
#if USE_PID
	PID_HandleInit(&foc_obj.speed_pi);
	foc_obj.speed_pi.hKpGain = 220;
	foc_obj.speed_pi.hKiGain = 15;
	foc_obj.speed_pi.hKdGain = 0;	
	foc_obj.speed_pi.hLowerOutputLimit = -32768;
	foc_obj.speed_pi.hUpperOutputLimit = 32767;
	
	PID_HandleInit(&foc_obj.cur_d_pi);
	foc_obj.cur_d_pi.hKpGain = PID_FLUX_KP_DEFAULT;
	foc_obj.cur_d_pi.hKiGain = PID_FLUX_KI_DEFAULT;
//	foc_obj.cur_d_pi.hKpGain = Kp;
//	foc_obj.cur_d_pi.hKiGain = Ki;
	foc_obj.cur_d_pi.hKpDivisor = TF_KPDIV;
	foc_obj.cur_d_pi.hKiDivisor = TF_KIDIV;
	foc_obj.cur_d_pi.hKdGain = 0;	
	foc_obj.cur_d_pi.hLowerOutputLimit = -32768;
	foc_obj.cur_d_pi.hUpperOutputLimit = 32767; 
	
	PID_HandleInit(&foc_obj.cur_q_pi);
	//foc_obj.cur_q_pi.hKpGain = Kp;
	//foc_obj.cur_q_pi.hKiGain = Ki;
	//foc_obj.cur_q_pi.hKpDivisor = Kp_div;
	//foc_obj.cur_q_pi.hKiDivisor = Ki_div;
	foc_obj.cur_q_pi.hKpGain = PID_TORQUE_KP_DEFAULT;
	foc_obj.cur_q_pi.hKiGain = PID_TORQUE_KI_DEFAULT;
	foc_obj.cur_q_pi.hKpDivisor = TF_KPDIV;
	foc_obj.cur_q_pi.hKiDivisor = TF_KIDIV;	
	foc_obj.cur_q_pi.hKdGain = 0;	
	foc_obj.cur_q_pi.hLowerOutputLimit = -32768;
	foc_obj.cur_q_pi.hUpperOutputLimit = 32767; 
#endif	

	foc_obj.svpwm.Tpwm = (uint16_t)get_pwm_period();
	foc_obj.svpwm.Udc = 1;
	//*FREQ_task*60/2060;	
	foc_obj.rpm_speed_set = 120;
	
	foc_obj.cur_pre_set_dq.qI_Component1 = PID_FLUX_REFERENCE;
	foc_obj.cur_pre_set_dq.qI_Component2 = PID_TORQUE_REFERENCE;
	
	//foc_obj.vol_pre_set_dq.qV_Component1 = 0;
	//foc_obj.vol_pre_set_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)
	//foc_obj.pre_set_vol_dq.qV_Component1 = PI_Controller(&foc_obj.speed_pi,foc_obj.rpm_speed_set - 0);

	//encoder_set_to_zero_position(); 
	foc_obj.feedback.sector = get_pos_rotor_2();
	//foc_motor_start2(&foc_obj, 5000, gw_hal_delay); 
	//foc_start_up(&foc_obj, 5000, gw_hal_delay); 
	//foc_start_up_02(&foc_obj, 500, gw_hal_delay); 
	//foc_obj.feedback.sector = get_pos_rotor_2();
	//encoder_set_to_zero_position();
	//task_motor_run_idzero();
	//encoder_clear_timercounter();
	//task_motor_run_iqzero();
	//tmp_cnt = encoder_get_timecounter_cnt();
	//task_motor_run_iqidharf();
	//task_motor_run_iq2id1();
	//task_motor_run_iq1id2();
	//foc_obj.feedback.sector = get_pos_rotor_2();
	//foc_obj.feedback.sector = get_pos_rotor();
}
/**
 * @brief test for motor park and clark 
 *
 */
int16_t set_angle = 0;
static void task_motor_park_clark(void){
	static int16_t cnt = 0;
	int32_t uart_data[4];	
	int32_t detal = 0x5555;
	Trig_Components cur_a,cur_b;
	Curr_Components Curr_Input;
	Curr_Components Curr_Output_clark;
	Curr_Components Curr_Output_dq;
	cur_a = trig_functions( cnt+=64);
	cur_b = trig_functions( (int16_t)(cnt - detal));
	Curr_Input.qI_Component1 = cur_a.hSin;
	Curr_Input.qI_Component2 = cur_b.hSin;
	
	Curr_Output_clark =	clarke(Curr_Input);
	Curr_Output_dq = park(Curr_Output_clark,cnt + set_angle);	
	
	uart_data[0] = Curr_Input.qI_Component1;
	uart_data[1] = Curr_Input.qI_Component2;
	uart_data[2] = Curr_Output_dq.qI_Component1;
	uart_data[3] = Curr_Output_dq.qI_Component2;
	
	SDS_OutPut_Data_INT(uart_data); 
	
}

static void task_motor_run_iq1id2(void){
	foc_obj.vol_pre_set_dq.qV_Component1 = 25000;
	foc_obj.vol_pre_set_dq.qV_Component2 = 12500;// 负数正转(顺时针)  正数反转(逆时针)
	
	foc_obj.vol_ab = park_rev(foc_obj.vol_pre_set_dq, 0);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;		
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
}

static void task_motor_run_iq2id1(void){
	foc_obj.vol_pre_set_dq.qV_Component1 = 12500;
	foc_obj.vol_pre_set_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)
	
	foc_obj.vol_ab = park_rev(foc_obj.vol_pre_set_dq, 0);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;		
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
}

static void task_motor_run_iqidharf(void){
	foc_obj.vol_pre_set_dq.qV_Component1 = 25000;
	foc_obj.vol_pre_set_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)
	
	foc_obj.vol_ab = park_rev(foc_obj.vol_pre_set_dq, 0);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;		
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
}

static void task_motor_run_idzero(void){
	foc_obj.vol_pre_set_dq.qV_Component1 = 0;
	foc_obj.vol_pre_set_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)
	
	foc_obj.vol_ab = park_rev(foc_obj.vol_pre_set_dq, 0);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;		
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
	
}

static void task_motor_run_iqzero(void){
	foc_obj.vol_pre_set_dq.qV_Component1 = 25000;
	foc_obj.vol_pre_set_dq.qV_Component2 = 0;// 负数正转(顺时针)  正数反转(逆时针)
	
	foc_obj.vol_ab = park_rev(foc_obj.vol_pre_set_dq, foc_obj.e_theta);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;		
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
}

static void task_motor_speed_loop(void){
	static uint16_t i = 0;	
	uint8_t sector = 0;
	
	Volt_Components volt_input;
	int32_t pi_speed_out = 0;
	int16_t pi_id_out, pi_iq_out;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int32_t uart_data[4];	
	pi_speed_out = PI_Controller(&foc_obj.speed_pi,foc_obj.rpm_speed_set - foc_obj.feedback.rpm);
	//foc_obj.vol_pre_set_dq.qV_Component2 = pi_speed_out;
	if(pi_speed_out >= INT16_MAX){
		pi_speed_out = INT16_MAX;
	}
	if(pi_speed_out <= INT16_MIN){
		pi_speed_out = INT16_MIN;
	}
	foc_obj.cur_pre_set_dq.qI_Component2 = (int16_t)pi_speed_out;
	foc_get_feedback(&foc_obj.feedback);
	
#if 1	 	
	uart_data[0] = (int16_t)((int32_t)foc_obj.feedback.ia*Q14/2048*165/454*5);
	uart_data[1] = (int16_t)((int32_t)foc_obj.feedback.ib*Q14/2048*165/454*5);//foc_obj.feedback.ib*10;
	uart_data[2] = foc_obj.cur_park_dq.qI_Component1;
	uart_data[3] = foc_obj.cur_park_dq.qI_Component2;//*50*60/2060;	
	
	//uart_data[0] = foc_obj.cur_pre_set_dq.qI_Component1;
	//uart_data[1] = foc_obj.cur_park_dq.qI_Component1;
	//uart_data[2] = foc_obj.cur_pre_set_dq.qI_Component2;
	//uart_data[3] = foc_obj.cur_park_dq.qI_Component2;//*50*60/2060;	
	#else
	uart_data[0] = foc_obj.feedback.theta_offset;
	uart_data[1] = foc_obj.e_theta;//foc_obj.feedback.ib*10;
	uart_data[2] = (int16_t)((int32_t)(foc_obj.feedback.theta) - foc_obj.e_theta);
	uart_data[3] = (int32_t)(foc_obj.feedback.theta);//*50*60/2060;		
	#endif
	//usart_printf("%d,",foc_obj.feedback.ia);
	SDS_OutPut_Data_INT(uart_data); 		
	
}

extern void task_motor_startup(Curr_Components cur_ab,uint16_t timeout){
	static int16_t cnt = 0;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int16_t pi_id_out, pi_iq_out;
	Volt_Components volt_input;
	
	if(cnt++ < timeout){
		foc_obj.cur_ab = cur_ab;
		//foc_obj.e_theta = ENCODER_ZERO_VAL;
		foc_obj.feedback.theta = 0;
		//foc_obj.feedback.theta = ENCODER_ZERO_VAL;//(int16_t)(encoder_get_e_theta()+ (int32_t)foc_obj.angle_cnt_detal );
		//foc_obj.feedback.theta = (int16_t)(encoder_get_e_theta()+ (int32_t)foc_obj.angle_cnt_detal );
			
		foc_obj.cur_clark_ab = clarke(cur_ab);	
		foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta);
		
		id_set = foc_obj.cur_pre_set_dq.qI_Component1;
		iq_set = foc_obj.cur_pre_set_dq.qI_Component2;
		
		pi_id_out = PI_Controller(&foc_obj.cur_d_pi, id_set - foc_obj.cur_park_dq.qI_Component1);
		pi_iq_out = PI_Controller(&foc_obj.cur_q_pi, iq_set - foc_obj.cur_park_dq.qI_Component2);
	
	//	volt_input.qV_Component1 = pi_id_out;// + foc_obj.cur_park_dq.qI_Component2 * 1000;
	//	volt_input.qV_Component2 = pi_iq_out;// + foc_obj.cur_park_dq.qI_Component1 * 1000;
	//	volt_input.qV_Component1 = foc_obj.cur_pre_set_dq.qI_Component1;
	//	volt_input.qV_Component2 = foc_obj.cur_pre_set_dq.qI_Component2;
		foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta);
		foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
		foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;	
		svpwm_main_run2(&foc_obj.svpwm);
		svpwm_reset_pwm_duty(&foc_obj.svpwm);	
		
	}else{
		stm_set_next_state(&motor_state,RUN);
		encoder_reset_aligment();
		//encoder_reset_zero();
		foc_obj.cur_pre_set_dq.qI_Component1 = 0;
		foc_obj.cur_pre_set_dq.qI_Component2 = PID_FLUX_REFERENCE;
		cnt = 0;
	}	
}
int16_t user_angle = 0;
extern void task_motor_cur_loop(Curr_Components cur_ab){
	static int16_t cnt = 0;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int16_t pi_id_out, pi_iq_out;
	Volt_Components volt_input;
	//foc_obj.cur_pre_set_dq.qI_Component1 = 15000;
	//foc_obj.cur_pre_set_dq.qI_Component2 = 0;
	foc_obj.cur_ab = cur_ab;
	//foc_obj.feedback.theta = (int16_t)(encoder_get_e_theta()+ (int32_t)foc_obj.angle_cnt_detal );
	foc_obj.feedback.theta = encoder_get_e_theta();
	//foc_obj.feedback.theta = cnt+=256;
	//foc_obj.feedback.theta = foc_obj.e_theta;
	foc_obj.cur_clark_ab = clarke(cur_ab);	
	foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta + user_angle);
	
	id_set = (int32_t)foc_obj.cur_pre_set_dq.qI_Component1;
	iq_set = (int32_t)foc_obj.cur_pre_set_dq.qI_Component2;
	
	pi_id_out = PI_Controller(&foc_obj.cur_d_pi, id_set - foc_obj.cur_park_dq.qI_Component1);
	pi_iq_out = PI_Controller(&foc_obj.cur_q_pi, iq_set - foc_obj.cur_park_dq.qI_Component2);
	//pi_iq_out = 0;
	volt_input.qV_Component1 = pi_id_out;
	volt_input.qV_Component2 = pi_iq_out;
	foc_obj.vol_pi_out.qV_Component1 = pi_id_out;
	foc_obj.vol_pi_out.qV_Component2 = pi_iq_out;
	//foc_obj.vol_pi_out.qV_Component1 = 0;
	//foc_obj.vol_pi_out.qV_Component2 = 25000;
	foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta + user_angle);
	
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;	
	
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
}

static void task_motor_open_loop(void){
	static uint16_t i = 0;	
	static int16_t cnt = 0;
	uint8_t sector = 0;
	int16_t detal = 0;
	Volt_Components volt_input;
	int16_t pi_speed_out = 0;
	int16_t pi_id_out, pi_iq_out;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int32_t uart_data[4];		
	foc_get_feedback(&foc_obj.feedback);
	#if 0
	volt_input.qV_Component1 = 0;
	volt_input.qV_Component2 = foc_obj.cur_pre_set_dq.qI_Component2;
	if(volt_input.qV_Component2>0){
		detal = 128;
	}else{
		detal = -128;
	}
	//vol_ab = park_rev(volt_input, agl*PN);
	//foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta+=detal);
	foc_obj.vol_ab = park_rev(volt_input, cnt+=detal);

	//foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta);
	//trig_math = trig_functions(i);
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;
	
	/*
		计算SVPWM波形
	*/
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
	#endif
	
	#if 1	 	
	/* 
	//对比电流
	uart_data[0] = foc_obj.feedback.ia;
	uart_data[1] = foc_obj.feedback.ib; 
	uart_data[2] = (foc_obj.feedback.ia_asc - 1539)*454/50;
	uart_data[3] = (foc_obj.feedback.ib_asc - 1538)*454/50;
	*/
	
	//uart_data[0] = TIM1->ARR;
	//uart_data[1] = TIM1->CCR1; 
	//uart_data[2] = TIM1->CCR2;
	//uart_data[3] = TIM1->CCR3;
	
	
	uart_data[0] = (int16_t)((int32_t)foc_obj.feedback.ia*Q14/2048*165/454*5);
	uart_data[1] = (int16_t)((int32_t)foc_obj.feedback.ib*Q14/2048*165/454*5);//foc_obj.feedback.ib*10;
	uart_data[2] = foc_obj.cur_park_dq.qI_Component1;
	uart_data[3] = foc_obj.cur_park_dq.qI_Component2;
	
	//uart_data[0] = foc_obj.cur_pre_set_dq.qI_Component1;
	//uart_data[1] = foc_obj.cur_park_dq.qI_Component1;
	//uart_data[2] = foc_obj.cur_pre_set_dq.qI_Component2;
	//uart_data[3] = foc_obj.cur_park_dq.qI_Component2;//*50*60/2060;
	#else
	uart_data[0] = foc_obj.feedback.theta_offset;
	uart_data[1] = foc_obj.e_theta;//foc_obj.feedback.ib*10;
	uart_data[2] = (int16_t)((int32_t)(foc_obj.feedback.theta) - foc_obj.e_theta);
	uart_data[3] = (int32_t)(foc_obj.feedback.theta);//*50*60/2060;		
	#endif
	//usart_printf("%d,",foc_obj.feedback.ia);
	SDS_OutPut_Data_INT(uart_data); 	
}

static void task_motor_run(void){
	
	static uint16_t i = 0;	
	uint8_t sector = 0;
	
	Volt_Components volt_input;
	int16_t pi_speed_out = 0;
	int16_t pi_id_out, pi_iq_out;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int32_t uart_data[4];		

#if USE_PID	

	#if USE_CUR_PID	
	foc_obj.cur_ab.qI_Component1 = foc_obj.feedback.ia;
	foc_obj.cur_ab.qI_Component2 = foc_obj.feedback.ib;
	
	foc_obj.cur_clark_ab = clarke(foc_obj.cur_ab);	
	
	foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab,
					(foc_obj.feedback.theta + foc_obj.feedback.theta_offset)*PN );
	//cur_park_dq = park(cur_clark_ab, i);
	//cur_park_dq.qI_Component2;
	#if USE_SPEED_PID
	pi_speed_out = PI_Controller(&foc_obj.speed_pi,foc_obj.rpm_speed_set - foc_obj.feedback.rpm);
	foc_obj.pre_set_vol_dq.qV_Component2 = pi_speed_out;
	#endif	

	id_set = foc_obj.cur_pre_set_dq.qI_Component1;
	iq_set = foc_obj.cur_pre_set_dq.qI_Component2;
	#if 1
	pi_id_out = PI_Controller(&foc_obj.cur_d_pi, id_set - foc_obj.cur_park_dq.qI_Component1);
	pi_iq_out = PI_Controller(&foc_obj.cur_q_pi, iq_set - foc_obj.cur_park_dq.qI_Component2);
	#else
	pi_id_out = id_set;
	pi_iq_out = iq_set;
	#endif
	volt_input.qV_Component1 = pi_id_out;// + foc_obj.cur_park_dq.qI_Component2 * 1000;
	volt_input.qV_Component2 = pi_iq_out;// + foc_obj.cur_park_dq.qI_Component1 * 1000;

	/**
		帕克反变换
	 */
	
	foc_obj.vol_ab = park_rev(volt_input, 
				(foc_obj.feedback.theta + foc_obj.feedback.theta_offset)*PN);
	foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
	foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;	
	#endif
#else
	volt_input.qV_Component1 = 0;
	volt_input.qV_Component2 = 25000;

	//vol_ab = park_rev(volt_input, agl*PN);
	vol_ab = park_rev(volt_input, i);
	//trig_math = trig_functions(i);
	svpwm.UBeta = vol_ab.qV_Component1;
	svpwm.UAlpha = vol_ab.qV_Component2;
	
#endif
	/*
		计算SVPWM波形
	*/
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
	
	foc_get_feedback(&foc_obj.feedback);
 
	//uart_data[0] = svpwm.Tcm1;
	//uart_data[1] = svpwm.Tcm2;
	//uart_data[2] = svpwm.Tcm3;
	//uart_data[3] = svpwm.sector*1000;	
	
	//uart_data[0] = Ia*20;
	//uart_data[1] = Ib*20;
	//uart_data[2] = i;
	//uart_data[3] = agl*PN;
	//SDS_OutPut_Data(uart_data);

	//uart_data[0] = i;
	//uart_data[1] = agl*PN;
	//uart_data[2] = pi_id_out;
	//uart_data[3] = pi_iq_out;	
	
	uart_data[0] = foc_obj.feedback.ia*10;
	uart_data[1] = foc_obj.feedback.ib*10;
	uart_data[2] = foc_obj.feedback.theta*PN;
	uart_data[3] = foc_obj.feedback.rpm*150;//*50*60/2060;	
	
	//printf("%d,\n",Ia);
	SDS_OutPut_Data_INT(uart_data); 
	//pwm_reset_duty_cnt(1, svpwm.Tcm1);
	//pwm_reset_duty_cnt(2, svpwm.Tcm2);
	//pwm_reset_duty_cnt(3, svpwm.Tcm3);
	//printf("Sector:%d Ua:0x%08X Ub:0x%08X\n", svpwm.sector,svpwm.Ua,svpwm.Ub);
	
}

static void task_motor_stop(void){
	pwm_disable();
}


