#include <stdio.h>
#include <stdint.h>

#include "gw_timer.h"
#include "gw_tasks.h"
#include "svpwm_driver.h"
#include "svpwm_module.h"
#include "svpwm_math.h"
//#include "display_driver.h"
#include "usart_driver.h"
#include "svpwm_module.h"
#include "pid_regulator.h"
#include "encoder.h"
#include "current.h"
#include "position.h"
#include "foc.h"

#include "SDS.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USE_PID 		1
#define USE_CUR_PID		1
#define USE_SPEED_PID	1
#define PN				2
#define TEST_TASK		3
/* Private function prototypes -----------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
foc_module_typedef foc_obj;

volatile Trig_Components trig_math;

int32_t Ia = 0;
int32_t Ib = 0;
int32_t Ic = 0;

extern void Delay(__IO uint32_t nTime);
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @brief hardware init and software init
 */
void task_init(void){
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	*/
	Volt_Components volt_input;
	/*
		hardware init
	*/
	timer_base_config();
	usart_init();	
	svpwm_init();		
	encoder_init();
	foc_obj.svpwm.Tpwm = (uint16_t)get_pwm_period();
	foc_obj.svpwm.Udc = 1;
	foc_obj.rpm_speed_set = -200;
	cur_fbk_init();
//	foc_obj.vol_dq.qV_Component1 = 0;
//	foc_obj.vol_dq.qV_Component2 = -25000;
	foc_obj.pre_set_vol_dq.qV_Component1 = 0;
	foc_obj.pre_set_vol_dq.qV_Component2 = 25000;// 负数正转(顺时针)  正数反转(逆时针)

	encoder_set_to_zero_position();	
	foc_obj.feedback.sector = get_pos_rotor_2();
	foc_motor_start(&foc_obj, 5000, Delay); 
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

#if (TEST_TASK == 1)
/**
 * 	使用编码器反馈的角度代替帕克逆变换的角度输入
 *	输入的角度通过矫正
 */
void task_svpwm(void){
	int32_t uart_data[4];	
	static int16_t cnt = 0;
	Trig_Components angle;
	int16_t detal = 0;
	Volt_Components volt_out;
	foc_get_feedback(&foc_obj.feedback);
	volt_out = park_rev(foc_obj.pre_set_vol_dq,foc_obj.angle_cnt);

	if(foc_obj.pre_set_vol_dq.qV_Component2 > 0){
		detal = 18000;
		volt_out = park_rev(foc_obj.pre_set_vol_dq,foc_obj.feedback.theta*PN + detal);
	}else{
		detal = -14000;
		volt_out = park_rev(foc_obj.pre_set_vol_dq,foc_obj.feedback.theta*PN + detal);
	}	
	//angle = trig_functions(foc_obj.feedback.theta);
	foc_obj.svpwm.UAlpha = volt_out.qV_Component1;
	foc_obj.svpwm.UBeta = volt_out.qV_Component2;
	//foc_obj.svpwm.UAlpha = angle.hSin;
	//foc_obj.svpwm.UBeta = angle.hCos;
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);
	
	//uart_data[0] = foc_obj.feedback.sector*1000;
	//uart_data[1] = (int16_t)(foc_obj.feedback.theta - foc_obj.feedback.theta_offset)*PN;
	//uart_data[1] = (int16_t)foc_obj.feedback.theta*PN;
	//uart_data[2] = foc_obj.feedback.ia*500;
	//uart_data[3] = encoder_get_zero_pos_flag()*10000;	
	uart_data[0] = foc_obj.feedback.ia*10;
	uart_data[1] = foc_obj.feedback.theta_offset*PN + foc_obj.feedback.theta*PN;
	if(foc_obj.pre_set_vol_dq.qV_Component2 > 0){
		uart_data[2] = foc_obj.angle_cnt-=128;
	}else{
		uart_data[2] = foc_obj.angle_cnt+=128;
	}	
	uart_data[3] = foc_obj.feedback.theta*PN  + detal;
	//printf("%d,\n",Ia);
	SDS_OutPut_Data_INT(uart_data); 
	//printf("%d,",foc_obj.feedback.sector);	
}
#elif (TEST_TASK == 2)
void task_svpwm(void){
	int32_t uart_data[4];	
	static int16_t i = 0;
	foc_get_feedback(&foc_obj.feedback);

	Volt_Components volt_out;
	
	volt_out = park_rev(foc_obj.pre_set_vol_dq, i-=128);
	foc_obj.svpwm.UAlpha = volt_out.qV_Component1;
	foc_obj.svpwm.UBeta = volt_out.qV_Component2;
	
	//foc_obj.svpwm.UAlpha = volt_out.qV_Component2;
	//foc_obj.svpwm.UBeta = volt_out.qV_Component1;
	
	svpwm_main_run2(&foc_obj.svpwm);
	svpwm_reset_pwm_duty(&foc_obj.svpwm);

	//uart_data[0] = foc_obj.feedback.sector*1000;
	//uart_data[1] = (int16_t)(foc_obj.feedback.theta - foc_obj.feedback.theta_offset)*PN;
	//uart_data[1] = (int16_t)foc_obj.feedback.theta*PN;
	//uart_data[2] = foc_obj.feedback.ia*500;
	//uart_data[3] = encoder_get_zero_pos_flag()*10000;	
	uart_data[0] = foc_obj.feedback.ia*10;
	uart_data[1] = (foc_obj.feedback.theta + foc_obj.feedback.theta_offset)*PN;
	uart_data[2] = i;
	uart_data[3] = foc_obj.feedback.theta*PN;
	//printf("%d,\n",Ia);
	SDS_OutPut_Data_INT(uart_data); 
	//printf("%d,",foc_obj.feedback.sector);
	
}
/**
 *@brief main task change it in gw_fifo.c 
 */
#elif (TEST_TASK == 3)
void task_svpwm(void){

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
	uart_data[3] = foc_obj.feedback.rpm*100;//*FREQ_task*60/2060;	
	
	//printf("%d,\n",Ia);
	SDS_OutPut_Data_INT(uart_data); 
	//pwm_reset_duty_cnt(1, svpwm.Tcm1);
	//pwm_reset_duty_cnt(2, svpwm.Tcm2);
	//pwm_reset_duty_cnt(3, svpwm.Tcm3);
	//printf("Sector:%d Ua:0x%08X Ub:0x%08X\n", svpwm.sector,svpwm.Ua,svpwm.Ub);
	i+=32;
}
#endif


#define USE_PRINTF 0
void task_idle(void){

	int32_t uart_data[4];	

	//uart_data[0] = psvpwm->UAlpha;
	//uart_data[1] = psvpwm->UBeta;
	//uart_data[2] = psvpwm->Angle*100	
	//uart_data[3] = psvpwm->sector*1000;	
	
#if USE_PRINTF	
	//printf("Tcm1 is %d\n",uart_data[0]);
	//printf("Tcm2 is %d\n",uart_data[1]);
	//printf("Tcm3 is %d\n",uart_data[2]);
#else	
	//SDS_OutPut_Data_INT(uart_data);	
#endif
	SDS_OutPut_Data_INT(uart_data); 
	//printf("%d\n",uart_data[3]);
}

void task_get_rpm(void){
	
}


