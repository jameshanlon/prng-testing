cmake_minimum_required(VERSION 3.16.0)
project(prng-testing)

find_package(Boost 1.68.0 COMPONENTS
             unit_test_framework)

find_program(BASH bash)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -pedantic)

set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/install)

include_directories(${INSTALL_DIR}/TestU01/include
                    ${INSTALL_DIR}/random123/include
                    ${INSTALL_DIR}/pcg-cpp/include)

enable_testing()

# add_generator
function(add_generator name sources defines)

  list(APPEND sources main.cpp jump.c)
  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)

  add_executable(${name} ${sources})

  target_compile_definitions(${name} PUBLIC ${defines})

  target_link_libraries(${name}
    ${INSTALL_DIR}/TestU01/lib/libtestu01.a
    ${INSTALL_DIR}/TestU01/lib/libmylib.a
    ${INSTALL_DIR}/TestU01/lib/libprobdist.a)

  add_test(NAME ${name} COMMAND ${CMAKE_BINARY_DIR}/${name} test x 1 -1)

	install(TARGETS ${target_name})

endfunction()

# add_hwd_test
function(add_hwd_test name sources defines)

  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)
  list(PREPEND sources "thirdparty/hwd.c;src/prngs_hwd.c")

  add_executable(${name} ${sources})

  target_include_directories(${name} PUBLIC ${CMAKE_SOURCE_DIR}/src)
  target_compile_definitions(${name} PUBLIC ${defines})

	install(TARGETS ${target_name})

endfunction()

# add_ze_test
function(add_ze_test name sources defines)

  list(APPEND sources zero-escape.cpp)
  list(TRANSFORM sources PREPEND ${CMAKE_SOURCE_DIR}/src/)

  add_executable(${name} ${sources})

  target_compile_definitions(${name} PUBLIC ${defines})

	install(TARGETS ${target_name})

endfunction()

