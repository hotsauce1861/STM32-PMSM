#include "svpwm_module.h"
#include "svpwm_math.h"
#include "svpwm_driver.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ABS(x)  ((x)>0?(x):-(x))
//#define SQRT_3 		1.732 * 100
#define SQRT_3 		173
#define MOTOR_POWER	20
//#define OVER_MODULATE	1
#define SQRT3_Q14	0x6ED9

#define TPWM		(int32_t)(5000)
#define SQRT3_TPWM	(int32_t)(8660)
#define ABS_X(x)	((x)>=0?(x):-(x))

void svpwm_init(void){
	pwm_init();	
}

//sqrt(3)*Vbeta作为标么值
uint8_t svpwm_nofloat_get_sector(struct svpwm_module *svpwm){

    uint8_t a,b,c;
	//int32_t U1,U2,U3;
	int32_t Ualpha = svpwm->UAlpha;
	int32_t Ubeta = svpwm->UBeta;

	/* 利用以下公式确定扇区 */
	//U1 = Ubeta;							
	//U2 = (SQRT_3*Ualpha - Ubeta * 100)/2;	
	//U3 = (-SQRT_3*Ualpha - Ubeta * 100)/2;

	if(Ubeta > (int32_t)0)
        a = 1;
    else
        a = 0;

	if((300*Ualpha) > (Ubeta * 100)){
		b = 1;
	}else{
		b = 0;
	}
	
	if((-300*Ualpha) > (Ubeta * 100))
        c = 1;
    else
        c = 0;

	return (4*c + 2*b + a);
}

void svpwm_time_check(uint16_t *time,uint16_t max_time,uint16_t min_time){
	if(*time>max_time){
		*time = max_time;
	}
	if(*time<min_time){
		*time = min_time;
	}
}


/**
 * 	@brief 过调制重置快关时间
 *	@args
 *		- Ts 	in 		pwm周期
 *		- pTx	out		更新后的Tx Q6格式
 *		- pTy	out		更新后的Ty Q6格式
 *		- Tx	in		原Tx时间
 *		- Ty	in		原Ty时间
 */
void svpwm_reset_time(	int32_t Ts,
							int32_t *pTx,int32_t *pTy,
							int32_t Tx,int32_t Ty){
	*pTx = Tx*Q6/(Tx+Ty);
	*pTy = Ty*Q6/(Tx+Ty) ;
}


/**
 * @brief produce svpwm wave
 *		X=sqrt(3)*Ubeta*Tpwm/Vdc
		Y=(3Ualpha+sqrt(3)*Ubeta)*Tpwm/2/Vdc
		Z=(-3Ualpha+sqrt(3)*Ubeta)*Tpwm/2/Vdc
	扇区		落在此扇区的充要条件

	I		Uα>0 ，Uβ>0 且Uβ/Uα<sqrt(3)
	Ⅱ 		Uα>0 ， 且Uβ/|Uα|>sqrt(3)
	Ⅲ 		Uα<0 ，Uβ>0 且-Uβ/Uα<sqrt(3)
	Ⅳ 		Uα<0 ，Uβ<0 且Uβ/Uα<sqrt(3)
	Ⅴ 		Uβ<0 且-Uβ/|Uα|>sqrt(3)
	Ⅵ 		Uα>0 ，Uβ<0 且-Uβ/Uα<sqrt(3)

 */
//1-3-2-6-4-5

void svpwm_get_sector(struct svpwm_module* const svpwm){
	int32_t wUalpha,wUbeta;
	uint8_t a,b,c;
	wUalpha = svpwm->UAlpha * SQRT3_Q14;
	wUbeta = svpwm->UBeta * Q14;
#if 1
	if(wUbeta>=0){
		if(wUalpha > wUbeta){
			svpwm->sector = 1;
		}else if(ABS_X(wUalpha) < wUbeta){
			svpwm->sector = 2;
		}else if(-wUalpha > wUbeta){
			svpwm->sector = 3;
		}
	}else{
		if(wUalpha < wUbeta){
			svpwm->sector = 4;
		}else if(ABS_X(wUalpha)< -wUbeta){
			svpwm->sector = 5;
		}else if(-wUalpha < wUbeta){
			svpwm->sector = 6;
		}
	}
	svpwm->Ua = wUalpha;
	svpwm->Ub = wUbeta;
	#else
	if(wUbeta>0)
		a = 1;
	else
		a = 0;
	
	if(wUalpha > wUbeta)
		b = 1;
	else
		b = 0;
	if(-wUalpha > wUbeta)
		c = 1;
	else
		c = 0;
	
	svpwm->sector = 4*c+2*b+a;	
	svpwm->Ua = wUalpha;
	svpwm->Ub = wUbeta;
	#endif
}

