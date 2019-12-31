#include "foc.h"

#if USE_STM32_ENCODER
#include "stm32f10x_encoder.h"
#else
#include "encoder.h"
#endif
#include "position.h"
#include "current.h"
#include <stdint.h>
#include "stm32f10x.h"

#include "user_config.h"
#include "user_state_machine.h"

const int16_t angle_table_forward[6] = {0x0000,0x2aaa,0x5554,0x8000,0xaaac,0xd556};
const int16_t angle_table_back[6] = {0xd556,0xaaac,0x8000,0x5554,0x2aaa,0x0000};

volatile uint32_t wGlobal_Flags = FIRST_START;

extern void hal_delay(__IO uint32_t nTime);

inline int16_t set_up_current(int16_t c){
	int32_t wtmp = 0;
	wtmp = c*Q14/1000;
	return (int16_t)wtmp;	 //x -2000mA -- 2000mA
}
/*
void foc_tesk_run_forward(foc_module_tst foc,uint16_t timeout,
													void (* const time_cbk)(int16_t)){

	while(){

	}
}	
*/
extern foc_mod_t foc_obj;

void foc_set_rpm_ref(foc_mod_t *p, int16_t data){
	p->rpm_speed_set = data;
	if(p->rpm_speed_set > MAX_RPM){
		p->rpm_speed_set = MAX_RPM;
	}
	if(p->rpm_speed_set < -MAX_RPM){
		p->rpm_speed_set = -MAX_RPM;
	}
}

int16_t foc_get_rpm_ref(foc_mod_t *p){
	return p->rpm_speed_set;
}

void foc_set_position_ref(foc_mod_t *p, int16_t data){
	p->position_set = data;
	if(p->position_set > MAX_POSITION){
		p->position_set = MAX_POSITION;
	}
	if(p->position_set < -MIN_POSITION){
		p->position_set = -MIN_POSITION;
	}
}

int16_t foc_get_position_ref(foc_mod_t *p){
	return p->position_set;
}