add_generator(zero_generator                         zero_generator.cpp          "")
add_generator(mersenne_twister                       mersenne_twister.cpp        "TEST_32BIT_OUTPUT")
add_generator(pcg32_fast_ref                         pcg32_fast_ref.cpp          "TEST_32BIT_OUTPUT")
add_generator(pcg64_ref                              pcg64_ref.cpp               "")
add_generator(pcg64_fast_ref                         pcg64_fast_ref.cpp          "")
add_generator(philox4x32_ref                         philox4x32_ref.cpp          "")
add_generator(philox4x32_ref_matchoutput             philox4x32_ref.cpp          "MATCH_OUTPUT")
add_generator(philox2x64_ref                         philox2x64_ref.cpp          "")
add_generator(philox4x32                             philox.cpp                  "SUFFIX=4x32_10")
add_generator(philox4x32_resetkeys                   philox.cpp                  "SUFFIX=4x32_10;RESET_KEYS")
add_generator(philox2x64                             philox.cpp                  "SUFFIX=2x64_10")
add_generator(xoroshiro128plus_55_14_36              xoroshiro128.cpp            "SUFFIX=plus;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128plus_24_16_37              xoroshiro128.cpp            "SUFFIX=plus;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")
add_generator(xoroshiro128aox_55_14_36               xoroshiro128.cpp            "SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_55_14_36_x2            xoroshiro128.cpp            "SUFFIX=aox;LFSR_ITERATIONS=2;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_24_16_37               xoroshiro128.cpp            "SUFFIX=aox;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")
add_generator(xoroshiro128aox_rule30x3               xoroshiro128.cpp            "SUFFIX=aox_rule30x3;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_shuffle                xoroshiro128.cpp            "SUFFIX=aox_shuffle;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_shuffle2               xoroshiro128.cpp            "SUFFIX=aox_shuffle2;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_shuffle3               xoroshiro128.cpp            "SUFFIX=aox_shuffle3;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_jump_1              xoroshiro128_interleave.cpp "SEED_JUMP;NUM_GENERATORS=1;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_jump_2              xoroshiro128_interleave.cpp "SEED_JUMP;NUM_GENERATORS=2;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_jump_10             xoroshiro128_interleave.cpp "SEED_JUMP;NUM_GENERATORS=10;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_jump_100            xoroshiro128_interleave.cpp "SEED_JUMP;NUM_GENERATORS=100;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_jump_1000           xoroshiro128_interleave.cpp "SEED_JUMP;NUM_GENERATORS=1000;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_mt64_10             xoroshiro128_interleave.cpp "SEED_MT64;NUM_GENERATORS=10;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_mt64_100            xoroshiro128_interleave.cpp "SEED_MT64;NUM_GENERATORS=100;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_mt64_1000           xoroshiro128_interleave.cpp "SEED_MT64;NUM_GENERATORS=1000;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_pcg64_10            xoroshiro128_interleave.cpp "SEED_PCG64;NUM_GENERATORS=10;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_pcg64_100           xoroshiro128_interleave.cpp "SEED_PCG64;NUM_GENERATORS=100;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_pcg64_1000          xoroshiro128_interleave.cpp "SEED_PCG64;NUM_GENERATORS=1000;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_2_jump_short_128mb  xoroshiro128_interleave.cpp "SEED_JUMP;JUMP_SHORT_128MB;NUM_GENERATORS=2;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_2_jump_short_256mb  xoroshiro128_interleave.cpp "SEED_JUMP;JUMP_SHORT_256MB;NUM_GENERATORS=2;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_il_2_jump_short_1024mb xoroshiro128_interleave.cpp "SEED_JUMP;JUMP_SHORT_1024MB;NUM_GENERATORS=2;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128plus_shift                 xoroshiro128_shift.cpp      "SUFFIX=plus;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(xoroshiro128aox_shift                  xoroshiro128_shift.cpp      "SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_generator(counter_xa                             counter_xoroshiro128.cpp    "SUFFIX=xa")
add_generator(counter_2xa                            counter_xoroshiro128.cpp    "SUFFIX=2xa")
add_generator(counter_3xa                            counter_xoroshiro128.cpp    "SUFFIX=3xa")
add_generator(counter_5xa                            counter_xoroshiro128.cpp    "SUFFIX=5xa")
add_generator(counter_x                              counter_xoroshiro128.cpp    "SUFFIX=x")
add_generator(counter_2x                             counter_xoroshiro128.cpp    "SUFFIX=2x")
add_generator(counter_3x                             counter_xoroshiro128.cpp    "SUFFIX=3x")
add_generator(counter_5x                             counter_xoroshiro128.cpp    "SUFFIX=5x")
add_generator(counter_12x                            counter_xoroshiro128.cpp    "SUFFIX=12x")
add_generator(counter_gray_12x                       counter_xoroshiro128.cpp    "SUFFIX=gray_12x")
add_generator(counter_xa_il_1                        counter_xa_interleave.cpp   "SUFFIX=xa;SEED_PCG64;NUM_GENERATORS=1")
add_generator(counter_xa_il_2                        counter_xa_interleave.cpp   "SUFFIX=xa;SEED_PCG64;NUM_GENERATORS=2")
add_generator(counter_xa_il_10                       counter_xa_interleave.cpp   "SUFFIX=xa;SEED_PCG64;NUM_GENERATORS=10")
add_generator(counter_xa_il_100                      counter_xa_interleave.cpp   "SUFFIX=xa;SEED_PCG64;NUM_GENERATORS=100")
add_generator(counter_xa_il_1000                     counter_xa_interleave.cpp   "SUFFIX=xa;SEED_PCG64;NUM_GENERATORS=1000")

# Various equidistant seeding scheme-based generators
foreach (VARIANT a b c d e)
  foreach (NUM 2 3 4 5 6 7 8 9 10 100 1000)
    string(TOUPPER "SEED_EQUI${VARIANT}" SEED_NAME)
    add_generator(xoroshiro128aox_il_equi${VARIANT}_${NUM}
                  xoroshiro128_interleave.cpp
                  "${SEED_NAME};NUM_GENERATORS=${NUM};SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
  endforeach()
endforeach()

# 64-bit HWD
add_hwd_test(hwd_mersenne_twister          mersenne_twister.cpp "C_LINKAGE;HWD_BITS=64")
add_hwd_test(hwd_pcg64_ref                 pcg64_ref.cpp        "C_LINKAGE;HWD_BITS=64")
add_hwd_test(hwd_philox4x32_ref            philox4x32_ref.cpp   "C_LINKAGE;HWD_BITS=64")
add_hwd_test(hwd_xoroshiro128plus_55_14_36 xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=64;SUFFIX=plus;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_hwd_test(hwd_xoroshiro128plus_24_16_37 xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=64;SUFFIX=plus;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")
add_hwd_test(hwd_xoroshiro128aox_55_14_36  xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=64;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_hwd_test(hwd_xoroshiro128aox_24_16_37  xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=64;SUFFIX=aox;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")

# 32-bit HWD
add_hwd_test(hwd_64_mersenne_twister          mersenne_twister.cpp "C_LINKAGE;HWD_BITS=32")
add_hwd_test(hwd_64_pcg64_ref                 pcg64_ref.cpp        "C_LINKAGE;HWD_BITS=32")
add_hwd_test(hwd_64_philox4x32_ref            philox4x32_ref.cpp   "C_LINKAGE;HWD_BITS=32")
add_hwd_test(hwd_64_xoroshiro128plus_55_14_36 xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=32;SUFFIX=plus;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_hwd_test(hwd_64_xoroshiro128plus_24_16_37 xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=32;SUFFIX=plus;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")
add_hwd_test(hwd_64_xoroshiro128aox_55_14_36  xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=32;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_hwd_test(hwd_64_xoroshiro128aox_24_16_37  xoroshiro128.cpp     "C_LINKAGE;HWD_BITS=32;SUFFIX=aox;SHIFT_A=24;SHIFT_B=16;SHIFT_C=37")

# Zero escape
add_ze_test(ze_mersenne_twister          mersenne_twister.cpp     "STATE32_OUTPUT32")
add_ze_test(ze_pcg64_ref                 pcg64_ref.cpp            "STATE128_OUTPUT64")
add_ze_test(ze_philox4x32_ref            philox4x32_ref.cpp       "STATE128_OUTPUT64")
add_ze_test(ze_xoroshiro128plus_55_14_36 xoroshiro128.cpp         "STATE128_OUTPUT64;SUFFIX=plus;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_ze_test(ze_xoroshiro128aox_55_14_36  xoroshiro128.cpp         "STATE128_OUTPUT64;SUFFIX=aox;SHIFT_A=55;SHIFT_B=14;SHIFT_C=36")
add_ze_test(ze_counter_x                 counter_xoroshiro128.cpp "STATE128_OUTPUT64;SUFFIX=x")
add_ze_test(ze_counter_xa                counter_xoroshiro128.cpp "STATE128_OUTPUT64;SUFFIX=xa")
add_ze_test(ze_counter_12x               counter_xoroshiro128.cpp "STATE128_OUTPUT64;SUFFIX=12x")
add_ze_test(ze_counter_gray_12x          counter_xoroshiro128.cpp "STATE128_OUTPUT64;SUFFIX=gray_12x")

# Uniformity (Chi Squared)
add_executable(aox_uniformity src/aox_uniformity.cpp)

# Uniformity (TestU01)
add_executable(aox_uniformity_testu01 src/aox_uniformity_testu01.cpp)
target_link_libraries(aox_uniformity_testu01
                      ${CMAKE_SOURCE_DIR}/install/TestU01/lib/libtestu01.a
                      ${CMAKE_SOURCE_DIR}/install/TestU01/lib/libmylib.a
                      ${CMAKE_SOURCE_DIR}/install/TestU01/lib/libprobdist.a)

add_subdirectory(tests)
