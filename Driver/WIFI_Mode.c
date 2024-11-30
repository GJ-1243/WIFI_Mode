#include "WIFI_Mode.h"

static inline void hd_spi_send_done(void)
{
    uint8 header[3] = { SPI_WRITE_END,0x00,0x00 };
    SPI_CS_LOW;
    SPI_SEND_BYTES((uint8*)(header), sizeof(header));
    SPI_CS_HIGH;
}

void WIFI_Mode_Init(uint32 n, Baud_t nbit)
{
#if USE_UART
	uart_init(n*(uint32)nbit);
#endif
#if USE_HD_SPI
	spi_hd_init(n*(uint32)nbit);
#if USE_SOFT_CS
	SOFT_CS_GPIO_INIT(1);
#endif
#endif
#if USE_FD_SPI
	spi_fd_init(n*(uint32)nbit);
#if USE_SOFT_CS
	SOFT_CS_GPIO_INIT(1);
#endif
#endif
}


void multi_src_clear(multi_src_t* multi_src_ptr)
{
	ASSERT(NULL!= multi_src_ptr);
	multi_src_ptr->src_num = 0;
}

uint8 multi_src_add(multi_src_t* multi_src_ptr, void* src, uint32 length)
{
    ASSERT(NULL != multi_src_ptr);
    ASSERT(NULL != src);
    if (multi_src_ptr->src_num < SRC_MAX_NUM)
    {
        multi_src_ptr->src[multi_src_ptr->src_num] = (uint8*)src;
        multi_src_ptr->src_len[multi_src_ptr->src_num] = length;
        multi_src_ptr->src_num++;
        return 1;
    }
    else
    {
        return 0;
    }
}

void send_8bit_uart(uint8 data)
{
    UART_SEND_BYTES(&data, 1);
}

void send_8bit_array_uart(uint8* array, uint32 length)
{
    if (length > 0)
    {
        ASSERT(NULL != array);
        UART_SEND_BYTES(array, length);
    }
}

void send_8bit_array_multi_uart(multi_src_t multi_src)
{
    for (uint8 i = 0; i < multi_src.src_num; i++)
    {
        if (multi_src.src_len[i] > 0)
        {
            UART_SEND_BYTES(multi_src.src[i], multi_src.src_len[i]);
        }
    }
}

void send_8bit_fd_spi(uint8 data)
{
    SPI_CS_LOW;
    SPI_SEND_BYTES(&data, 1);
    SPI_CS_HIGH;
}

void send_8bit_array_fd_spi(uint8* array, uint32 length)
{
    ASSERT(NULL != array);
    while (length > SPI_MAX_TRANS_SIZE)
    {
        SPI_CS_LOW;
        SPI_SEND_BYTES(array, SPI_MAX_TRANS_SIZE);
        SPI_CS_HIGH;
        array += SPI_MAX_TRANS_SIZE;
        length -= SPI_MAX_TRANS_SIZE;
    }
    if (length > 0)
    {
        SPI_CS_LOW;
        SPI_SEND_BYTES(array, length);
        SPI_CS_HIGH;
    }
}

void send_8bit_array_multi_fd_spi(multi_src_t multi_src)
{
    uint32 send_len = 0; // 已发送长度
    uint32 part_len = 0;
    SPI_CS_LOW;
    for (uint8 i = 0; i < multi_src.src_num; i++)
    {
        if (send_len + multi_src.src_len[i] > SPI_MAX_TRANS_SIZE)
        {
            part_len = SPI_MAX_TRANS_SIZE - send_len;
            SPI_SEND_BYTES(multi_src.src[i], part_len);
            SPI_CS_HIGH; // 结束这一次SPI传输
            multi_src.src[i] += part_len;
            multi_src.src_len[i] -= part_len;
            send_len = 0; // 重置
            i--;          // 回滚
            SPI_CS_LOW;   // 开启下一次SPI传输
        }
        else if (multi_src.src_len[i] > 0)
        {
            SPI_SEND_BYTES(multi_src.src[i], multi_src.src_len[i]);
            send_len += multi_src.src_len[i];
        }
    }
    SPI_CS_HIGH;
}

//*********************************************************************************************************

