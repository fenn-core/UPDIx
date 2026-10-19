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


updix_status_t nvm_p0_chip_erase(void)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_CHIP_ERASE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_write_flash_page(uint32_t addr, updix_address_size_t address_size,
                                       const uint8_t *data, uint16_t size)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_PAGE_BUFFER_CLR));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(
        updix_write_block(addr, address_size, UPDIX_DATA_SIZE_16, data, (size / 2)));

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_WRITE_PAGE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_erase_flash_page(uint32_t addr, updix_address_size_t address_size)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(updix_sts(addr, address_size, UPDIX_DATA_SIZE_8, 0xFF));

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_ERASE_PAGE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_erase_write_flash_page(uint32_t addr,
                                             updix_address_size_t address_size,
                                             const uint8_t *data, uint16_t size)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_PAGE_BUFFER_CLR));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(
        updix_write_block(addr, address_size, UPDIX_DATA_SIZE_16, data, (size / 2)));

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_ERASE_WRITE_PAGE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_write_eeprom(uint32_t addr, updix_address_size_t address_size,
                                   const uint8_t *data, uint16_t size)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_PAGE_BUFFER_CLR));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(
        updix_write_block(addr, address_size, UPDIX_DATA_SIZE_8, data, size));

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_ERASE_WRITE_PAGE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_erase_eeprom(void)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_ERASE_EEPROM));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}


updix_status_t nvm_p0_write_fuse(uint16_t fuse_address, uint8_t value)
{
    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    UPDIX_CHECK_STATUS(updix_sts(NVMCTRL_REG_ADDR(ADDR), UPDIX_ADDRESS_SIZE_16,
                                 UPDIX_DATA_SIZE_16, fuse_address));

    UPDIX_CHECK_STATUS(updix_sts(NVMCTRL_REG_ADDR(DATA), UPDIX_ADDRESS_SIZE_16,
                                 UPDIX_DATA_SIZE_8, value));

    UPDIX_CHECK_STATUS(nvm_p0_execute_command(UPDIX_NVM_WRITE_FUSE));

    UPDIX_CHECK_STATUS(nvm_p0_wait_ready());

    return UPDIX_OK;
}
