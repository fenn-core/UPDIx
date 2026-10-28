#include <stddef.h>
#include <stdint.h>
#include "updix_uart.h"

#define UPDIX_UART_TIMEOUT_MS 100U

#define UPDIX_CHECK_HAL_UART(code)                                                       \
    do                                                                                   \
    {                                                                                    \
        HAL_StatusTypeDef status = (code);                                               \
        if (status == HAL_TIMEOUT)                                                       \
        {                                                                                \
            return UPDIX_UART_ERROR_TIMEOUT;                                             \
        }                                                                                \
        else if (status != HAL_OK)                                                       \
        {                                                                                \
            return UPDIX_UART_ERROR_FRAMING;                                             \
        }                                                                                \
    } while (0)


/*
 * Define HAL dependencies to avoid including a mcu specific header
 * The linker is expected to link from the main CubeMX project
 */

typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

typedef enum
{
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U,
} HAL_StatusTypeDef;


extern HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *, const uint8_t *,
                                           uint16_t, uint32_t);

extern HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *, uint8_t *, uint16_t,
                                          uint32_t);

extern HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *);


extern HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *);


static UART_HandleTypeDef *huart;


void updix_stm32hal_bind(void *uart_handle)
{
    huart = ((UART_HandleTypeDef *)uart_handle);
}


updix_uart_status_t updix_uart_tx(const uint8_t *data, size_t len)
{
    UPDIX_CHECK_HAL_UART(HAL_HalfDuplex_EnableTransmitter(huart));

    UPDIX_CHECK_HAL_UART(HAL_UART_Transmit(huart, data, len, UPDIX_UART_TIMEOUT_MS));

    return UPDIX_UART_OK;
}


updix_uart_status_t updix_uart_rx(uint8_t *data, size_t len)
{
    UPDIX_CHECK_HAL_UART(HAL_HalfDuplex_EnableReceiver(huart));

    UPDIX_CHECK_HAL_UART(HAL_UART_Receive(huart, data, len, UPDIX_UART_TIMEOUT_MS));

    return UPDIX_UART_OK;
}
