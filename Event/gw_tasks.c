#include <stdio.h>
#include <stdint.h>

#include "gw_tasks.h"
#include "svpwm_driver.h"
#include "svpwm_module.h"
#include "svpwm_math.h"
#include "encoder.h"
#include "SDS.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct svpwm_module svpwm;
struct svpwm_module *psvpwm = &svpwm;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void task_svpwm(void){
	static int32_t i = 0;
	
	int32_t uart_data[4];
	//Ua = sine_table[i%10];
	//Ub = consine_table[i%10];
	//Ua = i+2;
	//Ub = i+3;
	psvpwm->UAlpha = sine_table[i%SINE_TABLE_SIZE]; 
	psvpwm->UBeta = consine_table[(i+270)%SINE_TABLE_SIZE];//需要限制幅度作处理			
	
	svpwm_nofloat_run(psvpwm);
	
	pwm_reset_duty_cnt(1, psvpwm->Tcm1);
	pwm_reset_duty_cnt(2, psvpwm->Tcm2);
	pwm_reset_duty_cnt(3, psvpwm->Tcm3);
	
	i++;
}

#define USE_PRINTF 0
void task_idle(void){

	int32_t uart_data[4];	
	uart_data[0] = psvpwm->Tcm1;
	uart_data[1] = psvpwm->Tcm2;
	uart_data[2] = psvpwm->Tcm3;
	//uart_data[3] = psvpwm->sector;	
	
#if USE_PRINTF	
	//printf("Tcm1 is %d\n",uart_data[0]);
	//printf("Tcm2 is %d\n",uart_data[1]);
	//printf("Tcm3 is %d\n",uart_data[2]);
#else	
	//SDS_OutPut_Data_INT(uart_data);	
#endif	
	//printf("task_idle\n");
}

void task_get_rpm(void){
	
	int32_t rpm = 0;
	rpm = encoder_get_signal_cnt();
}