void svpwm_main_run1(struct svpwm_module* const svpwm){

	int32_t wX,wY,wZ,wTimePhA,wTimePhB,wTimePhC;
	int32_t wUalpha,wUbeta;
	uint8_t a,b,c;
	uint8_t sector;
	uint16_t Ts = svpwm->Tpwm;
	int32_t T4,T6,Tsum;
 	//wUalpha wUbeta is Q15 format
	wUalpha = (int32_t)svpwm->UAlpha * (int32_t)SQRT3_Q14/Q14;
	wUbeta = (int32_t)svpwm->UBeta;
	
	wX = wUbeta;
	wY = (wUbeta + wUalpha)/2; //DIV 2
	wZ = (wUbeta - wUalpha)/2; //DIV 2		
		
 	//1-3-2-6-4-5 forward
	//3-1-5-4-6-2 back
	if(wUbeta>0)
		a = 1;
	else
		a = 0;
	
	if(wUalpha > wUbeta)
		b = 1;
	else
		b = 0;
	if(-wUalpha > wUbeta)
		c = 1;
	else
		c = 0;
	
	sector = 4*c+2*b+a;			
	svpwm->sector = sector;

	//wX wY wZ in Q15
	/**
    	N	1	2	3	4	5	6
		T4	Z	Y	-Z	-X	X	-Y
		T6	Y	-X	X	Z	-Y	-Z
	 */
	switch(sector){
		case 1:
			T4 = wZ;
			T6 = wY;
			break;
		case 2:
			T4 = wY;
			T6 = -wX;
			break;
		case 3:
			T4 = -wZ;
			T6 = wX;
			break;
		case 4:
			T4 = -wX;
			T6 = wZ;
			break;
		case 5:
			T4 = wX;
			T6 = -wY;
			break;
		case 6:
			T4 = -wY;
			T6 = -wZ;
			break;
	}

	if((T4+T6)*Ts > Ts*Q15){
		//需要过调制处理		
		Tsum = T4 + T6;
		T4 = T4*Ts/Tsum;
		T6 = T6*Ts/Tsum;
		
		wTimePhA = Ts/4 - (T4 + T6)/4/Q15;
		wTimePhB = wTimePhA + T4/2/Q15;
		wTimePhC = wTimePhB + T6/2/Q15;		
		
	}else{
		wTimePhA = Ts/4 - (T4 + T6)*Ts/4/Q15;
		wTimePhB = wTimePhA + T4*Ts/2/Q15;
		wTimePhC = wTimePhB + T6*Ts/2/Q15;
	}

	wTimePhA = wTimePhA > 0 ? wTimePhA:0;
	wTimePhB = wTimePhB > 0 ? wTimePhB:0;
	wTimePhC = wTimePhC > 0 ? wTimePhC:0;

	/**
		N	1	2	3	4	5	6
		Tc1	Tb	Ta	Ta	Tc	Tc	Tb
		Tc2	Ta	Tc	Tb	Tb	Ta	Tc
		Tc3	Tc	Tb	Tc	Ta	Tb	Ta
	 */
	switch(sector){
		
		case 1:
			svpwm->Tcm1 = (uint16_t)wTimePhB;
			svpwm->Tcm2 = (uint16_t)wTimePhA;
			svpwm->Tcm3 = (uint16_t)wTimePhC;
			break;
		case 2:
			svpwm->Tcm1 = (uint16_t)wTimePhA;
			svpwm->Tcm2 = (uint16_t)wTimePhC;
			svpwm->Tcm3 = (uint16_t)wTimePhB;
			break;
		case 3:
			svpwm->Tcm1 = (uint16_t)wTimePhA;
			svpwm->Tcm2 = (uint16_t)wTimePhB;
			svpwm->Tcm3 = (uint16_t)wTimePhC;
			break;
		case 4:
			svpwm->Tcm1 = (uint16_t)wTimePhC;
			svpwm->Tcm2 = (uint16_t)wTimePhB;
			svpwm->Tcm3 = (uint16_t)wTimePhA;
			break;
		case 5:
			svpwm->Tcm1 = (uint16_t)wTimePhC;
			svpwm->Tcm2 = (uint16_t)wTimePhA;
			svpwm->Tcm3 = (uint16_t)wTimePhB;
			break;
		case 6:
			svpwm->Tcm1 = (uint16_t)wTimePhB;
			svpwm->Tcm2 = (uint16_t)wTimePhC;
			svpwm->Tcm3 = (uint16_t)wTimePhA;
			break;
	}
}

