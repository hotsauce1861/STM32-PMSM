#include <stdint.h>
#include "user_config.h"
#include "pid_regulator.h"
#include "encoder.h"
#include "current.h"
#include "position.h"
#include "foc.h"
#include "SDS.h"

extern foc_mod_t foc_obj;

void pid_config(int8_t index){
	
#if USE_PID
	switch(index){
		case 1:			
			//cur 	PI
			//spd	PI
			//pos	P
			PID_HandleInit(&foc_obj.speed_pi);
			foc_obj.speed_pi.hKpGain = 120;
			foc_obj.speed_pi.hKiGain = 1;
			foc_obj.speed_pi.hKdGain = 0;	
			foc_obj.speed_pi.wLowerIntegralLimit = -32768*10;
			foc_obj.speed_pi.wUpperIntegralLimit = 32767*10;
			foc_obj.speed_pi.hLowerOutputLimit = -32768;
			foc_obj.speed_pi.hUpperOutputLimit = 32767;				
			foc_obj.speed_pi.hKpDivisor = 1;
			foc_obj.speed_pi.hKiDivisor = 1;
			foc_obj.speed_pi.hKdDivisor = 1;
			
			PID_HandleInit(&foc_obj.cur_d_pi);
			foc_obj.cur_d_pi.hKpGain = PID_FLUX_KP_DEFAULT;
			foc_obj.cur_d_pi.hKiGain = PID_FLUX_KI_DEFAULT;
			foc_obj.cur_d_pi.hKpDivisor = TF_KPDIV;
			foc_obj.cur_d_pi.hKiDivisor = TF_KIDIV;
			foc_obj.cur_d_pi.wLowerIntegralLimit = -32768*100;
			foc_obj.cur_d_pi.wUpperIntegralLimit = 32767*100;
			
			foc_obj.cur_d_pi.hKdGain = 0;	
			foc_obj.cur_d_pi.hLowerOutputLimit = -32768;
			foc_obj.cur_d_pi.hUpperOutputLimit = 32767; 
			foc_obj.cur_d_pi.hKpDivisorPOW2 = 1;
			foc_obj.cur_d_pi.hKiDivisorPOW2 = 7;
			
			PID_HandleInit(&foc_obj.cur_q_pi);			
			foc_obj.cur_q_pi.hKpGain = PID_TORQUE_KP_DEFAULT;
			foc_obj.cur_q_pi.hKiGain = PID_TORQUE_KI_DEFAULT;
			foc_obj.cur_q_pi.hKpDivisor = TF_KPDIV;
			foc_obj.cur_q_pi.hKiDivisor = TF_KIDIV;	
			foc_obj.cur_q_pi.hKpDivisorPOW2 = 1;
			foc_obj.cur_q_pi.hKiDivisorPOW2 = 7;
			foc_obj.cur_q_pi.hKdGain = 0;	
			foc_obj.cur_q_pi.hLowerOutputLimit = -32768;
			foc_obj.cur_q_pi.hUpperOutputLimit = 32767; 
			foc_obj.cur_q_pi.wLowerIntegralLimit = -32768*100;
			foc_obj.cur_q_pi.wUpperIntegralLimit = 32767*100;	
			
			PID_HandleInit(&foc_obj.position_pi);			
			foc_obj.position_pi.hKpGain = 3;
			foc_obj.position_pi.hKiGain = 0;
			foc_obj.position_pi.hKpDivisor = 0;
			foc_obj.position_pi.hKiDivisor = 0;	
			foc_obj.position_pi.hKpDivisorPOW2 = 0;
			foc_obj.position_pi.hKiDivisorPOW2 = 6;
			foc_obj.position_pi.hKdGain = 0;	
			foc_obj.position_pi.hLowerOutputLimit = -MAX_RPM;
			foc_obj.position_pi.hUpperOutputLimit = MAX_RPM;
			foc_obj.position_pi.wLowerIntegralLimit = -32768*100;
			foc_obj.position_pi.wUpperIntegralLimit = 32767*100;			
			break;
		case 2:
			//cur 	P
			//spd	PI
			//pos	P
			PID_HandleInit(&foc_obj.speed_pi);
			foc_obj.speed_pi.hKpGain = 50;
			foc_obj.speed_pi.hKiGain = 2;
			foc_obj.speed_pi.hKdGain = 0;	
			foc_obj.speed_pi.wLowerIntegralLimit = -32768*10;
			foc_obj.speed_pi.wUpperIntegralLimit = 32767*10;
			foc_obj.speed_pi.hLowerOutputLimit = -32768;
			foc_obj.speed_pi.hUpperOutputLimit = 32767;	
			foc_obj.speed_pi.hKpDivisor = 1;
			foc_obj.speed_pi.hKiDivisor = 1;
			foc_obj.speed_pi.hKdDivisor = 1;
			
			PID_HandleInit(&foc_obj.cur_d_pi);
			foc_obj.cur_d_pi.hKpGain = PID_FLUX_KP_DEFAULT;
			foc_obj.cur_d_pi.hKiGain = 0;
			foc_obj.cur_d_pi.hKpDivisor = TF_KPDIV;
			foc_obj.cur_d_pi.hKiDivisor = TF_KIDIV;
			
			foc_obj.cur_d_pi.hKdGain = 0;	
			foc_obj.cur_d_pi.hLowerOutputLimit = -32768;
			foc_obj.cur_d_pi.hUpperOutputLimit = 32767; 
			foc_obj.cur_d_pi.hKpDivisorPOW2 = 0;
			foc_obj.cur_d_pi.hKiDivisorPOW2 = 7;
			
			PID_HandleInit(&foc_obj.cur_q_pi);			
			foc_obj.cur_q_pi.hKpGain = PID_TORQUE_KP_DEFAULT;
			foc_obj.cur_q_pi.hKiGain = 0;
			foc_obj.cur_q_pi.hKpDivisor = TF_KPDIV;
			foc_obj.cur_q_pi.hKiDivisor = TF_KIDIV;	
			foc_obj.cur_q_pi.hKpDivisorPOW2 = 0;
			foc_obj.cur_q_pi.hKiDivisorPOW2 = 7;
			foc_obj.cur_q_pi.hKdGain = 0;	
			foc_obj.cur_q_pi.hLowerOutputLimit = -32768;
			foc_obj.cur_q_pi.hUpperOutputLimit = 32767; 		

			PID_HandleInit(&foc_obj.position_pi);			
			foc_obj.position_pi.hKpGain = 3;
			foc_obj.position_pi.hKiGain = 0;
			foc_obj.position_pi.hKpDivisor = 0;
			foc_obj.position_pi.hKiDivisor = 0;	
			foc_obj.position_pi.hKpDivisorPOW2 = 0;
			foc_obj.position_pi.hKiDivisorPOW2 = 6;
			foc_obj.position_pi.hKdGain = 0;	
			foc_obj.position_pi.hLowerOutputLimit = -75;
			foc_obj.position_pi.hUpperOutputLimit = 75;
			
			foc_obj.position_pi.wLowerIntegralLimit = -32768*100;
			foc_obj.position_pi.wUpperIntegralLimit = 32767*100;

			break;
	}
#endif	
}