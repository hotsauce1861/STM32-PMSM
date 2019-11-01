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
 
void usart_init(void);
void usart_send_char(char ch);
void usart_test_echo(void);
uint8_t usart_recv_char(void);
void usart_printf(const char *fmt, ...);

//extern GETCHAR_PROTOTYPE;

#endif
