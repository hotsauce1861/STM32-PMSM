#include <stdio.h>
#include "encoder.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "position.h"
#include "svpwm_math.h"

/*
0x7FFF - 0x7787
0x7FFF - 0x7788
0x7FFF - 0x778E
0x7FFF - 0x7791
0x7FFF - 0x7790
0x7FFF - 0x7791
0x7FFF - 0x778F
0x7FFF - 0x778E
0x7FFF - 0x7791
0x7FFF - 0x7790
0x7FFF - 0x7790
*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile int32_t N = 0;
volatile uint32_t EncCnt = 0;
volatile int32_t angle_cnt = 0;
volatile int16_t angle_one_circle_num = 0;
volatile int16_t first_zero_cnt = 0;
volatile uint8_t zero_pos_flag = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void encoder_pin_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void encoder_rcc_init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
}

void encoder_tim_init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure; 		
	
	//TIM_TimeBaseStructure.TIM_Period = ENCODER_MAX_CNT;
	TIM_TimeBaseStructure.TIM_Period = ENCODER_ONE_CIRCLE_CNT - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/*
		Set tim as encoder mode
	*/
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12, 
									TIM_ICPolarity_BothEdge, 
									TIM_ICPolarity_BothEdge);

	//TIM_ICStructInit(&TIM_ICInitStructure);//将结构体中的内容缺省输入
	//TIM_ICInitStructure.TIM_ICFilter = 4;  //选择输入比较滤波器 
	//TIM_ICStructInit(&TIM_ICInitStructure); // 设置默认值
/*
	TIM_ICStructInit(&TIM_ICInitStructure); // 设置默认值

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPrescaler = 0;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInitStructure.TIM_ICPrescaler = 0;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
*/
	//must clear it flag before enabe interrupt
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);	
	
	//TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
	TIM_SetCounter(TIM3,0);
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_Cmd(TIM3, ENABLE);
	
//	TIM3->CCMR1 |= 0x0001;
//	TIM3->CCMR2 |= 0x0001;
//	TIM3->CCER &= ~(0x0001<<1);
//	TIM3->CCER &= ~(0x0001<<5);
//	TIM3->SMCR |= 0x0003;
//	TIM3->CR1 |= 0x0001;

}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void encoder_irq_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

void encoder_init(void){
	encoder_rcc_init();
	encoder_pin_init();
	encoder_irq_init();
	encoder_tim_init();
}

/**
 * @brief 	获取编码器旋转方向
 * @retval	MOTO_DIR
 *				-FORWARD
 *				-BACK
 */
MOTO_DIR encoder_get_motor_dir(void)
{
	if((TIM3->CR1 & 0x0010) == 0x0010){
		return FORWARD;
	}else{
		return BACK;
	}
}

int16_t encoder_get_e_theta(void){
  int32_t temp;  
  temp = -(int32_t)(TIM_GetCounter(TIM3)) * (int32_t)(UINT32_MAX / ENCODER_ONE_CIRCLE_CNT);         
  temp *= 2;  
  return((int16_t)(temp/65536)); 
	
}

int16_t encoder_get_m_theta(void){
	int32_t temp;  
	temp = (int32_t)(TIM_GetCounter(TIM3)) * (int32_t)(UINT32_MAX / ENCODER_ONE_CIRCLE_CNT);         
	return((int16_t)(temp/65536)); 
}

void encoder_reset_aligment(void){
	TIM_SetCounter(TIM3, ENCODER_ZERO_VAL);	
}

void encoder_reset_zero(void){
	TIM_SetCounter(TIM3, 0);	
}

/**
 * @brief convert encoder count to angular posiong
 * @retval angle in format Q15
 */
int16_t encoder_conv_angular_pos(int16_t enc_cnt){
	int16_t angle = 0;
	angle = (int32_t)enc_cnt*Q15/ENCODER_ZERO_CIRCLE_CNT;
	return angle;
}


