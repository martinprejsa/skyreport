cmake_minimum_required(VERSION 3.21)
project(skyreport LANGUAGES C)

add_executable(skyreport)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

set(CMAKE_INSTALL_RPATH $ORIGIN)

add_subdirectory(3rdparty)

target_compile_options(skyreport PUBLIC -Wall -D_XOPEN_SOURCE -pedantic -fvisibility=hidden)

target_sources(skyreport PRIVATE source/main.c source/sqm_le.c source/wh2600.c source/bh1750.c)
target_include_directories(skyreport PRIVATE source/include)
target_link_libraries(skyreport PRIVATE iniparser-static)

if (BUILD_TESTS) 
    enable_testing()
    add_subdirectory(tests)
endif()
