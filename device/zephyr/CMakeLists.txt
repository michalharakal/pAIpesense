cmake_minimum_required(VERSION 3.20.0)

set(BOARD arduino_nano_33_ble)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(paipesense_device)

target_sources(app PRIVATE src/main.c)
target_include_directories(app PRIVATE include)
