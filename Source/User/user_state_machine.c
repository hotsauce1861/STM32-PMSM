#include "user_state_machine.h"

state_machine_mod_t motor_state;

static state_e ustm_get_cur_state(void);
static void ustm_set_next_state(state_e e);

void state_machine_init(state_machine_mod_t* sm){
	sm->state = IDLE;
	sm->super = sm;
	sm->get_cur_state = ustm_get_cur_state;
	sm->set_next_state = ustm_set_next_state;
}

static state_e ustm_get_cur_state(void){
	return IDLE;
}

static void ustm_set_next_state(state_e e){

}

state_e stm_get_cur_state(state_machine_mod_t* pstm){
	return pstm->state;
}

void stm_set_next_state(state_machine_mod_t* pstm, state_e e){
	pstm->state = e;
}

