#ifndef POSITION_H
#define POSITION_H
#include <stdint.h>

/**
	PHA--->PB0
	PHB--->PB1
	PHC--->PB2
 */

#define PHA			GPIO_Pin_0
#define PHB			GPIO_Pin_1
#define PHC			GPIO_Pin_2

//初始化电角三相信号的状态值
#define INIT_PHA_VALUE	1
#define INIT_PHB_VALUE	1
#define INIT_PHC_VALUE	0

typedef void (*PFUNC)(void*);	//到达初始位置时候回调函数

uint8_t get_pos_rotor(void);
uint8_t get_pos_rotor_2(void);
void pos_init(void);
void pos_set_cbk(PFUNC p,void *pargs);

#endif
