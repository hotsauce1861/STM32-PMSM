#include "svpwm_module.h"
#include "svpwm_math.h"
#include "svpwm_driver.h"
#include <stdint.h>
#include <stdio.h>

//#define SQRT_3 		1.732 * 100
#define SQRT_3 		173
#define MOTOR_POWER	20


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

void svpwm_time_check(int32_t *time,int32_t max_time,int32_t min_time){
	if(*time>max_time){
		*time = max_time;
	}
	if(*time<min_time){
		*time = min_time;
	}
}
//sqrt(3)*Vbeta作为标么值
void svpwm_nofloat_run(struct svpwm_module *svpwm)
{
    uint8_t sector=0;
    int32_t Udc = MOTOR_POWER;
	int32_t T4,T6,Terr,X,Y,Z,Ta,Tb,Tc;
	
//	uint16_t Ta,Tb,Tc;
	//Ts的单位是秒所以要做归一化处理
	//Ts = get_pwm_period ();
	//float Ts = 0.0001; //0.0001 * 1000*1000
	int16_t Ts = get_pwm_period();

	sector = svpwm_nofloat_get_sector(svpwm);

	//注意数据范围，是否会溢出
#if 0
	//产生了马鞍波形，但是毛刺较多，存在问题
	X = SQRT_3 * (int32_t)svpwm->UBeta/Udc;
	Y = SQRT_3 * (SQRT_3*(int32_t)svpwm->UAlpha + (int32_t)svpwm->UBeta * 100)/ 200/Udc;
	Z = SQRT_3 * (-SQRT_3*(int32_t)svpwm->UAlpha + (int32_t)svpwm->UBeta * 100)/ 200/Udc;
#else
	//产生了马鞍波形，但是毛刺较多，存在问题
	X = Ts*svpwm->UBeta/Q14/Udc;
	Y = Ts*(3*svpwm->UAlpha + svpwm->UBeta )/Q14/2/Udc;	//这里的UBeta查表处理，已经乘以sqrt3
	Z = Ts*(-3*svpwm->UAlpha + svpwm->UBeta)/Q14/2/Udc;
#endif
    /* 计算SVPWM占空比 */
	/* 确定扇区号计算开关时间
	N		3	1	5	4	6	2
	扇区	1	2	3	4	5	6
	*/
	svpwm->sector = sector;
	switch(sector){
		case 1:
			T4 = Z;
			T6 = Y;
		break;
		case 2:
			T4 = Y;
			T6 = -X;
		break;
		case 3:
			T4 = -Z;
			T6 = X;
		break;
		case 4:
			T4 = -X;
			T6 = Z;
		break;
		case 5:
			T4 = X;
			T6 = -Y;
		break;
		case 6:
			T4 = -Y;
			T6 = -Z;
		break;
		default:
			break;
	}

	//判断是否过调制，会导致失真
	// Ts = 0.0001
	// 0.0001 * 10000 * 100 = 100

	if ((T4+T6)>Ts){
		Terr = T4+T6;
		T4 = (float)((float)T4/(float)Terr)*Ts;
		T6 = (float)((float)T6/(float)Terr)*Ts;		
	}
	
	Ta = (Ts - T4 - T6)/4;
	Tb = Ta + (T4)/2;
	Tc = Tb + (T6)/2;
	
	svpwm_time_check(&Ta,Ts/2, 0);
	svpwm_time_check(&Tb,Ts/2, 0);
	svpwm_time_check(&Tc,Ts/2, 0);
	
	switch (sector){
		case 1:
			svpwm->Tcm1 = Tb;
			svpwm->Tcm2 = Ta;
			svpwm->Tcm3 = Tc;
		break;
		case 2:
			svpwm->Tcm1 = Ta;
			svpwm->Tcm2 = Tc;
			svpwm->Tcm3 = Tb;
			break;
		case 3:
			svpwm->Tcm1 = Ta;
			svpwm->Tcm2 = Tb;
			svpwm->Tcm3 = Tc;
			break;
		case 4:
			svpwm->Tcm1 = Tc;
			svpwm->Tcm2 = Tb;
			svpwm->Tcm3 = Ta;
			break;
		case 5:
			svpwm->Tcm1 = Tc;
			svpwm->Tcm2 = Ta;
			svpwm->Tcm3 = Tb;
			break;
		case 6: 	
			svpwm->Tcm1 = Tb;
			svpwm->Tcm2 = Tc;
			svpwm->Tcm3 = Ta;
			break;
		//	零向量的处理
		default:
			//svpwm->Tcm1 = Ts/2;
			//svpwm->Tcm2 = Ts/2;
			//svpwm->Tcm3 = Ts/2;		   
			break;
	}	
}

