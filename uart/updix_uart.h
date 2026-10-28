#ifndef INC_UPDIX_UART_H_
#define INC_UPDIX_UART_H_

#include <stddef.h>
#include <stdint.h>

#define UPDIX_CHECK_UART(code)                                                           \
    do                                                                                   \
    {                                                                                    \
        updix_uart_status_t status = (code);                                             \
        if (status == UPDIX_UART_ERROR_TIMEOUT)                                          \
        {                                                                                \
            return UPDIX_ERROR_TIMEOUT;                                                  \
        }                                                                                \
        else if (status != UPDIX_UART_OK)                                                \
        {                                                                                \
            return UPDIX_ERROR_TRANSPORT;                                                \
        }                                                                                \
    } while (0)


typedef enum
{
    UPDIX_UART_OK = 0,
    UPDIX_UART_ERROR_TIMEOUT,
    UPDIX_UART_ERROR_FRAMING,
    UPDIX_UART_ERROR_PARITY,
    UPDIX_UART_ERROR_OVERRUN,
    UPDIX_UART_ERROR_IO,
} updix_uart_status_t;


updix_uart_status_t updix_uart_tx(const uint8_t *data, size_t len);


updix_uart_status_t updix_uart_rx(uint8_t *data, size_t len);


void updix_stm32hal_bind(void *uart_handle);


#endif
