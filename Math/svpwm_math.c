#include "stdint.h"
#include "svpwm_math.h"


#define divSQRT_3 (int32_t)0x49E6    /* 1/sqrt(3) in q1.15 format=0.5773315*/
#define SIN_COS_TABLE {\
    0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,\
    0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,\
    0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,\
    0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,\
    0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,\
    0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,\
    0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,\
    0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,\
    0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,\
    0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,\
    0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,\
    0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,\
    0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,\
    0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,\
    0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,\
    0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,\
    0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,\
    0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,\
    0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,\
    0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,\
    0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,\
    0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,\
    0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,\
    0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,\
    0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,\
    0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,\
    0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,\
    0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,\
    0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,\
    0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,\
    0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,\
    0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE}

#define SIN_MASK        0x0300u
#define U0_90           0x0200u
#define U90_180         0x0300u                  
#define U180_270        0x0000u
#define U270_360        0x0100u

/* Private variables ---------------------------------------------------------*/
const int16_t hSin_Cos_Table[256] = SIN_COS_TABLE;

/**
  * @brief  This function transforms stator currents Ia and qIb (which are
  *         directed along axes each displaced by 120 degrees) into currents
  *         Ialpha and Ibeta in a stationary qd reference frame.
  *                               Ialpha = Ia
  *                       Ibeta = -(2*Ib+Ia)/sqrt(3)
  * @param  Curr_Input: stator current Ia and Ib in Curr_Components format
  * @retval Stator current Ialpha and Ibeta in Curr_Components format
  */
Curr_Components clarke( Curr_Components Curr_Input )
{
	  Curr_Components Curr_Output;
	
	  int32_t qIa_divSQRT3_tmp, qIb_divSQRT3_tmp ;
	  int32_t wIbeta_tmp;
	  int16_t hIbeta_tmp;
	
	  /* qIalpha = qIas*/
	  Curr_Output.qI_Component1 = Curr_Input.qI_Component1;
	
	  qIa_divSQRT3_tmp = divSQRT_3 * ( int32_t )Curr_Input.qI_Component1;
	
	  qIb_divSQRT3_tmp = divSQRT_3 * ( int32_t )Curr_Input.qI_Component2;
	
	  /*qIbeta = -(2*qIbs+qIas)/sqrt(3)*/
#ifdef FULL_MISRA_C_COMPLIANCY
	  wIbeta_tmp = ( -( qIa_divSQRT3_tmp ) - ( qIb_divSQRT3_tmp ) -
					 ( qIb_divSQRT3_tmp ) ) / 32768;
#else
	  /* WARNING: the below instruction is not MISRA compliant, user should verify
		that Cortex-M3 assembly instruction ASR (arithmetic shift right) is used by
		the compiler to perform the shift (instead of LSR logical shift right) */
	
	  wIbeta_tmp = ( -( qIa_divSQRT3_tmp ) - ( qIb_divSQRT3_tmp ) -
					 ( qIb_divSQRT3_tmp ) ) >> 15;
#endif
	
	  /* Check saturation of Ibeta */
	  if ( wIbeta_tmp > INT16_MAX )
	  {
		hIbeta_tmp = INT16_MAX;
	  }
	  else if ( wIbeta_tmp < ( -32768 ) )
	  {
		hIbeta_tmp = ( -32768 );
	  }
	  else
	  {
		hIbeta_tmp = ( int16_t )( wIbeta_tmp );
	  }
	
	  Curr_Output.qI_Component2 = hIbeta_tmp;
	
	  if ( Curr_Output.qI_Component2 == ( int16_t )( -32768 ) )
	  {
		Curr_Output.qI_Component2 = -32767;
	  }
	
	  return ( Curr_Output );

}

/**
  * @brief  This function transforms stator currents Ialpha and Ibeta, which
  *         belong to a stationary qd reference frame, to a rotor flux
  *         synchronous reference frame (properly oriented), so as Iq and Id.
  *                   Id= Ialpha *sin(theta)+qIbeta *cos(Theta)
  *                   Iq=qIalpha *cos(Theta)-qIbeta *sin(Theta)
  * @param  Curr_Input: stator current Ialpha and Ibeta in Curr_Components format
  * @param  Theta: rotating frame angular position in q1.15 format
  * @retval Stator current Iq and Id in Curr_Components format
  */
