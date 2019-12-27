#ifndef _USART_DRIVER_H
#define _USART_DRIVER_H
#include <stdio.h>
#include <stdint.h>

/* Private function prototypes -----------------------------------------------*/
#define USE_MICROLIB_USART 1

#if USE_MICROLIB_USART

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#define GETCHAR_PROTOTYPE int fgetc(FILE *f)

#endif /* __GNUC__ */
extern PUTCHAR_PROTOTYPE;
#else

#endif
 
//default 8N1
#define COM_PORT	USART3
#define TX_PIN		GPIO_Pin_10
#define RX_PIN		GPIO_Pin_11
#define BAUDRATE	115200

#define IRQ_UART_PRE	3
#define IRQ_UART_SUB	3

#define USART_Rx_DMA_Channel    DMA1_Channel3
#define USART_Rx_DMA_FLAG       DMA1_FLAG_TC3
#define USART_DR_Base           0x40004804
#define USART_BUF_SIZE			((uint16_t)16)

typedef void (*rx_cbk)(void* args);
struct uart_mod {
	
	uint8_t rx_buf[USART_BUF_SIZE];
	uint8_t rx_dat_len;
	uint8_t head;
	uint8_t tail;	
	
	void (*init)(void);
	
	void *pargs;
	rx_cbk pfunc_rx_cbk;
};
typedef struct uart_mod uart_mod_t;

extern  uart_mod_t user_uart_mod;
void usart_init(void);
void usart_set_rx_cbk(uart_mod_t *pmod, rx_cbk pfunc,void *pargs);
void usart_send_char(char ch);
void usart_test_echo(void);
uint8_t usart_recv_char(void);
int usart_printf(const char *fmt, ...);

//extern GETCHAR_PROTOTYPE;

#endif
