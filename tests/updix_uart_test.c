#include <stdio.h>
#include <string.h>
#include "updix_uart.h"

#define UPDIX_TEST_BUFFER_SIZE 1024

static uint8_t tx_buffer[UPDIX_TEST_BUFFER_SIZE] = {0x00};
static uint8_t rx_buffer[UPDIX_TEST_BUFFER_SIZE];

static char pretty_print[UPDIX_TEST_BUFFER_SIZE * 6];
static size_t pretty_print_pos = 0;
/*
 * There are 3 chars used per byte ex: "FF\n"
 * the worst case: single byte per transfer is assumed here.
 * So transmission headers are added per each transfer,
 * Which also equate to 3 chars: "TX\n", "RX\n"
 */


updix_uart_status_t updix_uart_tx(const uint8_t *data, size_t len)
{
    strcpy((pretty_print + pretty_print_pos), "TX\n");
    pretty_print_pos += 3;
    for (size_t i = 0; i < len; ++i)
    {
        tx_buffer[i] = data[i];
        int written = snprintf(pretty_print + pretty_print_pos,
                               sizeof(pretty_print) - pretty_print_pos, "%02X\n", data[i]);
        pretty_print_pos += written;
    }

    return UPDIX_UART_OK;
}


updix_uart_status_t updix_uart_rx(uint8_t *data, size_t len)
{
    memset(rx_buffer, 0x40, sizeof(rx_buffer));
    // simulate ACK bytes

    strcpy((pretty_print + pretty_print_pos), "RX\n");
    pretty_print_pos += 3;
    for (size_t i = 0; i < len; ++i)
    {
        data[i] = rx_buffer[i];
        int written = snprintf(pretty_print + pretty_print_pos,
                               sizeof(pretty_print) - pretty_print_pos, "%02X\n", data[i]);
        pretty_print_pos += written;
    }

    return UPDIX_UART_OK;
}


void updix_uart_debug_print(void)
{
    printf("%s\n", pretty_print);
}