Curr_Components park( Curr_Components Curr_Input, int16_t Theta )
{
	Curr_Components Curr_Output;
	int32_t qId_tmp_1, qId_tmp_2, qIq_tmp_1, qIq_tmp_2;
	Trig_Components Local_Vector_Components;
	int32_t wIqd_tmp;
	int16_t hIqd_tmp;

	Local_Vector_Components = trig_functions( Theta );

	/*No overflow guaranteed*/
	qIq_tmp_1 = Curr_Input.qI_Component1 * ( int32_t )Local_Vector_Components.hCos;

	/*No overflow guaranteed*/
	qIq_tmp_2 = Curr_Input.qI_Component2 * ( int32_t )Local_Vector_Components.hSin;

	/*Iq component in Q1.15 Format */
#ifdef FULL_MISRA_C_COMPLIANCY
	wIqd_tmp = ( qIq_tmp_1 - qIq_tmp_2 ) / 32768;
#else
	/* WARNING: the below instruction is not MISRA compliant, user should verify
	that Cortex-M3 assembly instruction ASR (arithmetic shift right) is used by
	the compiler to perform the shift (instead of LSR logical shift right) */
	wIqd_tmp = ( qIq_tmp_1 - qIq_tmp_2 ) >> 15;
#endif

	/* Check saturation of Iq */
	if ( wIqd_tmp > INT16_MAX )
	{
		hIqd_tmp = INT16_MAX;
	}
	else if ( wIqd_tmp < ( -32768 ) )
	{
		hIqd_tmp = ( -32768 );
	}
	else
	{
		hIqd_tmp = ( int16_t )( wIqd_tmp );
	}

	Curr_Output.qI_Component1 = hIqd_tmp;

	if ( Curr_Output.qI_Component1 == ( int16_t )( -32768 ) )
	{
		Curr_Output.qI_Component1 = -32767;
	}

	/*No overflow guaranteed*/
	qId_tmp_1 = Curr_Input.qI_Component1 * ( int32_t )Local_Vector_Components.hSin;

	/*No overflow guaranteed*/
	qId_tmp_2 = Curr_Input.qI_Component2 * ( int32_t )Local_Vector_Components.hCos;

	/*Id component in Q1.15 Format */
#ifdef FULL_MISRA_C_COMPLIANCY
	wIqd_tmp = ( qId_tmp_1 + qId_tmp_2 ) / 32768;
#else
	/* WARNING: the below instruction is not MISRA compliant, user should verify
	that Cortex-M3 assembly instruction ASR (arithmetic shift right) is used by
	the compiler to perform the shift (instead of LSR logical shift right) */
	wIqd_tmp = ( qId_tmp_1 + qId_tmp_2 ) >> 15;
#endif

	/* Check saturation of Id */
	if ( wIqd_tmp > INT16_MAX )
	{
		hIqd_tmp = INT16_MAX;
	}
	else if ( wIqd_tmp < ( -32768 ) )
	{
		hIqd_tmp = ( -32768 );
	}
	else
	{
		hIqd_tmp = ( int16_t )( wIqd_tmp );
	}

	Curr_Output.qI_Component2 = hIqd_tmp;

	if ( Curr_Output.qI_Component2 == ( int16_t )( -32768 ) )
	{
		Curr_Output.qI_Component2 = -32767;
	}

	return ( Curr_Output );

}

/**
  * @brief  This function transforms stator voltage qVq and qVd, that belong to
  *         a rotor flux synchronous rotating frame, to a stationary reference
  *         frame, so as to obtain qValpha and qVbeta:
  *                  Valfa= Vq*Cos(theta)+ Vd*Sin(theta)
  *                  Vbeta=-Vq*Sin(theta)+ Vd*Cos(theta)
  * @param  Curr_Input: stator voltage Vq and Vd in Volt_Components format
  * @param  Theta: rotating frame angular position in q1.15 format
  * @retval Stator voltage Valpha and Vbeta in Volt_Components format
  */