void encoder_set_to_zero_position(void){
	TIM_SetCounter(TIM3, ENCODER_ZERO_VAL);	
}

/**
 * @brief 设置零点信号的值
 */
void encoder_set_zero_pos_flag(uint8_t flag){
	//TODO flag值的合理性校验
	zero_pos_flag = flag;
}

/**
 * @brief 查询是否产生零点信号
 */
uint8_t encoder_get_zero_pos_flag(void){
	if(zero_pos_flag == 1){
		zero_pos_flag = 0;
		return 1;
	}
	return 0;
}

/**
 * @brief 清除计数器寄存器的值
 */
void encoder_clear_timercounter(void){
	TIM_SetCounter(TIM3,ENCODER_ZERO_VAL);
}

/**
 * @brief 获取相对于六个扇区的转子角度
 */
int16_t encoder_get_sector_angular(void){

	static int16_t old_cnt = 0;
	int16_t tmp_cnt = 0,detal_cnt = 0,cnt = 0;
	return (int16_t)(tmp_cnt+detal_cnt);
}

/**
 * @brief 获取第一次产生零点信号编码器计数
 */
int16_t encoder_get_first_zero_cnt(void){	
	return (int16_t)(first_zero_cnt);
}

/**
 * @brief 获取计数器寄存器的计数值/已经减去偏移量
 */
