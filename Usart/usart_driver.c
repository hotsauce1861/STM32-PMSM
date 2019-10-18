#include <stdio.h>
#include "stm32f10x_usart.h"
#include "usart_driver.h"

//default 8N1
#define COM_PORT	USART3
#define TX_PIN		GPIO_Pin_10
#define RX_PIN		GPIO_Pin_11
#define BAUDRATE	115200

static USART_InitTypeDef USART_InitStructure;

static void rcc_init(void){
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

void usart_send_char(char ch){
	USART_SendData(COM_PORT, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(COM_PORT, USART_FLAG_TC) == RESET)
	{}
}

uint8_t usart_recv_char(){
	/* Wait the byte is entirely received by USARTy */
    while(USART_GetFlagStatus(COM_PORT, USART_FLAG_RXNE) == RESET)
    {}
    /* Store the received byte in the RxBuffer1 */
    return (uint8_t)USART_ReceiveData(COM_PORT);
}

void usart_test_echo(){
	uint8_t tmp_dat = 0xff;

	tmp_dat = usart_recv_char();
	usart_send_char(tmp_dat);
}

void usart_init(void){

	rcc_init ();
	gpio_init ();
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

	/* Enable USART */
	USART_Cmd(COM_PORT, ENABLE);
}

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

//GETCHAR_PROTOTYPE
//{
//	return usart_recv_char();
//}

