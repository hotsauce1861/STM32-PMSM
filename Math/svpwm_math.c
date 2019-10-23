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

//Q14
const int16_t sine_table[SINE_TABLE_SIZE] = { 

	0,		286,	572,	857,	1143,	1428,	1713,	1997,	2280,	2563,	2845,	3126,	3406,	3686,	3964,	4240,	4516,	4790,
	5063,	5334,	5604,	5872,	6138,	6402,	6664,	6924,	7182,	7438,	7692,	7943,	8192,	8438,	8682,	8923,	9162,	9397,
	9630,	9860,	10087,	10311,	10531,	10749,	10963,	11174,	11381,	11585,	11786,	11982,	12176,	12365,	12551,	12733,	12911,	13085,
	13255,	13421,	13583,	13741,	13894,	14044,	14189,	14330,	14466,	14598,	14726,	14849,	14968,	15082,	15191,	15296,	15396,	15491,
	15582,	15668,	15749,	15826,	15897,	15964,	16026,	16083,	16135,	16182,	16225,	16262,	16294,	16322,	16344,	16362,	16374,	16382,
	16384,	16382,	16374,	16362,	16344,	16322,	16294,	16262,	16225,	16182,	16135,	16083,	16026,	15964,	15897,	15826,	15749,	15668,
	15582,	15491,	15396,	15296,	15191,	15082,	14968,	14849,	14726,	14598,	14466,	14330,	14189,	14044,	13894,	13741,	13583,	13421,
	13255,	13085,	12911,	12733,	12551,	12365,	12176,	11982,	11786,	11585,	11381,	11174,	10963,	10749,	10531,	10311,	10087,	9860,
	9630,	9397,	9162,	8923,	8682,	8438,	8192,	7943,	7692,	7438,	7182,	6924,	6664,	6402,	6138,	5872,	5604,	5334,
	5063,	4790,	4516,	4240,	3964,	3686,	3406,	3126,	2845,	2563,	2280,	1997,	1713,	1428,	1143,	857,	572,	286,
	0,		-286,	-572,	-857,	-1143,	-1428,	-1713,	-1997,	-2280,	-2563,	-2845,	-3126,	-3406,	-3686,	-3964,	-4240,	-4516,	-4790,
	-5063,	-5334,	-5604,	-5872,	-6138,	-6402,	-6664,	-6924,	-7182,	-7438,	-7692,	-7943,	-8192,	-8438,	-8682,	-8923,	-9162,	-9397,
	-9630,	-9860,	-10087, -10311, -10531, -10749, -10963, -11174, -11381, -11585, -11786, -11982, -12176, -12365, -12551, -12733, -12911, -13085,
	-13255, -13421, -13583, -13741, -13894, -14044, -14189, -14330, -14466, -14598, -14726, -14849, -14968, -15082, -15191, -15296, -15396, -15491,
	-15582, -15668, -15749, -15826, -15897, -15964, -16026, -16083, -16135, -16182, -16225, -16262, -16294, -16322, -16344, -16362, -16374, -16382,
	-16384, -16382, -16374, -16362, -16344, -16322, -16294, -16262, -16225, -16182, -16135, -16083, -16026, -15964, -15897, -15826, -15749, -15668,
	-15582, -15491, -15396, -15296, -15191, -15082, -14968, -14849, -14726, -14598, -14466, -14330, -14189, -14044, -13894, -13741, -13583, -13421,
	-13255, -13085, -12911, -12733, -12551, -12365, -12176, -11982, -11786, -11585, -11381, -11174, -10963, -10749, -10531, -10311, -10087, -9860,
	-9630,	-9397,	-9162,	-8923,	-8682,	-8438,	-8192,	-7943,	-7692,	-7438,	-7182,	-6924,	-6664,	-6402,	-6138,	-5872,	-5604,	-5334,
	-5063,	-4790,	-4516,	-4240,	-3964,	-3686,	-3406,	-3126,	-2845,	-2563,	-2280,	-1997,	-1713,	-1428,	-1143,	-857,	-572,	-286

}; 

