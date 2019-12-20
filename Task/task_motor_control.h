#ifndef TASK_MOTOR_CONTORL_H
#define TASK_MOTOR_CONTORL_H

#include "current.h"
#include "svpwm_math.h"

void task_motor_stop(void);
void task_motor_control_init(void);
void task_motor_control(void* args);

extern void task_motor_cur_loop(Curr_Components cur_ab);
extern void task_motor_startup(Curr_Components cur_ab,uint16_t timeout);
extern void task_motor_startup_02(Curr_Components cur_ab,uint16_t timeout);
#endif
