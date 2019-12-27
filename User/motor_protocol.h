#ifndef MOTOR_PROTOCOL_H
#define MOTOR_PROTOCOL_H
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif 
/**
	command package format
	------------------------------------
	Head command 	option   data 	 Tail
	%  2 bytes	1 byte   4 byte  	 &
*/

#define PACKAGE_HEAD	'%'
#define PACKAGE_TAIL	'&'
#define PACKAGE_SIZE	9

#define MOTOR_CMD_MASK	0xF000
#define MOTOR_CMD_SHIFT	12
enum motor_cmd {	
	//basic_cmd
	M_SYS_RESET		= 0x000F,
	M_STOP 			= 0x0001,
	M_RUN_RPM 		= 0x0002,
	M_RUN_POS 		= 0x0003,
	M_STARTUP		= 0x0004,
	
	//pid of current loop
	PID_CUR_KP 		= 0x1000,
	PID_CUR_KPDIV 	= 0x1001,
	PID_CUR_KI 		= 0x1002,
	PID_CUR_KIDIV 	= 0x1003,
	
	//pid of speed loop
	PID_SPD_KP 		= 0x2000,
	PID_SPD_KPDIV	= 0x2001,
	PID_SPD_KI 		= 0x2002,
	PID_SPD_KIDIV	= 0x2003,
	
	//pid of position loop
	PID_POS_KP 		= 0x3000,
	PID_POS_KPDIV 	= 0x3001,
	PID_POS_KI 		= 0x3002,
	PID_POS_KIDIV	= 0x3003,
	PID_POS_UP_LIM	= 0x3003,
	PID_POS_LOW_LIM	= 0x3004,
	
	FFC_A_FACTOR	= 0x4000,
	FFC_B_FACTOR	= 0x4001
};

enum motor_run_option{
	DIR_FORWARD = 0x01,
	DIR_BACK 	= 0x02,
};

union motor_cmd_data {
	int32_t value;
	uint8_t value_n[4];
};

union motor_cmd_cmd {
	int16_t value;
	uint8_t value_n[2];
};


struct motor_cmd_mod{
	uint8_t head;
	union motor_cmd_cmd cmd;
	uint8_t option;
	union motor_cmd_data data;
	uint8_t tail;
	int8_t process_flag;
};

typedef struct motor_cmd_mod motor_cmd_mod_t;

extern motor_cmd_mod_t user_cmd_mod;
int8_t motor_cmd_is_valid(motor_cmd_mod_t *pcmd);
void motor_cmd_process(motor_cmd_mod_t *pcmd);

void motor_get_cmd_from_uart(void *pargs);

#ifdef __cplusplus
}
#endif 

#endif
