cmake_minimum_required(VERSION 3.14.0)
project(prng-testing)

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
  message("sources ${sources}")

  add_executable(${name} ${sources})

  target_compile_options(${name} PUBLIC ${flags})

  target_link_libraries(${name}
    ${INSTALL_DIR}/TestU01/lib/libtestu01.a
    ${INSTALL_DIR}/TestU01/lib/libmylib.a
    ${INSTALL_DIR}/TestU01/lib/libprobdist.a)

  add_test(${CMAKE_BINARY_DIR}/${name} test 1 0)

endfunction()

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
