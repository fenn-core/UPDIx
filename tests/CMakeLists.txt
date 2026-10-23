cmake_minimum_required(VERSION 3.20)

project(UPDIx_test C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT_DIR)

add_executable(UPDIx_test
    main.c
    updix_uart_test.c
    test_instructions.c
    ${PARENT_DIR}/src/updix_memory.c
    ${PARENT_DIR}/src/updix_protocol.c
    ${PARENT_DIR}/src/updix_session.c
    ${PARENT_DIR}/src/nvm/nvm_p0.c
)

target_include_directories(UPDIx_test
    PRIVATE
        ${PARENT_DIR}/
        ${PARENT_DIR}/src
        ${PARENT_DIR}/uart
        ${PARENT_DIR}/include
)