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


foc_mod_t foc_obj;

static __maybe_unused void task_motor_open_loop(void);
static __maybe_unused void task_motor_speed_loop(void);
static __maybe_unused void task_motor_position_loop(void);
static __maybe_unused void task_motor_run_idzero(void);
static __maybe_unused void task_motor_run_iqzero(void);
static __maybe_unused void task_motor_run_iqidharf(void);
static __maybe_unused void task_motor_run_iq1id2(void);
static __maybe_unused void task_motor_run_iq2id1(void);
static __maybe_unused void task_motor_config(void);
static __maybe_unused void task_motor_park_clark(void);
void task_motor_at_zero_position(void *pargs);

void task_motor_stop(void);
void task_motor_control_init(void);
void task_motor_control(void* args);

void task_motor_control_create(){
	gw_task_args_create("motor_control", TASK_ID_MOTOR, 
					TYPE_POLL, GW_DISABLE, SPEED_PID_SAMPLE,
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
	
	//pid_config(2);
	pid_config(1);
	//sofeware init
	task_motor_config();
	foc_obj.feedback.cur_offset.qI_Component1 = 2020;
	foc_obj.feedback.cur_offset.qI_Component2 = 2048;
	foc_obj.position_set = 2500;
	foc_obj.rpm_speed_set = 50;
	
	foc_obj.ffc.a_factor = 40;
	foc_obj.ffc.b_factor = 80;
	
	encoder_init();
	encoder_reset_zero();
	cur_fbk_init();
	//stm_get_cur_state
	pos_init();
	pos_set_cbk(task_motor_at_zero_position, NULL);
	stm_set_next_state(&motor_state,START_UP);
	
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
	
	foc_obj.feedback.rpm = enconder_get_ave_speed();
	#if USE_POSITION_PID
	//task_motor_open_loop();	
	//task_motor_park_clark();
	//if(time_cnt++ % 2 == 0){
		task_motor_position_loop();	
	//}		
	#endif
	
	#if USE_FEED_FORWARD
	foc_obj.ffc.ein = foc_obj.position_set - TIM3->CNT;
	foc_obj.ffc.result = ff_calc_result(&foc_obj.ffc);
	foc_obj.rpm_speed_set += foc_obj.ffc.result;
	//foc_obj.+= foc_obj.ffc.result;
	//foc_obj.cur_pre_set_dq.qI_Component2 += foc_obj.ffc.result;	
	#endif	

	
	task_motor_speed_loop();
	if(time_cnt%5 == -1){
		usart_printf("$%d %d %d %d %d %d;",	foc_obj.position_set*10,
											TIM3->CNT*10,
											foc_obj.feedback.rpm*100,
											foc_obj.rpm_speed_set*100,
											foc_obj.cur_pre_set_dq.qI_Component2,
											foc_obj.cur_park_dq.qI_Component2);
	}	
	time_cnt++;
		
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

static void task_motor_config(void){
	/*
		software init		
	*/

	foc_obj.svpwm.Tpwm = (uint16_t)get_pwm_period();
	foc_obj.svpwm.Udc = 1;
	//*FREQ_task*60/2060;	
	foc_obj.rpm_speed_set = 120;
	
	foc_obj.cur_pre_set_dq.qI_Component1 = PID_FLUX_REFERENCE;
	foc_obj.cur_pre_set_dq.qI_Component2 = PID_TORQUE_REFERENCE;
	
	foc_obj.feedback.sector = get_pos_rotor_2();
	
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

static void task_motor_position_loop(void){

#if USE_POSITION_PID
	
	int16_t e_detal = 0;
	e_detal = foc_obj.position_set - TIM3->CNT;
	
	if(e_detal <= 100 && e_detal >= -100){
		foc_obj.position_pi.hKpGain = 1;
	}else if(e_detal <= 500 && e_detal >= -500){
		foc_obj.position_pi.hKpGain = 2;
	}else{
		foc_obj.position_pi.hKpGain = 3;
	}
	
	
	foc_obj.rpm_speed_set = PI_Controller(&foc_obj.position_pi, 
											foc_obj.position_set - TIM3->CNT);//foc_obj.feedback.theta);	

	//foc_obj.rpm_speed_set -= detal_spd/2;
	//if(foc_obj.rpm_speed_set < MIN_RPM || foc_obj.rpm_speed_set > MIN_RPM){
	//	foc_obj.rpm_speed_set = 0;
	//}
	/*
	if(foc_obj.rpm_speed_set > MAX_RPM){
		foc_obj.rpm_speed_set = MAX_RPM;
	}
	if(foc_obj.rpm_speed_set < -MAX_RPM){
		foc_obj.rpm_speed_set = -MAX_RPM;
	}
	*/	
#endif
}

static void task_motor_speed_loop(void){	
	uint8_t sector = 0;
	
	Volt_Components volt_input;
	int32_t pi_speed_out = 0;
	int16_t pi_id_out, pi_iq_out;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int32_t uart_data[4];	
	pi_speed_out = PI_Controller(&foc_obj.speed_pi, foc_obj.rpm_speed_set - foc_obj.feedback.rpm);
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
		
	
	uart_data[0] = foc_obj.position_set*10;//(int16_t)((int32_t)foc_obj.feedback.ia*Q14/2048*165/454*5);
	uart_data[1] = TIM3->CNT*10;//foc_obj.cur_park_dq.qI_Component2;
	uart_data[2] = foc_obj.feedback.rpm*100;
	uart_data[3] = foc_obj.rpm_speed_set*100;	
	
	//uart_data[0] = (int16_t)((int32_t)foc_obj.feedback.ia*Q14/2048*165/454*5);
	//uart_data[1] = (int16_t)((int32_t)foc_obj.feedback.ib*Q14/2048*165/454*5);//foc_obj.feedback.ib*10;
	//uart_data[2] = foc_obj.cur_park_dq.qI_Component1;
	//uart_data[3] = foc_obj.cur_park_dq.qI_Component2;//*50*60/2060;	
	
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
/**

*/
extern void task_motor_startup_02(Curr_Components cur_ab,uint16_t timeout){
	static int16_t cnt = 0;
	static int16_t angle = 0;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int16_t pi_id_out, pi_iq_out;
	Volt_Components volt_input;
	#define START_ZERO_SECTOR	3
	#if USE_POLL_MECH
	if( (get_pos_rotor() != START_ZERO_SECTOR) && cnt++ < timeout){
	#else	
	if	(cnt++ < timeout){
	#endif
		foc_obj.cur_ab = cur_ab;
	
		foc_obj.feedback.theta = encoder_get_e_theta();	
		//foc_obj.feedback.theta = (int16_t)(encoder_get_e_theta()+ (int32_t)foc_obj.angle_cnt_detal );
			
		foc_obj.cur_clark_ab = clarke(cur_ab);	
		foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta);
		
		id_set = foc_obj.cur_pre_set_dq.qI_Component1;
		iq_set = foc_obj.cur_pre_set_dq.qI_Component2;
		
		pi_id_out = PI_Controller(&foc_obj.cur_d_pi, id_set - foc_obj.cur_park_dq.qI_Component1);
		pi_iq_out = PI_Controller(&foc_obj.cur_q_pi, iq_set - foc_obj.cur_park_dq.qI_Component2);
	
	
		foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta);
		foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
		foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;	
		svpwm_main_run2(&foc_obj.svpwm);
		svpwm_reset_pwm_duty(&foc_obj.svpwm);			
	}
	#if USE_POLL_MECH
	else{
		task_motor_at_zero_position(NULL);
	}
	#endif
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
		get_pos_rotor_2();
		stm_set_next_state(&motor_state,RUN);
		encoder_reset_aligment();
		//encoder_reset_zero();
		foc_obj.cur_pre_set_dq.qI_Component1 = 0;
		foc_obj.cur_pre_set_dq.qI_Component2 = PID_FLUX_REFERENCE;
		cnt = 0;
	}	
}

void task_motor_at_zero_position(void *pargs){
	
	if(pargs == NULL){
		
	}	
	stm_set_next_state(&motor_state, RUN);
	encoder_reset_aligment();
	foc_obj.cur_pre_set_dq.qI_Component1 = 0;
	foc_obj.cur_pre_set_dq.qI_Component2 = PID_FLUX_REFERENCE;
}

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
	foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta);
	
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
	foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta);
	
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
	
	//wia = (int32_t)((int32_t)(ADC1->JDR1 - foc_obj.feedback.cur_offset.qI_Component1)*Q14/2048*165/454*5);
	//wib = (int32_t)((int32_t)(ADC1->JDR2 - foc_obj.feedback.cur_offset.qI_Component2)*Q14/2048*165/454*5);
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

void task_motor_stop(void){
	pwm_disable();
}