Volt_Components park_rev( Volt_Components Volt_Input, int16_t Theta )
{
	int32_t qValpha_tmp1, qValpha_tmp2, qVbeta_tmp1, qVbeta_tmp2;
	Trig_Components Local_Vector_Components;
	Volt_Components Volt_Output;

	Local_Vector_Components = trig_functions( Theta );

	/*No overflow guaranteed*/
	qValpha_tmp1 = Volt_Input.qV_Component1 * ( int32_t )Local_Vector_Components.hCos;
	qValpha_tmp2 = Volt_Input.qV_Component2 * ( int32_t )Local_Vector_Components.hSin;

#ifdef FULL_MISRA_C_COMPLIANCY
	Volt_Output.qV_Component1 = ( int16_t )( ( ( qValpha_tmp1 ) + ( qValpha_tmp2 ) ) / 32768 );
#else
	/* WARNING: the below instruction is not MISRA compliant, user should verify
	that Cortex-M3 assembly instruction ASR (arithmetic shift right) is used by
	the compiler to perform the shift (instead of LSR logical shift right) */
	Volt_Output.qV_Component1 = ( int16_t )( ( ( qValpha_tmp1 ) + ( qValpha_tmp2 ) ) >> 15 );
#endif

	qVbeta_tmp1 = Volt_Input.qV_Component1 * ( int32_t )Local_Vector_Components.hSin;
	qVbeta_tmp2 = Volt_Input.qV_Component2 * ( int32_t )Local_Vector_Components.hCos;

#ifdef FULL_MISRA_C_COMPLIANCY
	Volt_Output.qV_Component2 = ( int16_t )( ( qVbeta_tmp2 - qVbeta_tmp1 ) / 32768 );
#else
	/* WARNING: the below instruction is not MISRA compliant, user should verify
	that Cortex-M3 assembly instruction ASR (arithmetic shift right) is used by
	the compiler to perform the shift (instead of LSR logical shift right) */
	Volt_Output.qV_Component2 = ( int16_t )( ( qVbeta_tmp2 - qVbeta_tmp1 ) >> 15 );
#endif

	return ( Volt_Output );

}


/**
  * @brief  This function returns cosine and sine functions of the angle fed in
  *         input
  * @param  hAngle: angle in q1.15 format (-1~0.9999)
  * @retval Trig_Components Cos(angle) and Sin(angle) in Trig_Components format
  */
Trig_Components trig_functions( int16_t hAngle )
{
	int32_t shindex;
	uint16_t uhindex;

	Trig_Components Local_Components;

	/* 10 bit index computation  */
	shindex = ( ( int32_t )32768 + ( int32_t )hAngle );
	uhindex = ( uint16_t )shindex;
	uhindex /= ( uint16_t )64;
	/**
		| hAngle 			| angle 	| std 		|
		| (0,16384] 		| U0_90 	| (0,0.5]	|
		| (16384,32767]		| U90_180 	| (0.5,0.99]|
		| (-16384,-1] 		| U270_360 	| (0,-0.5] 	|
		| (-16384,-32768]	| U180_270 	| (-0.5,-1)	|
	*/
//SIN_MASK        0x0300u
	switch ( ( uint16_t )( uhindex ) & SIN_MASK )
	{          
//0x0200u
	  case U0_90:
		Local_Components.hSin = hSin_Cos_Table[( uint8_t )( uhindex )];
		Local_Components.hCos = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		break;
//0x0300u
	  case U90_180:
		Local_Components.hSin = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( uhindex )];
		break;
//0x0000u
	  case U180_270:
		Local_Components.hSin = -hSin_Cos_Table[( uint8_t )( uhindex )];
		Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		break;
//0x0100u
	  case U270_360:
		Local_Components.hSin =  -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		Local_Components.hCos =  hSin_Cos_Table[( uint8_t )( uhindex )];
		break;
	  default:
		break;
	}
	return ( Local_Components );
}

