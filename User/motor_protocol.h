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
	0x55  2 bytes	1 byte   4 byte  0xAA
*/

#define PACKAGE_HEAD	0x55
#define PACKAGE_TAIL	0xAA
#define MOTOR_CMD_MASK	0xF000
#define MOTOR_CMD_SHIFT	12
enum motor_cmd {
	
	//basic_cmd
	STOP 			= 0x0001,
	RUN_RPM 		= 0x0002,
	RUN_POS 		= 0x0003,
	
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

struct motor_cmd_mod{
	uint8_t head;
	uint16_t cmd;
	uint8_t option;
	int32_t data;
	uint8_t tail;
};

typedef struct motor_cmd_mod motor_cmd_mod_t;

int8_t motor_cmd_is_valid(motor_cmd_mod_t *pcmd);
void motor_cmd_process(motor_cmd_mod_t *pcmd);

#ifdef __cplusplus
}
#endif 

#endif
