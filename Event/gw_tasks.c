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

#include "SDS.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct svpwm_module svpwm;
struct svpwm_module *psvpwm = &svpwm;

PID_Handle_t speed_pi;
PID_Handle_t cur_d_pi;
PID_Handle_t cur_q_pi;

Curr_Components cur_dq;
Curr_Components cur_ab;
Volt_Components vol_ab;
Trig_Components trig_math;

int32_t Ia = 0;
int32_t Ib = 0;

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
	
	/*
		hardware init
	*/
	timer_base_config();
	usart_init();
	svpwm_init();	

	//pwm_disable();
	cur_fbk_init();
	encoder_init();

	/*
		software init
	*/
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
}

void task_svpwm(void){
	static int32_t i = 0;

	int32_t uart_data[4];		
	int32_t rpm = 0;	
	
	rpm = encoder_get_signal_cnt();
	Ia = (int32_t)cur_fbk_get_Ia()*330/4096 - 165;
	Ib = (int32_t)cur_fbk_get_Ib()*330/4096 - 165;	
	
	trig_math =  trig_functions(i%360*Q15/360);
	psvpwm->UAlpha = trig_math.hSin;
	trig_math =  trig_functions((i+270)%360*Q15/360);
	psvpwm->UBeta = trig_math.hSin;	
	psvpwm->Angle = i%360;
	svpwm_nofloat_run(psvpwm);
	
//	pwm_reset_duty_cnt(1, psvpwm->Tcm1);
//	pwm_reset_duty_cnt(2, psvpwm->Tcm2);
//	pwm_reset_duty_cnt(3, psvpwm->Tcm3);
	
	i++;
}

#define USE_PRINTF 0
void task_idle(void){

	int32_t uart_data[4];	
	//uart_data[0] = psvpwm->Tcm1;
	//uart_data[1] = psvpwm->Tcm2;
	//uart_data[2] = psvpwm->Tcm3;
	//uart_data[3] = psvpwm->sector;	

	uart_data[0] = psvpwm->UAlpha;
	uart_data[1] = psvpwm->UBeta;
	uart_data[2] = psvpwm->Angle*100;
	uart_data[3] = psvpwm->sector;	

	
#if USE_PRINTF	
	//printf("Tcm1 is %d\n",uart_data[0]);
	//printf("Tcm2 is %d\n",uart_data[1]);
	//printf("Tcm3 is %d\n",uart_data[2]);
#else	
	//SDS_OutPut_Data_INT(uart_data);	
#endif
	SDS_OutPut_Data_INT(uart_data); 
	//printf("task_idle\n");
}

void task_get_rpm(void){
	
}


