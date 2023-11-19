cmake_minimum_required(VERSION 3.14.0)
project(prng-testing)

find_program(BASH bash)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -pedantic)

set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/install)

enable_testing()

include_directories(
  ${INSTALL_DIR}/TestU01/include
  ${INSTALL_DIR}/random123/include
  ${INSTALL_DIR}/pcg-cpp/include)

function(add_generator name sources flags)

  list(APPEND sources main.cpp)
  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)

  add_executable(${name} ${sources})

  target_compile_options(${name} PUBLIC ${flags})

  target_link_libraries(${name}
    ${INSTALL_DIR}/TestU01/lib/libtestu01.a
    ${INSTALL_DIR}/TestU01/lib/libmylib.a
    ${INSTALL_DIR}/TestU01/lib/libprobdist.a)

  add_test(NAME ${name} COMMAND ${CMAKE_BINARY_DIR}/${name} test x 0 1)

endfunction()

function(add_hwd_test name sources flags)

  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)
  list(APPEND sources "thirdparty/hwd.c;thirdparty/prngs_hwd.c")

  list(APPEND flags "-DC_LINKAGE")

  add_executable(${name} ${sources})

  target_compile_options(${name} PUBLIC ${flags})

endfunction()

function(add_ze_test name sources flags)

  list(APPEND sources zero-escape.cpp)
  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)

  add_executable(${name} ${sources})

  target_compile_options(${name} PUBLIC ${flags})

endfunction()

# Generators
add_generator(zero_generator             zero_generator.cpp "")
add_generator(mersenne_twister           mersenne_twister.cpp "-DTEST_32BIT_OUTPUT")
add_generator(pcg32_fast_ref             pcg32_fast_ref.cpp "-DTEST_32BIT_OUTPUT")
add_generator(pcg64_ref                  pcg64_ref.cpp "")
add_generator(pcg64_fast_ref             pcg64_fast_ref.cpp "")
add_generator(philox4x32_ref             philox4x32_ref.cpp "")
add_generator(philox4x32_ref_matchoutput philox4x32_ref.cpp "-DMATCH_OUTPUT")
add_generator(philox2x64_ref             philox2x64_ref.cpp "")
add_generator(philox4x32                 philox.cpp "-DSUFFIX=4x32_10")
add_generator(philox4x32_resetkeys       philox.cpp "-DSUFFIX=4x32_10;-DRESET_KEYS")
add_generator(philox2x64                 philox.cpp "-DSUFFIX=2x64_10")
add_generator(xoroshiro128plus_55_14_36  xoroshiro128.cpp "-DSUFFIX=plus;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")
add_generator(xoroshiro128plus_24_16_37  xoroshiro128.cpp "-DSUFFIX=plus;-DSHIFT_A=24;-DSHIFT_B=16;-DSHIFT_C=37")
add_generator(xoroshiro128aox_55_14_36   xoroshiro128.cpp "-DSUFFIX=aox;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")
add_generator(xoroshiro128aox_24_16_37   xoroshiro128.cpp "-DSUFFIX=aox;-DSHIFT_A=24;-DSHIFT_B=16;-DSHIFT_C=37")

# HWD
add_hwd_test(hwd_mersenne_twister mersenne_twister.cpp "")
add_hwd_test(hwd_pcg64_ref pcg64_ref.cpp "")
add_hwd_test(hwd_philox4x32_ref philox4x32_ref.cpp "")
add_hwd_test(hwd_xoroshiro128plus_55_14_36 xoroshiro128.cpp "-DSUFFIX=plus;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")
add_hwd_test(hwd_xoroshiro128plus_24_16_37 xoroshiro128.cpp "-DSUFFIX=plus;-DSHIFT_A=24;-DSHIFT_B=16;-DSHIFT_C=37")
add_hwd_test(hwd_xoroshiro128aox_55_14_36 xoroshiro128.cpp "-DSUFFIX=aox;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")
add_hwd_test(hwd_xoroshiro128aox_24_16_37 xoroshiro128.cpp "-DSUFFIX=aox;-DSHIFT_A=24;-DSHIFT_B=16;-DSHIFT_C=37")

# Zero escape tests
add_ze_test(ze_mersenne_twister mersenne_twister.cpp "-DSTATE32_OUTPUT32")
add_ze_test(ze_pcg64_ref pcg64_ref.cpp "-DSTATE128_OUTPUT64")
add_ze_test(ze_philox4x32_ref philox4x32_ref.cpp "-DSTATE128_OUTPUT64")
add_ze_test(ze_xoroshiro128plus_55_14_36 xoroshiro128.cpp "-DSTATE128_OUTPUT64;-DSUFFIX=plus;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")
add_ze_test(ze_xoroshiro128aox_55_14_36 xoroshiro128.cpp "-DSTATE128_OUTPUT64;-DSUFFIX=aox;-DSHIFT_A=55;-DSHIFT_B=14;-DSHIFT_C=36")

# Add the Philox test
configure_file(test-philox.sh.in test-philox.sh)
add_test(test-philox ${BASH} ${CMAKE_CURRENT_BINARY_DIR}/test-philox.sh)
