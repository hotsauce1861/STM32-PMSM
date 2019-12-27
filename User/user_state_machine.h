#ifndef USER_STATE_MACHINE_H
#define USER_STATE_MACHINE_H

//ERROR CODE 
#define ERROR_OVER_VOLT		0x0001


typedef enum {
	IDLE 		= 0x00,
	START_UP	= 0x01,
	RUN			= 0x02,
	STOP		= 0x03,		
	WAIT		= 0x04,
	FAULT		= 0x05,
	TEST		= 0x06
}state_e;

typedef state_e (*hcbk_get_cur_state)(void);
typedef void (*hcbk_set_next_state)(state_e);


struct state_machine_mod{	
	
	struct state_machine_mod* super;
	state_e state;
	
	hcbk_get_cur_state 	get_cur_state;
	hcbk_set_next_state set_next_state;
		
};

typedef struct state_machine_mod state_machine_mod_t;

extern state_machine_mod_t motor_state;

void state_machine_init(state_machine_mod_t* sm);

state_e stm_get_cur_state(state_machine_mod_t* pstm);
void stm_set_next_state(state_machine_mod_t* pstm, state_e e);


#endif