//Cos_Q14 * sqrt_3
const int16_t consine_table[SINE_TABLE_SIZE] = {

	0,		495,	990,	1485,	1980,	2473,	2966,	3458,	3949,	4439,	4928,	5415,	5900,	6384,	6865,	7345,	7822,	8297,
	8769,	9239,	9706,	10170,	10631,	11088,	11542,	11993,	12440,	12883,	13323,	13758,	14189,	14616,	15038,	15456,	15869,	16277,
	16680,	17078,	17471,	17859,	18241,	18618,	18989,	19354,	19713,	20066,	20413,	20754,	21089,	21417,	21739,	22054,	22362,	22664,
	22958,	23246,	23526,	23800,	24066,	24325,	24576,	24820,	25056,	25285,	25506,	25719,	25925,	26122,	26312,	26493,	26667,	26832,
	26989,	27138,	27279,	27411,	27535,	27651,	27758,	27857,	27947,	28029,	28102,	28166,	28222,	28270,	28309,	28339,	28361,	28374,
	28378,	28374,	28361,	28339,	28309,	28270,	28222,	28166,	28102,	28029,	27947,	27857,	27758,	27651,	27535,	27411,	27279,	27138,
	26989,	26832,	26667,	26493,	26312,	26122,	25925,	25719,	25506,	25285,	25056,	24820,	24576,	24325,	24066,	23800,	23526,	23246,
	22958,	22664,	22362,	22054,	21739,	21417,	21089,	20754,	20413,	20066,	19713,	19354,	18989,	18618,	18241,	17859,	17471,	17078,
	16680,	16277,	15869,	15456,	15038,	14616,	14189,	13758,	13323,	12883,	12440,	11993,	11542,	11088,	10631,	10170,	9706,	9239,
	8769,	8297,	7822,	7345,	6865,	6384,	5900,	5415,	4928,	4439,	3949,	3458,	2966,	2473,	1980,	1485,	990,	495,
	0,		-495,	-990,	-1485,	-1980,	-2473,	-2966,	-3458,	-3949,	-4439,	-4928,	-5415,	-5900,	-6384,	-6865,	-7345,	-7822,	-8297,
	-8769,	-9239,	-9706,	-10170, -10631, -11088, -11542, -11993, -12440, -12883, -13323, -13758, -14189, -14616, -15038, -15456, -15869, -16277,
	-16680, -17078, -17471, -17859, -18241, -18618, -18989, -19354, -19713, -20066, -20413, -20754, -21089, -21417, -21739, -22054, -22362, -22664,
	-22958, -23246, -23526, -23800, -24066, -24325, -24576, -24820, -25056, -25285, -25506, -25719, -25925, -26122, -26312, -26493, -26667, -26832,
	-26989, -27138, -27279, -27411, -27535, -27651, -27758, -27857, -27947, -28029, -28102, -28166, -28222, -28270, -28309, -28339, -28361, -28374,
	-28378, -28374, -28361, -28339, -28309, -28270, -28222, -28166, -28102, -28029, -27947, -27857, -27758, -27651, -27535, -27411, -27279, -27138,
	-26989, -26832, -26667, -26493, -26312, -26122, -25925, -25719, -25506, -25285, -25056, -24820, -24576, -24325, -24066, -23800, -23526, -23246,
	-22958, -22664, -22362, -22054, -21739, -21417, -21089, -20754, -20413, -20066, -19713, -19354, -18989, -18618, -18241, -17859, -17471, -17078,
	-16680, -16277, -15869, -15456, -15038, -14616, -14189, -13758, -13323, -12883, -12440, -11993, -11542, -11088, -10631, -10170, -9706,	-9239,
	-8769,	-8297,	-7822,	-7345,	-6865,	-6384,	-5900,	-5415,	-4928,	-4439,	-3949,	-3458,	-2966,	-2473,	-1980,	-1485,	-990,	-495
}; 


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
Volt_Components rev_park( Volt_Components Volt_Input, int16_t Theta )
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
  * @param  hAngle: angle in q1.15 format
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


	switch ( ( uint16_t )( uhindex ) & SIN_MASK )
	{
	  case U0_90:
		Local_Components.hSin = hSin_Cos_Table[( uint8_t )( uhindex )];
		Local_Components.hCos = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		break;

	  case U90_180:
		Local_Components.hSin = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( uhindex )];
		break;

	  case U180_270:
		Local_Components.hSin = -hSin_Cos_Table[( uint8_t )( uhindex )];
		Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		break;

	  case U270_360:
		Local_Components.hSin =  -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
		Local_Components.hCos =  hSin_Cos_Table[( uint8_t )( uhindex )];
		break;
	  default:
		break;
	}
	return ( Local_Components );
}

