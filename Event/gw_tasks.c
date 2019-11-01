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

#include "SDS.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USE_PID 		0
#define USE_CUR_PID		1
#define USE_SPEED_PID	0
#define PN				2

/* Private function prototypes -----------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct svpwm_module svpwm;

PID_Handle_t speed_pi;
PID_Handle_t cur_d_pi;
PID_Handle_t cur_q_pi;

Curr_Components cur_park_dq;
Curr_Components cur_clark_ab;
Curr_Components cur_ab;

Volt_Components vol_ab;
volatile Trig_Components trig_math;

int32_t Ia = 0;
int32_t Ib = 0;
int32_t Ic = 0;
int16_t Ia_ref = 0;
int16_t Ib_ref = 0;

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
	pwm_disable();
	
	encoder_init();
	svpwm.Tpwm = (uint16_t)get_pwm_period();
	svpwm.Udc = 8;
	cur_fbk_init();
	Ia_ref = cur_fbk_get_Ia_avl(10);
	Ib_ref = cur_fbk_get_Ib_avl(10);
	pwm_enable();
	volt_input.qV_Component1 = 0;
	volt_input.qV_Component2 = 20;
	vol_ab = park_rev(volt_input, (int32_t)270*Q15/360);
	svpwm.UAlpha = vol_ab.qV_Component1;
	svpwm.UBeta = vol_ab.qV_Component2;
	svpwm_main_run1(&svpwm);
	pwm_reset_duty_cnt(1, svpwm.Tcm1);
	pwm_reset_duty_cnt(2, svpwm.Tcm2);
	pwm_reset_duty_cnt(3, svpwm.Tcm3);

	/*
		software init
	*/
#if USE_PID
	PID_HandleInit(&speed_pi);
	speed_pi.hKpGain = 100;
	speed_pi.hKiGain = 0;
	speed_pi.hKdGain = 0;	
	speed_pi.hLowerOutputLimit = -100;
	speed_pi.hLowerOutputLimit = 100;
	PID_HandleInit(&cur_d_pi);
	cur_d_pi.hKpGain = 1;
	cur_d_pi.hKiGain = 0;
	cur_d_pi.hKdGain = 0;	
	cur_d_pi.hLowerOutputLimit = -100;
	cur_d_pi.hLowerOutputLimit = 100;	
	PID_HandleInit(&cur_q_pi);
	cur_q_pi.hKpGain = 1;
	cur_q_pi.hKiGain = 0;
	cur_q_pi.hKdGain = 0;	
	cur_q_pi.hLowerOutputLimit = -100;
	cur_q_pi.hLowerOutputLimit = 100;
#endif	
}

void task_main(void){
	
	Volt_Components volt_input;
	volt_input.qV_Component1 = 0;
	volt_input.qV_Component2 = 200;
	
	vol_ab = park_rev(volt_input, (int32_t)270*Q15/360);
	svpwm.UAlpha = vol_ab.qV_Component1;
	svpwm.UBeta = vol_ab.qV_Component2;
	svpwm_main_run1(&svpwm);
	pwm_reset_duty_cnt(1, svpwm.Tcm1);
	pwm_reset_duty_cnt(2, svpwm.Tcm2);
	pwm_reset_duty_cnt(3, svpwm.Tcm3);
		
}

void task_svpwm(void){

	static uint16_t i = 0;	
	uint8_t sector = 0;
	int16_t theta = 0;
	Volt_Components volt_input;
	int16_t pi_speed_out = 0;
	int16_t pi_id_out = 0;
	int16_t pi_iq_out = 0;
	
	int32_t speed_set = 1000;
	int32_t id_set = 0;
	int32_t uart_data[4];	
	//Volt_Components vlot;
	int32_t rpm = 0;
	int16_t agl = 0;
	sector = get_pos_rotor();
	rpm = encoder_get_signal_cnt();
	agl = theta = encoder_get_angular_pos();
//	Ia = (int32_t)cur_fbk_get_Ia()*330/4096 - 165;
//	Ib = (int32_t)cur_fbk_get_Ib()*330/4096 - 165;	

//Ia = (int32_t)cur_fbk_get_Ia() - 2048;	
//	Ib = (int32_t)cur_fbk_get_Ib() - 2048;	
	//Ia = (int32_t)cur_fbk_get_Ia() - Ia_ref;	
	//Ib = (int32_t)cur_fbk_get_Ib() - Ib_ref;	
	Ia = (int32_t)cur_fbk_get_Ia_avl(10) - 2048;	
	Ib = (int32_t)cur_fbk_get_Ib_avl(10) - 2048;	

#if USE_PID	
	cur_ab.qI_Component1 = Ia;
	cur_ab.qI_Component2 = Ib;

	cur_clark_ab = clarke(cur_ab);
	
	cur_park_dq = park(cur_clark_ab, agl);

	//cur_park_dq.qI_Component2;

	pi_speed_out = PI_Controller(&speed_pi,speed_set - rpm);
	
	pi_id_out = PI_Controller(&cur_d_pi,id_set - cur_park_dq.qI_Component1);

	pi_iq_out = PI_Controller(&cur_q_pi,pi_speed_out - cur_park_dq.qI_Component2);

	volt_input.qV_Component1 = pi_id_out + cur_park_dq.qI_Component2*1000;
	volt_input.qV_Component2 = pi_iq_out + cur_park_dq.qI_Component1*1000;

	vol_ab = park_rev(volt_input, agl);
	
//	trig_math = trig_functions(i);

	svpwm.UAlpha = vol_ab.qV_Component1;
	svpwm.UBeta = vol_ab.qV_Component2;	
#else
	trig_math = trig_functions(i);
	svpwm.UBeta = trig_math.hSin;
	svpwm.UAlpha = trig_math.hCos;
	
#endif
	svpwm_main_run2(&svpwm);
	pwm_reset_duty_cnt(1, svpwm.Tcm1);
	pwm_reset_duty_cnt(2, svpwm.Tcm2);
	pwm_reset_duty_cnt(3, svpwm.Tcm3);
	
	trig_math = trig_functions(agl);
	//uart_data[0] = svpwm.Tcm1;
	//uart_data[1] = svpwm.Tcm2;
	//uart_data[2] = svpwm.Tcm3;
	//uart_data[3] = svpwm.sector*1000;	
	
	//uart_data[0] = svpwm.UAlpha;
	//uart_data[1] = svpwm.UBeta;
	//uart_data[2] = agl;
	//uart_data[3] = svpwm.Tcm1;
	//SDS_OutPut_Data(uart_data);
	
	uart_data[0] = Ia;
	uart_data[1] = Ib;
	uart_data[2] = (Ia+Ib)-(Ia+Ib)/2;
	uart_data[3] = svpwm.Tcm1;	
	
	printf("%d,\n",Ia);
//SDS_OutPut_Data_INT(uart_data); 
	//pwm_reset_duty_cnt(1, svpwm.Tcm1);
	//pwm_reset_duty_cnt(2, svpwm.Tcm2);
	//pwm_reset_duty_cnt(3, svpwm.Tcm3);
//	printf("Sector:%d Ua:0x%08X Ub:0x%08X\n",svpwm.sector,svpwm.Ua,svpwm.Ub);
	i+=32;
}

#define USE_PRINTF 0
void task_idle(void){

	int32_t uart_data[4];	
	uart_data[0] = svpwm.Tcm1;
	uart_data[1] = svpwm.Tcm2;
	uart_data[2] = svpwm.Tcm3;
	uart_data[3] = svpwm.sector*1000;	

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