/**
 * @brief produce svpwm wave
 * 		X=sqrt(3)*Ubeta*Tpwm/Vdc
 		Y=(3Ualpha+sqrt(3)*Ubeta)*Tpwm/2/Vdc
 		Z=(-3Ualpha+sqrt(3)*Ubeta)*Tpwm/2/Vdc
	扇区		落在此扇区的充要条件

	1		Uα>0 ，Uβ>0 且Uβ/Uα<sqrt(3)
	2 		Uα>0 ， 且Uβ/|Uα|>sqrt(3)
	3		Uα<0 ，Uβ>0 且-Uβ/Uα<sqrt(3)
	4 		Uα<0 ，Uβ<0 且Uβ/Uα<sqrt(3)
	5 		Uβ<0 且-Uβ/|Uα|>sqrt(3)
	6 		Uα>0 ，Uβ<0 且-Uβ/Uα<sqrt(3)

 */
#define OVER_MODULATE	1
void svpwm_main_run2(struct svpwm_module* const svpwm){
	int32_t wX,wY,wZ,wTimePhA,wTimePhB,wTimePhC;
	int32_t wUalpha,wUbeta;
	uint8_t a,b,c;
	int32_t sector;
	uint16_t Ts = svpwm->Tpwm;
	int32_t T4,T6;
	wUalpha = (int32_t)svpwm->UAlpha * (int32_t)SQRT3_Q14 / Q14;
	wUbeta = (int32_t)svpwm->UBeta;	
	wX = wUbeta/svpwm->Udc;
	wY = (wUalpha + wUbeta)/2/svpwm->Udc;
	wZ = (-wUalpha + wUbeta)/2/svpwm->Udc;
	
#if 0

#else
	if(wUbeta>=0)
		a = 1;
	else
		a = 0;
	if(wUalpha>=wUbeta)
		b = 1;
	else
		b = 0;
	if(-wUalpha>=wUbeta)
		c = 1;
	else
		c = 0;
	//svpwm->sector = 4*c + 2*b + a;

	/*

	 *	N	1	2	3	4	5	6
		T4	Z	Y	-Z	-X	X	-Y
		T6	Y	-X	X	Z	-Y	-Z
		---------------------------
		svpwm->Tpwm
		Q15	wX,wY,wZ
	 */
	switch(4*c+2*b+a){
		case 2:
			svpwm->sector = 1;
//			wTimePhA = (int32_t)svpwm->Tpwm/4+(wX - wY)/Q15/4;
//			wTimePhB = wTimePhA + wY/Q16;
//			wTimePhC = wTimePhB - wX/Q16;
			//Q6		
			wTimePhA = (int32_t)svpwm->Tpwm*Q4 + (wX - wY)*(int32_t)Ts/Q11;
			if(wTimePhA > 0){
				wTimePhB = wTimePhA + wY*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB - wX*(int32_t)Ts/Q10;
			}else{
				//过调制 T4 T6 Q6
				#if OVER_MODULATE
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,wY,-wX);
				wTimePhA = (int32_t)svpwm->Tpwm*Q6/4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;	
				#endif
			}
			svpwm->Tcm1 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			
			break;
		case 6:
			svpwm->sector = 2;
			wTimePhA = (int32_t)svpwm->Tpwm*Q4 + (wY + wZ)*(int32_t)Ts/Q11;
			if(wTimePhA > 0){
				wTimePhB = wTimePhA - wY*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB - wZ*(int32_t)Ts/Q10;
			}else{
				#if OVER_MODULATE
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,-wY,-wZ);
				wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;
				#endif
			}
			svpwm->Tcm1 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);

			break;
		case 1:
			svpwm->sector = 3;
			wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (wZ+wY)*(int32_t)Ts/Q11;
			if(wTimePhA > 0){
				wTimePhB = wTimePhA + wZ*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB + wY*(int32_t)Ts/Q10;
			}else{
				#if OVER_MODULATE
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,wZ,wY);
				wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;
				#endif
			}
			svpwm->Tcm1 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);
			
			break;
		case 4:
			svpwm->sector = 4;
			wTimePhA = (int32_t)svpwm->Tpwm*Q4 + (wX - wZ)*(int32_t)Ts/Q11;
			if(wTimePhA>0){
				wTimePhB = wTimePhA - wX*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB + wZ*(int32_t)Ts/Q10;
			}else{
				#if OVER_MODULATE
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,-wX,wZ);
				wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;
				#endif
			}
			svpwm->Tcm1 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);
			
			break;
		case 3:
			svpwm->sector = 5;
			wTimePhA = (int32_t)svpwm->Tpwm*Q4+(wZ - wX)*(int32_t)Ts/Q11;
			if(wTimePhA>0){
				wTimePhB = wTimePhA - wZ*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB + wX*(int32_t)Ts/Q10;
			}else{
				#if OVER_MODULATE				
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,-wZ,wX);
				wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;
				#endif				
			}		
			svpwm->Tcm1 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);

			break;
		case 5:
			svpwm->sector = 6;
			wTimePhA = (int32_t)svpwm->Tpwm*Q4 + (wY - wX)*(int32_t)Ts/Q11;
			if(wTimePhA>0){
				wTimePhB = wTimePhA + wX*(int32_t)Ts/Q10;
				wTimePhC = wTimePhB - wY*(int32_t)Ts/Q10;
			}else{
				#if OVER_MODULATE				
				svpwm_reset_time((int32_t)svpwm->Tpwm,
								&T4,&T6,wX,-wY);
				wTimePhA = (int32_t)svpwm->Tpwm*Q4 - (T4+T6)*(int32_t)Ts/4;
				wTimePhB = wTimePhA + T4*(int32_t)Ts/2;
				wTimePhC = wTimePhB + T6*(int32_t)Ts/2;
				#endif
			}
			svpwm->Tcm1 = (uint16_t)(wTimePhC/Q6 > 0 ? wTimePhC/Q6 : 0);
			svpwm->Tcm2 = (uint16_t)(wTimePhA/Q6 > 0 ? wTimePhA/Q6 : 0);
			svpwm->Tcm3 = (uint16_t)(wTimePhB/Q6 > 0 ? wTimePhB/Q6 : 0);
			
			break;
		default:
			svpwm->sector = 0;
			break;
	}
	//svpwm->Tcm1 /= svpwm->Udc;
	//svpwm->Tcm2 /= svpwm->Udc;
	//svpwm->Tcm3 /= svpwm->Udc;
	svpwm_time_check(&svpwm->Tcm1, Ts*2/3,1);
	svpwm_time_check(&svpwm->Tcm2, Ts*2/3,1);
	svpwm_time_check(&svpwm->Tcm3, Ts*2/3,1);
	
#endif
}

void svpwm_reset_pwm_duty(struct svpwm_module* const svpwm){
	pwm_reset_duty_cnt(1, svpwm->Tcm1);
	pwm_reset_duty_cnt(2, svpwm->Tcm2);
	pwm_reset_duty_cnt(3, svpwm->Tcm3);
}

