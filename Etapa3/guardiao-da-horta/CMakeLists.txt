# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(guardiao-da-horta C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src build) #nescessário para o SDcard

# Add executable. Default name is the project name, version 0.1

add_executable(guardiao-da-horta     
    app/hw_config.c 
    app/main.c
    hal/wifi_conn.c
    hal/mqtt_comm.c
    hal/aht10.c
    hal/bmp280.c
    hal/bh1750.c
    hal/display.c
    hal/sdcard_manager.c
    hal/mqtt_manager.c
    drivers/ssd1306.c
    drivers/font.c


    )

# Can leave these off for silent mode:
# add_compile_definitions(USE_PRINTF USE_DBG_PRINTF)
add_compile_definitions(USE_PRINTF)



pico_set_program_name(guardiao-da-horta "guardiao-da-horta")
pico_set_program_version(guardiao-da-horta "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(guardiao-da-horta 0)
pico_enable_stdio_usb(guardiao-da-horta 1)

# Add the standard library to the build
target_link_libraries(guardiao-da-horta
        pico_stdlib
        hardware_i2c
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
        pico_lwip
        )

# Add the standard include files to the build
target_include_directories(guardiao-da-horta PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
)

# Add any user requested libraries
target_link_libraries(guardiao-da-horta 
        
        )

pico_add_extra_outputs(guardiao-da-horta)
