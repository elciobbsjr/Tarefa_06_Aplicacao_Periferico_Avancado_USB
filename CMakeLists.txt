# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(Tarefa_06_Aplicacao_Periferico_Avancado_USB C CXX ASM)

pico_sdk_init()

add_executable(Tarefa_06_Aplicacao_Periferico_Avancado_USB 
    Tarefa_06_Aplicacao_Periferico_Avancado_USB.c
    ssd1306_i2c.c
)

pico_set_program_name(Tarefa_06_Aplicacao_Periferico_Avancado_USB "Tarefa_06_Aplicacao_Periferico_Avancado_USB")
pico_set_program_version(Tarefa_06_Aplicacao_Periferico_Avancado_USB "0.1")

pico_enable_stdio_uart(Tarefa_06_Aplicacao_Periferico_Avancado_USB 0)
pico_enable_stdio_usb(Tarefa_06_Aplicacao_Periferico_Avancado_USB 1)

target_link_libraries(Tarefa_06_Aplicacao_Periferico_Avancado_USB
    pico_stdlib
    hardware_i2c
)

target_include_directories(Tarefa_06_Aplicacao_Periferico_Avancado_USB PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Tarefa_06_Aplicacao_Periferico_Avancado_USB)
