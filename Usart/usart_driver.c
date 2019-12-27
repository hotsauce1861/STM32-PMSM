#include <stdio.h>
#include <stdarg.h>
#include "stm32f10x_usart.h"
#include "usart_driver.h"

uint8_t RxBuffer[USART_BUF_SIZE] = { 0 };

uart_mod_t user_uart_mod = {
	.rx_dat_len = 0,
	.head = 0,
	.tail = 0,
	.pfunc_rx_cbk = NULL,
	.pargs = NULL
};

static USART_InitTypeDef USART_InitStructure;

static void rcc_init(void){

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB 
							| RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE);
}

static void gpio_init(void){

  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = RX_PIN;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

static void dma_init(void){

  DMA_InitTypeDef DMA_InitStructure;

  /* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
 
	DMA_DeInit(USART_Rx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer;
	//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART_BUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(USART_Rx_DMA_Channel, &DMA_InitStructure);

}

static void irq_init(void){

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USART3_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQ_UART_PRE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = IRQ_UART_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void usart_send_char(char ch){

	/* Loop until the end of transmission */
	//while (USART_GetFlagStatus(COM_PORT, USART_FLAG_TC) == RESET){}
	while((COM_PORT->SR & USART_FLAG_TC) != USART_FLAG_TC){
	
	}	
	USART_SendData(COM_PORT, (uint8_t) ch);
}

uint8_t usart_recv_char(){
	/* Wait the byte is entirely received by USARTy */
    //while(USART_GetFlagStatus(COM_PORT, USART_FLAG_RXNE) == RESET){}
	while((COM_PORT->SR & USART_FLAG_RXNE) != USART_FLAG_RXNE){
	
	}
	
    /* Store the received byte in the RxBuffer1 */
    return (uint8_t)USART_ReceiveData(COM_PORT);
}

int usart_printf(const char *fmt, ... )
{
    uint8_t i = 0;
    uint8_t usart_tx_buf[128] = { 0 };
    va_list ap;

    va_start(ap, fmt );
    vsprintf((char*)usart_tx_buf, fmt, ap);
    va_end(ap);
	
	while(usart_tx_buf[i] && i < 128){
		usart_send_char(usart_tx_buf[i]);		 
		i++;
	}	
   	usart_send_char('\0');
	return 0;
}


void usart_test_echo(){
	uint8_t tmp_dat = 0xff;

	tmp_dat = usart_recv_char();
	usart_send_char(tmp_dat);
}

void usart_init(void){

	rcc_init ();
	gpio_init ();
	irq_init();
	
	/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* USART configuration */
	USART_Init(COM_PORT, &USART_InitStructure);

	USART_ITConfig(COM_PORT, USART_IT_IDLE, ENABLE);
	//USART_ITConfig(COM_PORT, USART_IT_RXNE, ENABLE);
	/* Enable USART */
	USART_Cmd(COM_PORT, ENABLE);
	
	USART_DMACmd(COM_PORT,USART_DMAReq_Rx, ENABLE);
	dma_init();
	DMA_ITConfig(USART_Rx_DMA_Channel, DMA_IT_TC, ENABLE);	
	DMA_ITConfig(USART_Rx_DMA_Channel, DMA_IT_TE, ENABLE);
	DMA_Cmd(USART_Rx_DMA_Channel, ENABLE);	

}

void usart_set_rx_cbk(uart_mod_t *pmod, rx_cbk pfunc,void *pargs){
	pmod->pargs = pargs;
	pmod->pfunc_rx_cbk = pfunc;
}

void DMA1_Channel3_IRQHandler(void){
     if(DMA_GetITStatus(USART_Rx_DMA_FLAG) == SET){        
        DMA_ClearITPendingBit(USART_Rx_DMA_FLAG);
    }
}

/**
  * @brief  This function handles USART3 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	uint8_t buf[USART_BUF_SIZE];
	uint16_t rect_len = 0;
	if(USART_GetITStatus(COM_PORT, USART_IT_IDLE) != RESET)	
	{
		uint8_t i = 0;
		USART_ReceiveData(COM_PORT);
		user_uart_mod.head = USART_BUF_SIZE - DMA_GetCurrDataCounter(USART_Rx_DMA_Channel);		
		//fifo is not full	
		while(user_uart_mod.head%USART_BUF_SIZE != user_uart_mod.tail%USART_BUF_SIZE){			
			user_uart_mod.rx_buf[i++] = RxBuffer[user_uart_mod.tail++%USART_BUF_SIZE];
		}
		user_uart_mod.rx_dat_len = i;
		//DMA_Cmd(USART_Rx_DMA_Channel, ENABLE);
		if(user_uart_mod.pfunc_rx_cbk != NULL){
			user_uart_mod.pfunc_rx_cbk(user_uart_mod.pargs);
		}
	}
	
	USART_ClearITPendingBit(COM_PORT, USART_IT_IDLE);
	//USART_ClearITPendingBit(COM_PORT, USART_IT_RXNE);
}



#if USE_MICROLIB_USART
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(COM_PORT, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(COM_PORT, USART_FLAG_TC) == RESET)
	{}
	return ch;
}

#else

#pragma import(__use_no_semihosting)                          
struct __FILE 
{ 
	int handle; 

}; 
FILE __stdout;       

int _sys_exit(int x)
{ 
	x = x; 
	return 0;
} 
int fputc(int ch, FILE *f)
{      
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(COM_PORT, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(COM_PORT, USART_FLAG_TC) == RESET)
	{}
	return ch;
}
#endif

