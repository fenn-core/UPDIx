#include <stddef.h>
#include <stdint.h>
#include "updix.h"

#define UPDIX_NVM_FLASH_BUSY_BIT 0x00
#define UPDIX_NVM_EEPROM_BUSY_BIT 0x01
#define UPDIX_NVM_WRITE_ERROR_BIT 0x02


typedef struct
{
    volatile uint8_t CTRLA;
    volatile uint8_t CTRLB;
    volatile uint8_t STATUS;
    volatile uint8_t INTCTRL;
    volatile uint8_t INTFLAGS;
    volatile uint8_t RESERVED0[1];
    volatile uint16_t DATA;
    volatile uint16_t ADDR;
} NVMCTRL_TypeDef;

extern uint32_t NVMCTRL_BASE;

#define NVMCTRL_REG_ADDR(reg) (NVMCTRL_BASE + offsetof(NVMCTRL_TypeDef, reg))

_Static_assert(offsetof(NVMCTRL_TypeDef, CTRLA) == 0, "CTRLA offset");
_Static_assert(offsetof(NVMCTRL_TypeDef, INTFLAGS) == 4, "INTFLAGS offset");
_Static_assert(offsetof(NVMCTRL_TypeDef, DATA) == 6, "DATA offset");
_Static_assert(offsetof(NVMCTRL_TypeDef, ADDR) == 8, "ADDR offset");
_Static_assert(sizeof(NVMCTRL_TypeDef) == 10, "Struct size");


typedef enum
{
    UPDIX_NVM_NOP = 0x00,
    UPDIX_NVM_WRITE_PAGE = 0x01,
    UPDIX_NVM_ERASE_PAGE = 0x02,
    UPDIX_NVM_ERASE_WRITE_PAGE = 0x03,
    UPDIX_NVM_PAGE_BUFFER_CLR = 0x04,
    UPDIX_NVM_CHIP_ERASE = 0x05,
    UPDIX_NVM_ERASE_EEPROM = 0x06,
    UPDIX_NVM_WRITE_FUSE = 0x07,
} updix_nvm_command_t;


static updix_status_t nvm_p0_check_status(uint16_t *buffer)
{
    UPDIX_CHECK_STATUS(updix_lds(NVMCTRL_REG_ADDR(STATUS), UPDIX_ADDRESS_SIZE_16,
                                 UPDIX_DATA_SIZE_8, buffer));

    if (UPDIX_GET_BIT(*buffer, UPDIX_NVM_WRITE_ERROR_BIT))
    {
        return UPDIX_ERROR_NVM;
    }

    return UPDIX_OK;
}


updix_status_t nvm_p0_wait_ready(void)
{
    uint16_t buffer[1];

    for (uint16_t i = 0; i < UPDIX_POLL_MAX_TRIES; ++i)
    {
        UPDIX_CHECK_STATUS(nvm_p0_check_status(buffer));

        if (!(UPDIX_GET_BIT(*buffer, UPDIX_NVM_FLASH_BUSY_BIT) |
              UPDIX_GET_BIT(*buffer, UPDIX_NVM_EEPROM_BUSY_BIT)))
        {
            return UPDIX_OK;
        }
    }

    return UPDIX_ERROR_TIMEOUT;
}


updix_status_t nvm_p0_execute_command(updix_nvm_command_t command)
{
    UPDIX_CHECK_STATUS(updix_sts(NVMCTRL_REG_ADDR(CTRLA), UPDIX_ADDRESS_SIZE_16,
                                 UPDIX_DATA_SIZE_8, command));

    return UPDIX_OK;
}