void send_8bit_hd_spi(uint8 data)
{
    uint8 header[3] = { SPI_WRITE_DATA,0x00,0x00 };
    SPI_CS_LOW;
    SPI_SEND_BYTES((uint8*)(header), sizeof(header));
    SPI_SEND_BYTES(&data, 1);
    SPI_CS_HIGH;
    hd_spi_send_done();
}

void send_8bit_array_hd_spi(uint8* array, uint32 length)
{
    uint8 header[3] = { SPI_WRITE_DATA,0x00,0x00 };
    while (length > SPI_MAX_TRANS_SIZE)
    {
        SPI_CS_LOW;
        SPI_SEND_BYTES((uint8*)(header), sizeof(header));
        SPI_SEND_BYTES(array, SPI_MAX_TRANS_SIZE);
        SPI_CS_HIGH;
        hd_spi_send_done();
        array += SPI_MAX_TRANS_SIZE;
        length -= SPI_MAX_TRANS_SIZE;
    }
    if (length > 0)
    {
        SPI_CS_LOW;
        SPI_SEND_BYTES((uint8*)(header), sizeof(header));
        SPI_SEND_BYTES(array, length);
        SPI_CS_HIGH;
        hd_spi_send_done();
    }
}

void send_8bit_array_multi_hd_spi(multi_src_t multi_src)
{
    uint8 header[3] = { SPI_WRITE_DATA,0x00,0x00 };
    uint32 send_len = 0; // 已发送长度
    uint32 part_len = 0;
    SPI_CS_LOW;
    SPI_SEND_BYTES((uint8*)(header), sizeof(header));
    for (uint8 i = 0; i < multi_src.src_num; i++)
    {
        if (send_len + multi_src.src_len[i] > SPI_MAX_TRANS_SIZE)
        {
            part_len = SPI_MAX_TRANS_SIZE - send_len;
            SPI_SEND_BYTES(multi_src.src[i], part_len);
            SPI_CS_HIGH; // 结束这一次SPI传输
            hd_spi_send_done();
            multi_src.src[i] += part_len;
            multi_src.src_len[i] -= part_len;
            send_len = 0; // 重置
            i--;          // 回滚
            SPI_CS_LOW;   // 开启下一次SPI传输
            SPI_SEND_BYTES((uint8*)(header), sizeof(header));
        }
        else if (multi_src.src_len[i] > 0)
        {
            SPI_SEND_BYTES(multi_src.src[i], multi_src.src_len[i]);
            send_len += multi_src.src_len[i];
        }
    }
    SPI_CS_HIGH;
    hd_spi_send_done();
}

#if USE_HD_SPI

static inline uint8 hd_spi_read_status(uint32* read_len)
{
    uint8 header[3] = { SPI_READ_BUF,RD_STATUS_REG,0x00 };
    uint8 status[4];
    SPI_CS_LOW;
    SPI_SEND_BYTES((uint8*)(header), sizeof(header));
    SPI_READ_BYTES((uint8*)(status), sizeof(status));
    SPI_CS_HIGH;
    *read_len = (uint32)((uint16)(status[2] << 8 | status[3]));
    return status[0];
}

static inline void hd_spi_read_done(void)
{
    uint8 header[3] = { SPI_READ_END,0x00,0x00 };
    SPI_CS_LOW;
    SPI_SEND_BYTES((uint8*)(header), sizeof(header));
    SPI_CS_HIGH;
}

uint32 read_8bit_array_hd_spi(uint8* buffer, uint32 length)
{
    uint32 read_len = 0;
    if (hd_spi_read_status(&read_len))
    {
        read_len = read_len > length ? length : read_len;
        uint8 header[3] = {SPI_READ_DATA,0x00,0x00 };
        SPI_CS_LOW;
        SPI_SEND_BYTES((uint8*)(header), sizeof(header));
        SPI_READ_BYTES(buffer, read_len);
        SPI_CS_HIGH;
        hd_spi_read_done();
    }
    return read_len;
}
#else

__weak uint32 read_8bit_array_hd_spi(uint8* buffer, uint32 length)
{
    PASSWRING(buffer);
    PASSWRING(length);
    return 0;
}
#endif
