#include <stdint.h>
#include "updix.h"

#define UPDIX_LOCKSTATUS_BIT 0x00
#define UPDIX_PROGMODE_BIT 0x03
#define UPDIX_NVMPROG_KEY_BIT 0x04

#define UPDIX_RELEASE_SYS_RESET 0x00
#define UPDIX_DISABLE_UPDI 0x0C  // disables both UPDI and collision detection 
#define UPDIX_ASSERT_SYS_RESET 0x59


updix_status_t updix_enter_progmode(void)
{
    uint8_t buffer[1] = {0x00};
    updix_activation_key_t act_key_nvmprog = UPDIX_NVMPROG;

    UPDIX_CHECK_STATUS(updix_ldcs(UPDIX_CS_ASI_SYS_STATUS, buffer));

    if (UPDIX_GET_BIT(*buffer, UPDIX_PROGMODE_BIT))
    {
        return UPDIX_OK; // return if already in prog mode
    }
    if (UPDIX_GET_BIT(*buffer, UPDIX_LOCKSTATUS_BIT))
    {
        return UPDIX_ERROR_TARGET_LOCKED;
    }

    UPDIX_CHECK_STATUS(
        updix_key(UPDIX_KEY_MODE_SEND, UPDIX_KEY_SIZE_64, &act_key_nvmprog));

    UPDIX_CHECK_STATUS(updix_ldcs(UPDIX_CS_ASI_KEY_STATUS, buffer));

    if (!UPDIX_GET_BIT(*buffer, UPDIX_NVMPROG_KEY_BIT))
    {
        return UPDIX_ERROR_PROTOCOL;
    }

    UPDIX_CHECK_STATUS(updix_stcs(UPDIX_CS_ASI_RESET_REQ, UPDIX_ASSERT_SYS_RESET));

    UPDIX_CHECK_STATUS(updix_stcs(UPDIX_CS_ASI_RESET_REQ, UPDIX_RELEASE_SYS_RESET));

    for (uint16_t i = 0; i < UPDIX_POLL_MAX_TRIES; ++i)
    {
        UPDIX_CHECK_STATUS(updix_ldcs(UPDIX_CS_ASI_SYS_STATUS, buffer));
        if (UPDIX_GET_BIT(*buffer, UPDIX_PROGMODE_BIT))
        {
            return UPDIX_OK;
        }
    }

    return UPDIX_ERROR_TIMEOUT;
}


updix_status_t updix_exit_progmode(void)
{
    UPDIX_CHECK_STATUS(updix_stcs(UPDIX_CS_ASI_RESET_REQ, UPDIX_ASSERT_SYS_RESET));

    UPDIX_CHECK_STATUS(updix_stcs(UPDIX_CS_ASI_RESET_REQ, UPDIX_RELEASE_SYS_RESET));

    UPDIX_CHECK_STATUS(updix_stcs(UPDIX_CS_CTRLB, UPDIX_DISABLE_UPDI));

    return UPDIX_OK;
}
