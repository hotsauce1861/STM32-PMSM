#include "foc.h"
#include "encoder.h"
#include "position.h"
#include "current.h"
#include <stdint.h>
#include "stm32f10x.h"

const int16_t angle_table_forward[6] = {0x0000,0x2aaa,0x5554,0x8000,0xaaac,0xd556};
const int16_t angle_table_back[6] = {0xd556,0xaaac,0x8000,0x5554,0x2aaa,0x0000};

extern void Delay(__IO uint32_t nTime);

/*
void foc_tesk_run_forward(foc_module_typedef * const foc,uint16_t timeout,
													void (* const time_cbk)(int16_t)){

	while(){

	}
}	
*/

void foc_get_feedback(feedback_module_typedef * const pfbk){
	static int16_t sample_times = 0;
	const int16_t iref = 2048;	
	pfbk->ia = get_inject_ia() - iref;
	pfbk->ib = get_inject_ib() - iref;
	pfbk->sector = get_pos_rotor_2();
	pfbk->rpm = encoder_get_signal_cnt();
	//pfbk->theta = encoder_get_angular_pos() + 12800/2;
	pfbk->theta = encoder_get_angular_pos();	
	encoder_clear_timercounter();

}

void foc_motor_start(foc_module_typedef * const foc,uint16_t timeout,
													void (* const time_cbk)(int16_t)){
				
	uint8_t circle = 0;
	int16_t sector;
	int16_t cnt = 0;
	uint16_t time_cnt = 0;
	Volt_Components volt_out;
	int16_t tmp_cnt;
	int16_t detal = 0;
	uint8_t start_sector = 0;
#if 1
	if(foc->rpm_speed_set > 0){
#else														
	if(foc->pre_set_vol_dq.qV_Component2 > 0){
#endif
		detal = -64;
		//1-2-3-4-5-6
		start_sector = 1;
	}else{
		//6-5-4-3-2-1
		detal = 64;
		start_sector = 1;
	}
	while((sector = get_pos_rotor()) != start_sector){

		volt_out = park_rev(foc->pre_set_vol_dq, cnt+=detal);
		foc->svpwm.UAlpha = volt_out.qV_Component1;
		foc->svpwm.UBeta = volt_out.qV_Component2;

		svpwm_main_run2(&foc->svpwm);
		svpwm_reset_pwm_duty(&foc->svpwm);
		
		if(time_cbk != 0){
			time_cbk(1); // 1ms
		}
		if(time_cnt++>timeout){
			break;
		}
	}
	time_cnt = 0;
	tmp_cnt = encoder_get_timecounter_cnt();
	encoder_set_zero_pos_flag(0);
	#if 1
	//while(encoder_get_zero_pos_flag() != start_sector){
	while(encoder_get_zero_pos_flag() != 1){
		volt_out = park_rev(foc->pre_set_vol_dq, cnt+=detal);
		foc->svpwm.UAlpha = volt_out.qV_Component1;
		foc->svpwm.UBeta = volt_out.qV_Component2;

		svpwm_main_run2(&foc->svpwm);
		svpwm_reset_pwm_duty(&foc->svpwm);

		if(time_cbk != 0){
			time_cbk(1); // 1ms
		}
		if(time_cnt++>timeout){
			break;
		}
	}
	//foc->feedback.theta_offset = encoder_get_first_zero_cnt();	
	foc->feedback.theta_offset = encoder_get_timecounter_cnt() - tmp_cnt;
	#endif
}

/*
void foc_motor_get_zero_cnt(foc_module_typedef * const foc,uint16_t timeout,
													void (* const time_cbk)(int16_t)){

		while((sector = get_pos_rotor_2()) != 1){
		
		volt_out = park_rev(foc->vol_dq, cnt+=64);
		foc->svpwm.UAlpha = volt_out.qV_Component1;
		foc->svpwm.UBeta = volt_out.qV_Component2;

		svpwm_main_run2(&foc->svpwm);
		svpwm_reset_pwm_duty(&foc->svpwm);
		
		if(time_cbk != 0){
			time_cbk(1); // 1ms
		}
		if(time_cnt++>timeout || circle > 2){
			break;
		}
	}
}
*/
