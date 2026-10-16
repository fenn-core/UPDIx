#ifndef INC_UPDIX_UPDIX_H_
#define INC_UPDIX_UPDIX_H_

#define UPDIX_SYNCH 0x55
#define UPDIX_ACK 0x40

#define UPDIX_CHECK_STATUS(code)                                                         \
    do                                                                                   \
    {                                                                                    \
        updix_status_t status = (code);                                                  \
        if (status != UPDIX_OK)                                                          \
        {                                                                                \
            return status;                                                               \
        }                                                                                \
    } while (0)

typedef enum
{
    UPDIX_OK = 0,
    UPDIX_ERROR_INVALID_ARGUMENT,
    UPDIX_ERROR_TRANSPORT,
    UPDIX_ERROR_TIMEOUT,
    UPDIX_ERROR_ACK,
    UPDIX_ERROR_PROTOCOL,
} updix_status_t;


typedef enum
{
    UPDIX_ADDRESS_SIZE_8 = 0x00,
    UPDIX_ADDRESS_SIZE_16 = 0x01,
    UPDIX_ADDRESS_SIZE_24 = 0x02,
} updix_address_size_t;


typedef enum
{
    UPDIX_DATA_SIZE_8 = 0x00,
    UPDIX_DATA_SIZE_16 = 0x01,
} updix_data_size_t;


typedef enum
{
    UPDIX_POINTER_MODE_INDIRECT = 0x00,
    UPDIX_POINTER_MODE_POST_INCREMENT = 0x01,
    UPDIX_POINTER_MODE_REGISTER = 0x02,
} updix_pointer_mode_t;


typedef enum
{
    UPDIX_KEY_MODE_SEND = 0x00,
    UPDIX_KEY_MODE_READ_SIB = 0x01,
} updix_key_mode_t;


typedef enum
{
    UPDIX_KEY_SIZE_64 = 0x00,
    UPDIX_KEY_SIZE_128 = 0x01,
} updix_key_size_t;


typedef enum
{
    UPDIX_NVMPROG = 0x00,
    UPDIX_CHIPERASE = 0x01,
    UPDIX_USERROW = 0x02,
} updix_activation_key_t;


typedef enum
{
    UPDIX_CS_STATUSA = 0x00,
    UPDIX_CS_STATUSB = 0x01,
    UPDIX_CS_CTRLA = 0x02,
    UPDIX_CS_CTRLB = 0x03,
    UPDIX_CS_ASI_KEY_STATUS = 0x07,
    UPDIX_CS_ASI_RESET_REQ = 0x08,
    UPDIX_ASI_CTRLA = 0x09,
    UPDIX_ASI_SYS_CTRLA = 0x0A,
    UPDIX_ASI_SYS_STATUS = 0x0B,
    UPDIX_CS_ASI_SYS_STATUS = 0x0C,
} updix_cs_register_t;


#endif
