#ifndef	_SVPWM_MODULE_H
#define	_SVPWM_MODULE_H
#include <stdint.h>


struct svpwm_time_on{
	float Tcm1;	//A相打开的时间
	float Tcm2;	//B相打开的时间
	float Tcm3;	//C相打开的时间
};

struct svpwm_module {

	//确定转子象限
	int16_t Ua;		
	int16_t Ub;
	int16_t Uc;

	uint8_t sector;
	//克拉克变换OAB坐标
    int32_t UAlpha; 	//输入，静止坐标系Alpha轴定子电压
    int32_t UBeta;  	//输入，静止坐标系Beta轴定子电压

	int32_t Tcm1;	//A相打开的时间
	int32_t Tcm2;	//B相打开的时间
	int32_t Tcm3;	//C相打开的时间
	int32_t Tcm4;	//过调制信号

};


void svpwm_init(void);
uint8_t svpwm_nofloat_get_sector(struct svpwm_module *svpwm);
void svpwm_nofloat_run(struct svpwm_module *svpwm);


#endif
