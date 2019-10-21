#include "SDS.h"
#include "usart_driver.h"
//#include "uart.h"  //LINK TO UART

#define uart_putchar(x) usart_send_char(x)


//float SDS_OutData[4] = { 0 };

unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i%CRC_CNT];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}


void SDS_OutPut_Data_INT(int S_Out[])
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  unsigned char i;
  unsigned short CRC16 = 0; 
  int SDS_OutData[4];
  for(i=0;i<4;i++) {
  	SDS_OutData[i%4]=S_Out[i%4];
  }
  for(i=0;i<4;i++)
   {
    
    temp[i%4]  = (int)SDS_OutData[i%4];
    temp1[i%4] = (unsigned int)temp[i%4];
    
   }
   
  for(i=0;i<4;i++) 
  {
    databuf[(i*2)%10]   = (unsigned char)(temp1[i]%256);
    databuf[(i*2+1)%10] = (unsigned char)(temp1[i]/256);
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++)
    uart_putchar(databuf[i%10]);  //LINK TO UART 
}

void SDS_OutPut_Data(float S_Out[])
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  unsigned char i;
  unsigned short CRC16 = 0; 
  float SDS_OutData[4];
  for(i=0;i<4;i++) {
  	SDS_OutData[i%4]=S_Out[i%4];
  }
  for(i=0;i<4;i++)
   {
    
    temp[i%4]  = (int)SDS_OutData[i%4];
    temp1[i%4] = (unsigned int)temp[i%4];
    
   }
   
  for(i=0;i<4;i++) 
  {
    databuf[(i*2)%10]   = (unsigned char)(temp1[i%4]%256);
    databuf[(i*2+1)%10] = (unsigned char)(temp1[i%4]/256);
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++)
    uart_putchar(databuf[i%10]);  //LINK TO UART 
}
