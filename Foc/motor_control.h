#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include <stdint.h>

struct motor_module{
	int16_t pair_num;
};
typedef struct motor_module motor_module_typedef;

void motor_run(void);
void motor_stop(void);

#endif