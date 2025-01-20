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

add_executable(bh1750_test)
target_sources(bh1750_test PRIVATE bh1750_test.c ../source/bh1750.c)
target_include_directories(bh1750_test PRIVATE ../source/include)
target_compile_options(bh1750_test PRIVATE -g)


add_test(NAME sqm_le_test COMMAND sqm_le_test)
add_test(NAME weather_test COMMAND weather_test)
add_test(NAME bh1750_test COMMAND bh1750_test)
