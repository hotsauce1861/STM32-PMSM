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
#define USE_POSITION_PID		1
#define	USE_FEED_FORWARD		0

#define MAX_RPM 				75
#define MIN_RPM	 				5

#define PN						2

#define SPEED_PID_2MS			2
#define SPEED_PID_5MS			5
#define SPEED_PID_10MS			10

#define SPEED_PID_SAMPLE		SPEED_PID_2MS
/*
#if (SPEED_PID_SAMPLE == SPEED_PID_2MS)
#define ENCODER_SPEED_FACTOR	500
#elif (SPEED_PID_SAMPLE == SPEED_PID_5MS)
#define ENCODER_SPEED_FACTOR	200
#elif (SPEED_PID_SAMPLE == SPEED_PID_10MS)
#define ENCODER_SPEED_FACTOR	100
#else
#endif
*/
#define __maybe_unused __attribute__((unused)) 

void pid_config(int8_t index);

#endif
