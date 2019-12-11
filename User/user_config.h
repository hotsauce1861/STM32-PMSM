#ifndef USER_CONFIG_H
#define USER_CONFIG_H

/**
 include file gw_os
 */
#include "gw_type.h"
#include "gw_fifo.h"
#include "gw_tasks.h"
#include "gw_type.h"
#include "gw_tasks.h"
#include "gw_hal.h"
#include "gw_log.h"

#define USE_TASK_LIST 			1

#define TASK_ID_MOTOR			1
#define TASK_ID_KEY				2
#define TASK_ID_DISPLAY			3

#define USE_PID 				1
#define USE_CUR_PID				1
#define USE_SPEED_PID			0
#define PN						2


void pid_config(int8_t index);

#endif
