#include <stdint.h>
#include "updix.h"
#include "updix_uart.h"


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


static const uint8_t updix_key_nvmprog[8] = {0x20, 0x67, 0x6F, 0x72,
                                             0x50, 0x4D, 0x56, 0x4E};

static const uint8_t updix_key_chiperase[8] = {0x65, 0x73, 0x61, 0x72,
                                               0x45, 0x4D, 0x56, 0x4E};

static const uint8_t updix_key_userrow[8] = {0x65, 0x74, 0x26, 0x73,
                                             0x55, 0x4D, 0x56, 0x4E};


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


updix_status_t updix_sts(uint32_t addr, updix_address_size_t address_size,
                         updix_data_size_t data_size, uint16_t data)
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

    serialize_data(data, data_size, tx_buffer);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, (data_size + 1)));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, 1));

    if (*rx_buffer != UPDIX_ACK)
    {
        return UPDIX_ERROR_ACK;
    }

    return UPDIX_OK;
}


updix_status_t updix_ld(updix_pointer_mode_t pointer_mode, uint8_t size_field,
                        uint32_t *result)
{
    uint8_t tx_buffer[3] = {0x00};
    uint8_t rx_buffer[3] = {0x00};

    set_instr_header(UPDIX_LD, tx_buffer, pointer_mode, size_field);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 2));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, (size_field + 1)));

    *result = 0;

    for (uint8_t i = 0; i < (size_field + 1); ++i)
    {
        *result |= ((uint32_t)rx_buffer[i] << (8 * i));
    }

    return UPDIX_OK;
}


updix_status_t updix_st(updix_pointer_mode_t pointer_mode, uint8_t size_field,
                        uint32_t data)
{
    uint8_t tx_buffer[5] = {0x00};
    uint8_t rx_buffer[1] = {0x00};

    set_instr_header(UPDIX_ST, tx_buffer, pointer_mode, size_field);

    for (uint8_t i = 0; i < (size_field + 1); ++i)
    {
        tx_buffer[i + 2] = (uint8_t)(data >> (8 * i));
    }

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, (size_field + 3)));
    UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, 1));

    if (*rx_buffer != UPDIX_ACK)
    {
        return UPDIX_ERROR_ACK;
    }

    return UPDIX_OK;
}


updix_status_t updix_repeat(uint8_t repeat_count)
{
    uint8_t tx_buffer[3] = {0x00};

    set_instr_header(UPDIX_REPEAT, tx_buffer, 0x00, 0x00);

    tx_buffer[2] = repeat_count;

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 3));

    return UPDIX_OK;
}


updix_status_t updix_read_block(uint32_t address, updix_address_size_t address_size,
                                updix_data_size_t data_size, uint8_t *buffer,
                                uint16_t element_count)
{
    // For 16 bit data, provide a buffer that is twice as long as the element count.

    if (element_count == 0 || element_count > 256)
    {
        return UPDIX_ERROR_INVALID_ARGUMENT;
    }

    uint8_t tx_buffer[2] = {0x00};

    UPDIX_CHECK_STATUS(updix_st(UPDIX_POINTER_MODE_REGISTER, address_size, address));

    UPDIX_CHECK_STATUS(updix_repeat(element_count - 1));

    set_instr_header(UPDIX_LD, tx_buffer, UPDIX_POINTER_MODE_POST_INCREMENT, data_size);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 2));

    UPDIX_CHECK_UART(updix_uart_rx(buffer, element_count * (data_size + 1)));

    return UPDIX_OK;
}


updix_status_t updix_write_block(uint32_t address, updix_address_size_t address_size,
                                 updix_data_size_t data_size, const uint8_t *buffer,
                                 uint16_t element_count)
{
    // For transmitting 16 bit data, provide the buffer in little endian form.

    if (element_count == 0 || element_count > 256)
    {
        return UPDIX_ERROR_INVALID_ARGUMENT;
    }

    uint8_t tx_buffer[2] = {0x00};
    uint8_t rx_buffer[1] = {0x00};

    UPDIX_CHECK_STATUS(updix_st(UPDIX_POINTER_MODE_REGISTER, address_size, address));

    UPDIX_CHECK_STATUS(updix_repeat(element_count - 1));

    set_instr_header(UPDIX_ST, tx_buffer, UPDIX_POINTER_MODE_POST_INCREMENT, data_size);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 2));

    for (uint16_t i = 0; i < element_count; ++i)
    {
        UPDIX_CHECK_UART(updix_uart_tx(buffer, data_size + 1));
        UPDIX_CHECK_UART(updix_uart_rx(rx_buffer, 1));

        if (*rx_buffer != UPDIX_ACK)
        {
            return UPDIX_ERROR_ACK;
        }
        buffer += (data_size + 1);
    }

    return UPDIX_OK;
}


updix_status_t updix_ldcs(updix_cs_register_t cs_register, uint8_t *result)
{
    uint8_t tx_buffer[2] = {0x00};

    set_instr_header(UPDIX_LDCS, tx_buffer, 0x00, cs_register);

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 2));
    UPDIX_CHECK_UART(updix_uart_rx(result, 1));

    return UPDIX_OK;
}


updix_status_t updix_stcs(updix_cs_register_t cs_register, uint8_t value)
{
    uint8_t tx_buffer[3] = {0x00};

    set_instr_header(UPDIX_STCS, tx_buffer, 0x00, cs_register);

    tx_buffer[2] = value;

    UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 3));

    return UPDIX_OK;
}


updix_status_t updix_key(updix_key_mode_t mode, updix_key_size_t key_size, void *buffer)
{
    /*
     * The buffer argument accepts a void pointer to a updix_activation_key_t enum
     * when the mode is UPDIX_KEY_MODE_SEND, and receives the read SIB value
     * as uint8_t when the mode is UPDIX_KEY_MODE_READ_SIB
     *
     * The UPDIX_KEY_SIZE_128 option is only valid for SIB reads, not sends
     */

    uint8_t tx_buffer[10] = {0x00};
    const uint8_t *key_ptr;

    set_instr_header(UPDIX_KEY, tx_buffer, mode, key_size);

    switch (mode)
    {
    case UPDIX_KEY_MODE_SEND:
        switch (*((updix_activation_key_t *)buffer))
        {
        case UPDIX_NVMPROG:
            key_ptr = updix_key_nvmprog;
            break;
        case UPDIX_CHIPERASE:
            key_ptr = updix_key_chiperase;
            break;
        case UPDIX_USERROW:
            key_ptr = updix_key_userrow;
            break;

        default:
            return UPDIX_ERROR_INVALID_ARGUMENT;
        }

        for (uint8_t i = 0; i < 8; ++i)
        {
            tx_buffer[i + 2] = key_ptr[i];
        }

        UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 10));
        break;

    case UPDIX_KEY_MODE_READ_SIB:
        UPDIX_CHECK_UART(updix_uart_tx(tx_buffer, 2));
        UPDIX_CHECK_UART(updix_uart_rx((uint8_t *)buffer, (key_size + 1) * 8));
        break;
    }

    return UPDIX_OK;
}
