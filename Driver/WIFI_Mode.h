#ifndef __WIFI_MODE_H_
#define __WIFI_MODE_H_
#include "string.h"
#include "zf_common_headfile.h"
#define __weak  __attribute__((weak))
#define PASSWRING(x)        if(x){}
#define PASSWRINGT(a,b)     if((a)&&(b)){}
#ifndef _zf_common_headfile_h_
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;
#endif
//*******************************************************************************************************/
#define SPI_MAX_TRANS_SIZE   (4096)// SPIһ�δ�������ֽ���, ���16384, ����˫��ȫ˫��SPI���4096
// UART
#define USE_UART             (0)
// ��˫��SPI 1bit
#define USE_HD_SPI           (1)
// ȫ˫��SPI 1bit
#define USE_FD_SPI           (0)
//�Ƿ�ʹ�����CS
#define USE_SOFT_CS          (1)
//���CS��ʼ��
#define SOFT_CS_GPIO_INIT(x) PASSWRING(x)
//���CS��ʱ
#define SOFT_CS_TIME         (150)
//�������CS
#define SOFT_CS_LOW          (void)(0)//�û��� (void)(0)����ʾ��Ҫ�滻Ϊ�û�����
//�������CS
#define SOFT_CS_HIGH          (void)(0)
//*******************************************************************************************************/
typedef enum
{
	Baud_1b=1,
	Baud_1kb=1000,
	Baud_1Mb=1000000
}Baud_t;
//*******************************************************************************************************/
#define SRC_MAX_NUM (10)
//*******************************************************************************************************/
//**************************************�û�����(��Ҫ�û��Լ�����)************************************************/
#define spi_hd_init(baud)      PASSWRING((baud))
#define spi_fd_init(baud)      PASSWRING((baud))
#define uart_init(baud)        PASSWRING((baud))
#define UART_SEND_BYTES(p,len) PASSWRINGT((p),(len))
#define SPI_SEND_BYTES(p,len)  PASSWRINGT((p),(len))
#define SPI_READ_BYTES(p,len)  PASSWRINGT((p),(len))
#define ASSERT(x)              PASSWRING((x))
//*******************************************************************************************************/
//*******************************************************************************************************/
//*****************************************���������ֹ�û��޸�*************************************************/
//*******************************************************************************************************/
//*******************************************************************************************************/
#if USE_UART+USE_HD_SPI+USE_FD_SPI>1
#error "USE Define ERROR"
#endif
#if SRC_MAX_NUM<1
#error "The SRC_MAX_NUM must be greater than 0"
#endif
//*******************************************************************************************************/
#if USE_UART
#define send_8bit(data)                            send_8bit_uart((uint8)(data))
#define send_8bit_array(array, length)             send_8bit_array_uart((uint8*)(array), (length))
#define send_8bit_array_multi(multi_src)           send_8bit_array_multi_uart(multi_src)
#endif
//*******************************************************************************************************/
#if USE_FD_SPI
#define send_8bit(data)                            send_8bit_fd_spi((uint8)(data))
#define send_8bit_array(array, length)             send_8bit_array_fd_spi((uint8*)(array), (length))
#define send_8bit_array_multi(multi_src)           send_8bit_array_multi_fd_spi(multi_src)
#if SPI_MAX_TRANS_SIZE<1||SPI_MAX_TRANS_SIZE>16384
#error "The SPI_MAX_TRANS_SIZE must be between 1 and 4096"
#endif
#endif
//*******************************************************************************************************/
#if USE_HD_SPI
#define send_8bit(data)                           send_8bit_hd_spi((uint8)(data))
#define send_8bit_array(array, length)            send_8bit_array_hd_spi((uint8*)(array),(length))
#define send_8bit_array_multi(multi_src)          send_8bit_array_multi_hd_spi(multi_src)
#if SPI_MAX_TRANS_SIZE<1||SPI_MAX_TRANS_SIZE>4096
#error "The SPI_MAX_TRANS_SIZE must be between 1 and 16384"
#endif
#endif
//*******************************************************************************************************/

#if USE_SOFT_CS
#define SPI_CS_LOW do{\
for(uint16 i=SOFT_CS_TIME;i>0;--i)SOFT_CS_LOW;\
}while(0)

#define SPI_CS_HIGH do{\
for(uint16 i=SOFT_CS_TIME;i>0;--i)SOFT_CS_HIGH;\
}while(0)
#else
#define SPI_CS_LOW (void)(0)
#define SPI_CS_HIGH (void)(0)
#endif
//*******************************************************************************************************/
#define SPI_WRITE_BUF     (0x02)
#define SPI_READ_BUF      (0x02)
#define SPI_WRITE_DATA    (0x03)
#define SPI_READ_DATA     (0x04)
#define SPI_WRITE_END     (0x07)
#define SPI_READ_END      (0x08)
#define RD_STATUS_REG     (0x04)
//*******************************************************************************************************/
typedef struct
{
	uint8 src_num;                       // ����Դ����
	uint8* src[SRC_MAX_NUM];             // ����Դ�׵�ַ����
	uint32 src_len[SRC_MAX_NUM];    // ����Դ��������
}multi_src_t;
void WIFI_Mode_Init(uint32 n, Baud_t nbit);
void multi_src_clear(multi_src_t* multi_src_ptr);
uint8 multi_src_add(multi_src_t* multi_src_ptr, void* src, uint32 length);
void send_8bit_uart(uint8 data);
void send_8bit_array_uart(uint8* array, uint32 length);
void send_8bit_array_multi_uart(multi_src_t multi_src);
void send_8bit_fd_spi(uint8 data);
void send_8bit_array_fd_spi(uint8* array, uint32 length);
void send_8bit_array_multi_fd_spi(multi_src_t multi_src);
void send_8bit_hd_spi(uint8 data);
void send_8bit_array_hd_spi(uint8* array, uint32 length);
void send_8bit_array_multi_hd_spi(multi_src_t multi_src);
__weak uint32 read_8bit_array_hd_spi(uint8* buffer, uint32 length);
#endif
