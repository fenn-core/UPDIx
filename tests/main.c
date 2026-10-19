#include "test.h"
#include "updix.h"


int main(void)
{
    const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

    updix_write_block(0x10, UPDIX_ADDRESS_SIZE_8,
        UPDIX_DATA_SIZE_8, buffer, 6);

    updix_uart_debug_print();

    return 0;
}