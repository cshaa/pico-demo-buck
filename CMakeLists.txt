option(PICO_W "Whether to compile for Pico W, rather than Pico" OFF)

if(PICO_W)
  set(PICO_BOARD pico_w)
  add_definitions(-DPICO_W)
else()
  set(PICO_BOARD pico)
endif()

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pico_example_01)
pico_sdk_init()


add_executable(main main.c)
target_link_libraries(main pico_stdlib pico_float hardware_pwm hardware_vreg hardware_adc)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)
