#include <stdint.h>
#include "uart.h"
#include "updix.h"


typedef enum
{
    UPDIX_LDS = 0x00,
    UPDIX_LD = 0x20,
    UPDIX_STS = 0x40,
    UPDIX_ST = 0x60,
    UPDIX_LDCS = 0x80,
    UPDIX_REPEAT = 0xA0,
    UPDIX_STCS = 0xC0,
    UPDIX_KEY = 0xE0,
} updix_instr_base_t;


static void set_instr_header(updix_instr_base_t instr_base, uint8_t *buffer,
                             uint8_t higher_byte, uint8_t lower_byte)
{
    buffer[0] = UPDIX_SYNCH;
    buffer[1] = (uint8_t)instr_base | (higher_byte << 2) | (lower_byte << 0);
}


static void serialize_address(uint32_t addr, updix_address_size_t address_size,
                              uint8_t *buffer)
{
    switch (address_size)
    {
        case UPDIX_ADDRESS_SIZE_8:
            buffer[0] = (uint8_t)(addr >> 0);
            break;
        case UPDIX_ADDRESS_SIZE_16:
            buffer[0] = (uint8_t)(addr >> 0);
            buffer[1] = (uint8_t)(addr >> 8);
            break;
        case UPDIX_ADDRESS_SIZE_24:
            buffer[0] = (uint8_t)(addr >> 0);
            buffer[1] = (uint8_t)(addr >> 8);
            buffer[2] = (uint8_t)(addr >> 16);
            break;
    }
}


static void serialize_data(uint16_t data, updix_data_size_t data_size, uint8_t *buffer)
{
    switch (data_size)
    {
        case UPDIX_DATA_SIZE_8:
            buffer[0] = (uint8_t)(data >> 0);
            break;
        case UPDIX_DATA_SIZE_16:
            buffer[0] = (uint8_t)(data >> 0);
            buffer[1] = (uint8_t)(data >> 8);
            break;
    }
}


updix_status_t updix_lds(uint32_t addr, updix_address_size_t address_size,
                         updix_data_size_t data_size, uint16_t *result)
{
    uint8_t tx_buffer[5] = {0x00};
    uint8_t rx_buffer[2] = {0x00};

    set_instr_header(UPDIX_LDS, tx_buffer, address_size, data_size);

    serialize_address(addr, address_size, (tx_buffer + 2));

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, (address_size + 3)));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, (data_size + 1)));

    *result = rx_buffer[0];

    if (data_size == UPDIX_DATA_SIZE_16)
    {
        *result |= ((uint16_t)rx_buffer[1] << 8);
    }

    return UPDIX_OK;
}


updix_status_t updix_sts(uint32_t addr, uint16_t data, updix_address_size_t address_size,
                         updix_data_size_t data_size)
{
    uint8_t tx_buffer[5] = {0x00};
    uint8_t rx_buffer[1] = {0x00};

    set_instr_header(UPDIX_STS, tx_buffer, address_size, data_size);

    serialize_address(addr, address_size, (tx_buffer + 2));

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, (address_size + 3)));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, 1));

    if (*rx_buffer != UPDIX_ACK)
    {
        return UPDIX_ERROR_ACK;
    }

    serialize_data(data_size, data, tx_buffer);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, (data_size + 1)));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, 1));

    if (*rx_buffer != UPDIX_ACK)
    {
        return UPDIX_ERROR_ACK;
    }

    return UPDIX_OK;
}
