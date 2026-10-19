#include <stdint.h>
#include "updix.h"


updix_status_t updix_verify_memory(uint32_t addr, updix_address_size_t address_size,
                                   const uint8_t *expected, uint32_t size)
{
    uint8_t rx_buffer[128] = {0x00};

    uint32_t remaining = size;

    while (remaining > 0)
    {
        uint8_t chunk_size =
            remaining < sizeof(rx_buffer) ? remaining : sizeof(rx_buffer);

        UPDIX_CHECK_STATUS(updix_read_block(addr, address_size, UPDIX_DATA_SIZE_8,
                                            rx_buffer, chunk_size));

        for (int i = 0; i < chunk_size; ++i)
        {
            if (rx_buffer[i] != expected[(size - remaining) + i])
            {
                return UPDIX_ERROR_VERIFY;
            }
        }
        addr += chunk_size;
        remaining -= chunk_size;
    }

    return UPDIX_OK;
}