#if 0
uint16_t PWMC_SetPhaseVoltage( PWMC_Handle_t * pHandle, Volt_Components Valfa_beta )
{
  int32_t wX, wY, wZ, wUAlpha, wUBeta, wTimePhA, wTimePhB, wTimePhC;
  PWMC_SetSampPointSectX_Cb_t pSetADCSamplingPoint;

  wUAlpha = Valfa_beta.qV_Component1 * ( int32_t )pHandle->hT_Sqrt3;
  wUBeta = -( Valfa_beta.qV_Component2 * ( int32_t )( pHandle->hPWMperiod ) ) * 2;

  wX = wUBeta;
  wY = ( wUBeta + wUAlpha ) / 2;
  wZ = ( wUBeta - wUAlpha ) / 2;

  /* Sector calculation from wX, wY, wZ */
  if ( wY < 0 )
  {
    if ( wZ < 0 )
    {
      pHandle->hSector = SECTOR_5;
      wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wY - wZ ) / ( int32_t )262144 );
      wTimePhB = wTimePhA + wZ / 131072;
      wTimePhC = wTimePhA - wY / 131072;
      pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect5;
    }
    else /* wZ >= 0 */
      if ( wX <= 0 )
      {
        pHandle->hSector = SECTOR_4;
        wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wX - wZ ) / ( int32_t )262144 );
        wTimePhB = wTimePhA + wZ / 131072;
        wTimePhC = wTimePhB - wX / 131072;
        pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect4;
      }
      else /* wX > 0 */
      {
        pHandle->hSector = SECTOR_3;
        wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wY - wX ) / ( int32_t )262144 );
        wTimePhC = wTimePhA - wY / 131072;
        wTimePhB = wTimePhC + wX / 131072;
        pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect3;
      }
  }
  else /* wY > 0 */
  {
    if ( wZ >= 0 )
    {
      pHandle->hSector = SECTOR_2;
      wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wY - wZ ) / ( int32_t )262144 );
      wTimePhB = wTimePhA + wZ / 131072;
      wTimePhC = wTimePhA - wY / 131072;
      pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect2;
    }
    else /* wZ < 0 */
      if ( wX <= 0 )
      {
        pHandle->hSector = SECTOR_6;
        wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wY - wX ) / ( int32_t )262144 );
        wTimePhC = wTimePhA - wY / 131072;
        wTimePhB = wTimePhC + wX / 131072;
        pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect6;
      }
      else /* wX > 0 */
      {
        pHandle->hSector = SECTOR_1;
        wTimePhA = ( int32_t )( pHandle->hPWMperiod ) / 4 + ( ( wX - wZ ) / ( int32_t )262144 );
        wTimePhB = wTimePhA + wZ / 131072;
        wTimePhC = wTimePhB - wX / 131072;
        pSetADCSamplingPoint = pHandle->pFctSetADCSampPointSect1;
      }
  }

  pHandle->hCntPhA = ( uint16_t )wTimePhA;
  pHandle->hCntPhB = ( uint16_t )wTimePhB;
  pHandle->hCntPhC = ( uint16_t )wTimePhC;

  if ( pHandle->DTTest == 1u )
  {
    /* Dead time compensation */
    if ( pHandle->hIa > 0 )
    {
      pHandle->hCntPhA += pHandle->DTCompCnt;
    }
    else
    {
      pHandle->hCntPhA -= pHandle->DTCompCnt;
    }

    if ( pHandle->hIb > 0 )
    {
      pHandle->hCntPhB += pHandle->DTCompCnt;
    }
    else
    {
      pHandle->hCntPhB -= pHandle->DTCompCnt;
    }

    if ( pHandle->hIc > 0 )
    {
      pHandle->hCntPhC += pHandle->DTCompCnt;
    }
    else
    {
      pHandle->hCntPhC -= pHandle->DTCompCnt;
    }
  }

  return ( pSetADCSamplingPoint( pHandle ) );
}


#endif

