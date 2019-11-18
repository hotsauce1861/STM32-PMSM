#include <stdio.h>
#include <stdint.h>

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

#define USE_PID 		1
#define USE_CUR_PID		1
#define USE_SPEED_PID	1
#define PN				2

foc_mod_t foc_obj;

static void task_motor_run(void);
static void task_motor_config(void);


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
	svpwm_init();		
	encoder_init();
	cur_fbk_init();
	//sofeware init
	task_motor_config();
}
void task_motor_control(void* args){
    struct gw_event *pev = (struct gw_event *)args;
    gw_msg_fifo_t * const pmsg_fifo = &msg_fifo;
    gw_msg_t msg;
    if(gw_msg_receive(pev->id, pmsg_fifo, &msg) == GW_TRUE){
        /**
            TODO
        */               
        printf("%016d: name:[%s] id:[%d] msg:[%s]\n", pev->g_timer->timestamp,
               pev->name, pev->id,(uint8_t*)msg.pstr);
    }
	task_motor_run();
    pev->msg.pstr = "MSG from task_04: Hello I am task 04";
    if(gw_msg_send_msg(TASK_ID_DISPLAY, pmsg_fifo, &pev->msg) == GW_TRUE){
        /**
            TODO
        */
    }
}

static void task_motor_config(void){
	
	foc_obj.svpwm.Tpwm = (uint16_t)get_pwm_period();
	foc_obj.svpwm.Udc = 1;
	//*FREQ_task*60/2060;	
	foc_obj.rpm_speed_set = -100 * 2060 / (50 * 60);
	foc_obj.pre_set_vol_dq.qV_Component1 = 0;
	foc_obj.pre_set_vol_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)

	encoder_set_to_zero_position(); 
	foc_obj.feedback.sector = get_pos_rotor_2();
	foc_motor_start(&foc_obj, 5000, gw_hal_delay); 
	foc_obj.feedback.sector = get_pos_rotor_2();
	encoder_set_to_zero_position();
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
	foc_obj.cur_d_pi.hKpGain = 1;
	foc_obj.cur_d_pi.hKpDivisor = 2;
	foc_obj.cur_d_pi.hKiGain = 1;
	foc_obj.cur_d_pi.hKiDivisor = 64;
	foc_obj.cur_d_pi.hKdGain = 0;	
	foc_obj.cur_d_pi.hLowerOutputLimit = -32768;
	foc_obj.cur_d_pi.hUpperOutputLimit = 32767; 
	
	PID_HandleInit(&foc_obj.cur_q_pi);
	foc_obj.cur_q_pi.hKpGain = 1;
	foc_obj.cur_q_pi.hKpDivisor = 2;
	foc_obj.cur_q_pi.hKiGain = 1;
	foc_obj.cur_q_pi.hKiDivisor = 64;
	foc_obj.cur_q_pi.hKdGain = 0;	
	foc_obj.cur_q_pi.hLowerOutputLimit = -32768;
	foc_obj.cur_q_pi.hUpperOutputLimit = 32767; 
#endif	
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
	if(foc_obj.pre_set_vol_dq.qV_Component2 > 0){
		foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta*PN );
	}else{
		foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta*PN );
	}
	//cur_park_dq = park(cur_clark_ab, i);
	//cur_park_dq.qI_Component2;
	#if USE_SPEED_PID
	pi_speed_out = PI_Controller(&foc_obj.speed_pi,foc_obj.rpm_speed_set - foc_obj.feedback.rpm);
	foc_obj.pre_set_vol_dq.qV_Component2 = pi_speed_out;
	#endif	
	id_set = foc_obj.pre_set_vol_dq.qV_Component1;
	iq_set = foc_obj.pre_set_vol_dq.qV_Component2;
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
	if(foc_obj.pre_set_vol_dq.qV_Component2 > 0){		
		foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta*PN + INT16_MAX/2);
	}else{
		foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta*PN);
	}
	
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
	
	uart_data[0] = foc_obj.feedback.ia;
	uart_data[1] = foc_obj.feedback.sector*1000;
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


