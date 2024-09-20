include(CTest)

enable_testing()

add_executable(sqm_le_test)
target_sources(sqm_le_test PRIVATE sqm_le_test.c ../source/sqm_le.c)
target_include_directories(sqm_le_test PRIVATE ../source/include)
target_compile_options(sqm_le_test PRIVATE -g)

add_executable(weather_test)
target_sources(weather_test PRIVATE weather_test.c ../source/wh2600.c)
target_include_directories(weather_test PRIVATE ../source/include)
target_compile_options(weather_test PRIVATE -g)

add_test(NAME sqm_le_test COMMAND sqm_le_test)
add_test(NAME weather_test COMMAND weather_test)