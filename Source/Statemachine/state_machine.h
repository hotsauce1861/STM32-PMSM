#ifndef STATE_MACHINE_h
#define STATE_MACHINE_h


#include <stdint.h>


enum state_mod{
	IDLE 		= 0x0001U,
	STOP 		= 0x0010U,
	ANY_STOP 	= 0x0020U,
	STOP_IDLE 	= 0x0030U,

};

typedef enum state_mod moto_state_t;



#endif
