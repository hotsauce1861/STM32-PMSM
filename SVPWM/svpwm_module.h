#ifndef	_SVPWM_MODULE_H
#define	_SVPWM_MODULE_H
#include <stdint.h>
#include "svpwm_math.h"

struct svpwm_mod {

	//确定转子象限
//	int16_t Ua;		
//	int16_t Ub;
//	int16_t Uc;
	int16_t Udc;
	int32_t sector;
	//克拉克变换OAB坐标
	int16_t UAlpha; 	//输入，静止坐标系Alpha轴定子电压
    int16_t UBeta;  	//输入，静止坐标系Beta轴定子电压
	int16_t pwm_period;
	uint16_t Tcm1;	//A相打开的时间
	uint16_t Tcm2;	//B相打开的时间
	uint16_t Tcm3;	//C相打开的时间
	int16_t Angle;
	uint16_t Tpwm;
	int32_t Ua;
	int32_t Ub;
};
	
typedef struct svpwm_mod svpwm_mod_t;

void svpwm_init(void);
//uint16_t swpwm_setphase_vol( struct svpwm_mod *pHandle , Volt_Components Valfa_beta );
void svpwm_main_run1(struct svpwm_mod* const svpwm);
void svpwm_main_run2(struct svpwm_mod* const svpwm);
void svpwm_get_sector(struct svpwm_mod* const svpwm);
void svpwm_reset_pwm_duty(struct svpwm_mod* const svpwm);

#endif
