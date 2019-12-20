#include "motor_protocol.h"
#include "user_config.h"


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

void motor_cmd_process(motor_cmd_mod_t *pcmd){
	uint16_t cmd_type = 0x0000;
	cmd_type = (pcmd->cmd & MOTOR_CMD_MASK)>>MOTOR_CMD_SHIFT;
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
	switch(pcmd->cmd){
		case STOP:
			
			break;
		case RUN_RPM:
			break;
		case RUN_POS:
			break;
	}
}

static void process_cur_pid_cmd(motor_cmd_mod_t *pcmd){
	
	switch(pcmd->cmd){
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
	
	switch(pcmd->cmd){
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
	
	switch(pcmd->cmd){
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
	switch(pcmd->cmd){
		case FFC_A_FACTOR:
			break;
		case FFC_B_FACTOR:
			break;
		
	}
}