void foc_start_up_03(Curr_Components cur_ab,uint16_t timeout){
	static int16_t cnt = 0;
	int32_t id_set = 0;
	int32_t iq_set = 0;
	int16_t pi_id_out, pi_iq_out;
	Volt_Components volt_input;
	
	if(cnt++ < timeout){
		foc_obj.cur_ab = cur_ab;
		foc_obj.feedback.theta = (int16_t)(encoder_get_e_theta()+ (int32_t)foc_obj.angle_cnt_detal );
	
		foc_obj.cur_clark_ab = clarke(cur_ab);	
		foc_obj.cur_park_dq = park(foc_obj.cur_clark_ab, foc_obj.feedback.theta);
		
		id_set = foc_obj.cur_pre_set_dq.qI_Component1;
		iq_set = foc_obj.cur_pre_set_dq.qI_Component2;
		
		pi_id_out = PI_Controller(&foc_obj.cur_d_pi, id_set - foc_obj.cur_park_dq.qI_Component1);
		pi_iq_out = PI_Controller(&foc_obj.cur_q_pi, iq_set - foc_obj.cur_park_dq.qI_Component2);
		volt_input.qV_Component1 = pi_id_out;// + foc_obj.cur_park_dq.qI_Component2 * 1000;
		volt_input.qV_Component2 = pi_iq_out;// + foc_obj.cur_park_dq.qI_Component1 * 1000;
		
		foc_obj.vol_ab = park_rev(volt_input, foc_obj.feedback.theta);
		foc_obj.svpwm.UAlpha = foc_obj.vol_ab.qV_Component1;
		foc_obj.svpwm.UBeta = foc_obj.vol_ab.qV_Component2;	
		svpwm_main_run2(&foc_obj.svpwm);
		svpwm_reset_pwm_duty(&foc_obj.svpwm);	
	}else{
		
	}	
}
void foc_start_up_02(foc_mod_t *foc,uint16_t timeout,void (* const time_cbk)(int16_t)){

	uint8_t circle = 0;
	int16_t sector;
	int16_t cnt = 0;
	uint16_t time_cnt = 0;
	Volt_Components volt_out;
	int16_t tmp_cnt;
	int16_t reg_cnt = 0;
	int16_t detal_cnt = 0;
	int16_t detal = 0;
	uint8_t start_sector = 0;
	foc->vol_pre_set_dq.qV_Component1 = foc->cur_pre_set_dq.qI_Component1;
	foc->vol_pre_set_dq.qV_Component2 = foc->cur_pre_set_dq.qI_Component2;
#if 0
	
	if(foc->rpm_speed_set > 0){
#else														
	if(foc->cur_pre_set_dq.qI_Component2 > 0){
#endif
		detal = -64;
		//1-2-3-4-5-6
		start_sector = START_SECTOR;
	}else{
		//6-5-4-3-2-1
		detal = 64;
		start_sector = START_SECTOR;
	}
	
	while(1){
				//ENCODER_ZERO_VAL
		volt_out = park_rev(foc->vol_pre_set_dq, 0);
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
	
	foc->feedback.theta_offset = encoder_get_e_theta();	
	encoder_reset_aligment();
	foc->e_theta = encoder_get_e_theta();
}


void foc_start_up(foc_mod_t *foc,uint16_t timeout,void (* const time_cbk)(int16_t)){

	uint8_t circle = 0;
	int16_t sector;
	int16_t cnt = 0;
	uint16_t time_cnt = 0;
	Volt_Components volt_out;
	int16_t tmp_cnt;
	int16_t reg_cnt = 0;
	int16_t detal_cnt = 0;
	int16_t detal = 0;
	uint8_t start_sector = 0;
	foc->vol_pre_set_dq.qV_Component1 = foc->cur_pre_set_dq.qI_Component1;
	foc->vol_pre_set_dq.qV_Component2 = foc->cur_pre_set_dq.qI_Component2;
#if 0
	
	if(foc->rpm_speed_set > 0){
#else														
	if(foc->cur_pre_set_dq.qI_Component2 > 0){
#endif
		detal = -64;
		//1-2-3-4-5-6
		start_sector = START_SECTOR;
	}else{
		//6-5-4-3-2-1
		detal = 64;
		start_sector = START_SECTOR;
	}
	
	while(get_pos_rotor()  != start_sector){
				
		volt_out = park_rev(foc->vol_pre_set_dq, cnt+=detal);
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
	
	foc->feedback.theta_offset = encoder_get_e_theta();	
	encoder_reset_aligment();
	foc->e_theta = encoder_get_e_theta();
}

void foc_get_feedback(fdbk_mod_t * const pfbk){
	static int16_t sample_times = 0;
	const int16_t iref = 2048;	
	pfbk->ia = get_inject_ia() - pfbk->cur_offset.qI_Component1;
	pfbk->ib = get_inject_ib() - pfbk->cur_offset.qI_Component2;
	pfbk->sector = get_pos_rotor_2();
	
	#if USE_STM32_ENCODER
	pfbk->theta = ENC_Get_Electrical_Angle();
	ENC_ResetEncoder();
	#else
	//pfbk->rpm = encoder_get_signal_cnt();
	//pfbk->theta = encoder_get_angular_pos() + 12800/2;
	//pfbk->theta = encoder_get_angular_pos();	
	//pfbk->theta = encoder_get_e_theta();
	//encoder_clear_timercounter();
	#endif

}

void foc_calc_fluxtorque_ref(){

}

void foc_motor_start2(foc_mod_t *foc,uint16_t timeout,void (* const time_cbk)(int16_t)){
				
	uint8_t circle = 0;
	int16_t sector;
	int16_t cnt = 0;
	uint16_t time_cnt = 0;
	Volt_Components volt_out;
	int16_t tmp_cnt;
	int16_t reg_cnt = 0;
	int16_t detal_cnt = 0;
	int16_t detal = 0;
	uint8_t start_sector = 0;
#if 0
	if(foc->rpm_speed_set > 0){
#else														
	if(foc->vol_pre_set_dq.qV_Component2 > 0){
#endif
		detal = -64;
		//1-2-3-4-5-6
		start_sector = 5;
	}else{
		//6-5-4-3-2-1
		detal = 64;
		start_sector = 5;
	}
	uint8_t get_start_rotor = 0;
	uint8_t get_zero_rotor = 0;
	while(get_zero_rotor != 1 || get_start_rotor != 1){
		
		if(get_start_rotor == 0 && (get_pos_rotor() ) == start_sector){
			get_start_rotor = 1;
			//tmp_cnt = encoder_get_timecounter_cnt();
			//tmp_cnt = tmp_cnt>=0 ? tmp_cnt:-tmp_cnt;
			tmp_cnt = encoder_get_e_theta();
		}
		if(get_zero_rotor == 0 && encoder_get_zero_pos_flag() == 1){
			get_zero_rotor = 1;
			//reg_cnt = encoder_get_timecounter_cnt();
			reg_cnt = encoder_get_e_theta();
		}
		volt_out = park_rev(foc->vol_pre_set_dq, cnt+=detal);
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
	detal_cnt = reg_cnt - tmp_cnt;
	detal_cnt = (detal_cnt >= 0 ?detal_cnt:-detal_cnt);
	//foc->feedback.theta_offset = encoder_get_first_zero_cnt();	
	//foc->feedback.theta_offset = encoder_conv_angular_pos(reg_cnt - tmp_cnt);
	foc->feedback.theta_offset = tmp_cnt - reg_cnt;
}


void foc_motor_start(foc_mod_t *foc,uint16_t timeout,void (* const time_cbk)(int16_t)){
				
	uint8_t circle = 0;
	int16_t sector;
	int16_t cnt = 0;
	uint16_t time_cnt = 0;
	Volt_Components volt_out;
	int16_t tmp_cnt;
	int16_t reg_cnt = 0;
	int16_t detal = 0;
	uint8_t start_sector = 0;
#if 0
	if(foc->rpm_speed_set > 0){
#else														
	if(foc->vol_pre_set_dq.qV_Component2 > 0){
#endif
		detal = -64;
		//1-2-3-4-5-6
		start_sector = 5;
	}else{
		//6-5-4-3-2-1
		detal = 64;
		start_sector = 5;
	}
	while((sector = get_pos_rotor()) != start_sector){

		volt_out = park_rev(foc->vol_pre_set_dq, cnt+=detal);
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
	#if USE_STM32_ENCODER
	
	#else
	tmp_cnt = encoder_get_timecounter_cnt();
	tmp_cnt = tmp_cnt>=0 ? tmp_cnt:-tmp_cnt;
	encoder_set_zero_pos_flag(0);
	#endif	
	foc->e_theta = 0;
	#if 1
	//while(encoder_get_zero_pos_flag() != start_sector){
	while(encoder_get_zero_pos_flag() != 1){
		volt_out = park_rev(foc->vol_pre_set_dq, foc->e_theta+=detal);
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
	reg_cnt = encoder_get_timecounter_cnt();
	reg_cnt = reg_cnt >=0 ? reg_cnt : - reg_cnt;
	foc->angle_cnt_detal = reg_cnt;
	//foc->feedback.theta_offset = encoder_get_first_zero_cnt();	
	foc->feedback.theta_offset = encoder_conv_angular_pos(foc->angle_cnt_detal);
	#endif
}

void foc_encoder_get_zero_line_offset(void *pargs){
	encoder_mod_t *p = pargs;
	if(pargs == NULL){
		return;
	}	
	//已经检测过复位信号偏移位置
	if(p->first_zero_signal_flag == 0){
		TIM3->CNT = p->zero_signal_offset;
		return;
	}
	
	//检测是否已经到初始化扇区
	if(p->first_start_sector_flag == 1
		&& stm_get_cur_state(&motor_state) == RUN ){
		p->zero_signal_offset = TIM3->CNT;
		p->first_zero_signal_flag = 0;
	}	
}
	