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