int16_t encoder_get_timecounter_cnt(void){
	return (int16_t)(TIM3->CNT - ENCODER_ZERO_VAL);
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
#if 1

volatile uint16_t overflow_times = 0;
#define ENCODER_TIMER TIM3

static volatile u16 hEncoder_Timer_Overflow; 
int16_t previous_angle = 0;
signed long long temp_spd;
int32_t wdelta_angle;
int16_t enconder_calc_rot_speed(void){
	
	static uint8_t bIs_First_Measurement = 1;
	

	uint16_t hEnc_Timer_Overflow_sample_one, hEnc_Timer_Overflow_sample_two;
	uint16_t hCurrent_angle_sample_one, hCurrent_angle_sample_two;	
	
	int16_t haux;
	if (!bIs_First_Measurement)
	{
		// 1st reading of overflow counter    
		hEnc_Timer_Overflow_sample_one = hEncoder_Timer_Overflow; 
		// 1st reading of encoder timer counter
		hCurrent_angle_sample_one = ENCODER_TIMER->CNT;
		// 2nd reading of overflow counter
		hEnc_Timer_Overflow_sample_two = hEncoder_Timer_Overflow;  
		// 2nd reading of encoder timer counter
		hCurrent_angle_sample_two = ENCODER_TIMER->CNT;      

		// Reset hEncoder_Timer_Overflow and read the counter value for the next
		// measurement
		hEncoder_Timer_Overflow = 0;
		haux = ENCODER_TIMER->CNT;   

		if (hEncoder_Timer_Overflow != 0) 
		{
			haux = ENCODER_TIMER->CNT; 
			hEncoder_Timer_Overflow = 0;            
		}

		if (hEnc_Timer_Overflow_sample_one != hEnc_Timer_Overflow_sample_two)
		{ 	
			//Compare sample 1 & 2 and check if an overflow has been generated right 
			//after the reading of encoder timer. If yes, copy sample 2 result in 
			//sample 1 for next process 
			hCurrent_angle_sample_one = hCurrent_angle_sample_two;
			hEnc_Timer_Overflow_sample_one = hEnc_Timer_Overflow_sample_two;
		}

		if ( (ENCODER_TIMER->CR1 & TIM_CounterMode_Down) == TIM_CounterMode_Down)  
		{
			// encoder timer down-counting
			wdelta_angle = (s32)(hCurrent_angle_sample_one - previous_angle - 
			(hEnc_Timer_Overflow_sample_one) * ENCODER_ONE_CIRCLE_CNT);
		}else  {
			//encoder timer up-counting
			wdelta_angle = (s32)(hCurrent_angle_sample_one - previous_angle + 
			(hEnc_Timer_Overflow_sample_one) * ENCODER_ONE_CIRCLE_CNT);
		}

		// speed computation as delta angle * 1/(speed sempling time)
		temp_spd = (signed long long)(wdelta_angle * SPEED_SAMPLING_FREQ);                                                                
		temp_spd *= 10;  // 0.1 Hz resolution
		temp_spd /= ENCODER_ONE_CIRCLE_CNT;

	} //is first measurement, discard it
	else
	{
		bIs_First_Measurement = 0;
		temp_spd = 0;
		hEncoder_Timer_Overflow = 0;
		haux = ENCODER_TIMER->CNT;       
		// Check if hEncoder_Timer_Overflow is still zero. In case an overflow IT 
		// occured it resets overflow counter and wPWM_Counter_Angular_Velocity
		if (hEncoder_Timer_Overflow != 0) 
		{
			haux = ENCODER_TIMER->CNT; 
			hEncoder_Timer_Overflow = 0;            
		}
	}

	previous_angle = haux;  
 
	return (int16_t)temp_spd;
}

#define SPEED_BUFFER_SIZE 7
static int16_t hSpeed_Buffer[SPEED_BUFFER_SIZE];
int16_t encoder_calc_ave(int16_t *pdata, uint8_t len);

int16_t enconder_get_ave_speed(void){
	static uint8_t index = 0;
	int32_t wtemp_spd = 0;
	uint16_t abs_spd = 0;  
	wtemp_spd = enconder_calc_rot_speed();
	abs_spd = (wtemp_spd > 0?wtemp_spd:-wtemp_spd);
	/**
		TODO 
	//	限幅处理
	*/
	hSpeed_Buffer[index++%SPEED_BUFFER_SIZE] = (int16_t)wtemp_spd;
	
	return encoder_calc_ave(hSpeed_Buffer,SPEED_BUFFER_SIZE);
	
}

int16_t encoder_calc_ave(int16_t *pdata, uint8_t len){

	int32_t wsum = 0;
	int16_t temp = 0;
	int i=0,j=0;
	temp = pdata[i];
	if(len < 2){
		return 0;
	}
	for( ; i<len; i++){
				
		for( j = i+1; j<len; j++){
			if(pdata[i] > pdata[j]){
				temp = pdata[j];
				pdata[j] = pdata[i];
				pdata[i] = temp;
			}
		}
	}
	
	for(i=1; i<len-1; i++){
		wsum+= pdata[i];
	}
	
	return (int16_t)(wsum/(len-2));
}

void enconder_get_pulse_cnt(int16_t *pdata){
	int32_t cnt = 0;
	if(overflow_times > 0){
		cnt = overflow_times*ENCODER_ONE_CIRCLE_CNT + TIM3->CNT;
	}else{
		cnt = overflow_times*ENCODER_ONE_CIRCLE_CNT - TIM3->CNT;
	}	
	if(cnt >= INT16_MAX){
		*pdata = INT16_MAX;
	}else{
		*pdata = cnt;
	}
	if(cnt <= INT16_MIN){
		*pdata = INT16_MIN;
	}else{
		*pdata = cnt;
	}
	overflow_times = 0;
}

void enconder_get_rpm(int16_t *pdata){
	
}
void TIM3_IRQHandler(void)
{ 
	if(TIM3->SR&(TIM_FLAG_Update)){		
		
		if(overflow_times >= UINT16_MAX){
			overflow_times = UINT16_MAX;
		}else{
			overflow_times++;
		}
	}
	TIM3->SR&=~(TIM_FLAG_Update);		
}

#endif

#if 1

void EXTI9_5_IRQHandler(void){

	if(EXTI_GetITStatus(EXTI_Line5) == SET){ 
		zero_pos_flag = 1;
		//first_zero_cnt = angle_cnt;
	}
	EXTI_ClearITPendingBit(EXTI_Line5);
	
}

#endif
