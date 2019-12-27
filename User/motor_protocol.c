#include "motor_protocol.h"
#include "user_config.h"
#include "user_state_machine.h"
#include "task_motor_control.h"

#include "usart_driver.h"
#include "foc.h"

extern foc_mod_t foc_obj;

motor_cmd_mod_t user_cmd_mod = {
	.head = 0,
	.cmd = 0,
	.option = 0,
	.data = 0,
	.tail = 0,
	.process_flag = 0
};

static __maybe_unused void process_basic_cmd(motor_cmd_mod_t *pcmd);
static __maybe_unused void process_cur_pid_cmd(motor_cmd_mod_t *pcmd);
static __maybe_unused void process_spd_pid_cmd(motor_cmd_mod_t *pcmd);
static __maybe_unused void process_pos_pid_cmd(motor_cmd_mod_t *pcmd);
static __maybe_unused void process_ffc_cmd(motor_cmd_mod_t *pcmd);

int8_t motor_cmd_is_valid(motor_cmd_mod_t *pcmd){
	if(pcmd->head == PACKAGE_HEAD && pcmd->tail == PACKAGE_TAIL){
		return 1;
	}
	return -1;
}

void motor_get_cmd_from_uart(void *pargs){
	
	if(pargs == NULL){
		return;
	}	
	uart_mod_t *p = pargs;
	if(p->rx_dat_len > 0 && p->rx_dat_len == PACKAGE_SIZE){
		if(p->rx_buf[0] == PACKAGE_HEAD 
		&& p->rx_buf[PACKAGE_SIZE - 1] == PACKAGE_TAIL){
			user_cmd_mod.head = p->rx_buf[0];
			user_cmd_mod.cmd.value_n[0] = p->rx_buf[1];
			user_cmd_mod.cmd.value_n[1] = p->rx_buf[2];
			
			user_cmd_mod.option = p->rx_buf[3];
			
			user_cmd_mod.data.value_n[0] = p->rx_buf[4];
			user_cmd_mod.data.value_n[1] = p->rx_buf[5];
			user_cmd_mod.data.value_n[2] = p->rx_buf[6];
			user_cmd_mod.data.value_n[3] = p->rx_buf[7];
			
			user_cmd_mod.tail = p->rx_buf[PACKAGE_SIZE - 1];
			user_cmd_mod.process_flag = 1;
		}		
	}
	p->rx_dat_len = 0;
	
}

void motor_cmd_process(motor_cmd_mod_t *pcmd){
	uint16_t cmd_type = 0x0000;
	pcmd->process_flag = 0;
	cmd_type = (pcmd->cmd.value & MOTOR_CMD_MASK)>>MOTOR_CMD_SHIFT;
	switch(cmd_type){
		case 0:
			process_basic_cmd(pcmd);
			break;
		case 1:
			process_cur_pid_cmd(pcmd);
			break;
		case 2:
			process_spd_pid_cmd(pcmd);
			break;
		case 3:
			process_pos_pid_cmd(pcmd);
			break;
		case 4:
			process_ffc_cmd(pcmd);
			break;
	}
}

static void process_basic_cmd(motor_cmd_mod_t *pcmd){
	switch(pcmd->cmd.value){
		case M_SYS_RESET:
			soft_reset();
			break;
		case M_STOP:
			if(stm_get_cur_state(&motor_state) == RUN){				
				stm_set_next_state(&motor_state, STOP);
			}			
			break;
		case M_RUN_RPM:
			if(stm_get_cur_state(&motor_state) == STOP){				
				stm_set_next_state(&motor_state, RUN);
				task_motor_pwm_enable();
			}						
			if(pcmd->option == DIR_FORWARD){				
				foc_set_rpm_ref(&foc_obj, pcmd->data.value);
			}else{
				foc_set_rpm_ref(&foc_obj, -pcmd->data.value);
			}
			break;
		case M_RUN_POS:
			if(stm_get_cur_state(&motor_state) == STOP){				
				stm_set_next_state(&motor_state, RUN);
				task_motor_pwm_enable();
			}				
			foc_set_position_ref(&foc_obj,pcmd->data.value);
			
			break;
		case M_STARTUP:
			if(stm_get_cur_state(&motor_state) == IDLE){
				stm_set_next_state(&motor_state, START_UP);
			}
			break;
	}
}

static void process_cur_pid_cmd(motor_cmd_mod_t *pcmd){
	
	switch(pcmd->cmd.value){
		case PID_CUR_KI:
			break;
		case PID_CUR_KIDIV:
			break;
		case PID_CUR_KP:
			break;
		case PID_CUR_KPDIV:
			break;
	}
}

static void process_spd_pid_cmd(motor_cmd_mod_t *pcmd){
	
	switch(pcmd->cmd.value){
		case PID_SPD_KI:
			break;
		case PID_SPD_KIDIV:
			break;
		case PID_SPD_KP:
			break;
		case PID_SPD_KPDIV:
			break;
	}
}

static void process_pos_pid_cmd(motor_cmd_mod_t *pcmd){
	
	switch(pcmd->cmd.value){
		case PID_POS_KI:
			break;
		case PID_POS_KIDIV:
			break;
		case PID_POS_KP:
			break;
		case PID_POS_KPDIV:
			break;
	}
}

static void process_ffc_cmd(motor_cmd_mod_t *pcmd){
	switch(pcmd->cmd.value){
		case FFC_A_FACTOR:
			break;
		case FFC_B_FACTOR:
			break;
		
	}
}