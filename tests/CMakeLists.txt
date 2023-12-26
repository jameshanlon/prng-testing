# xoroshiro128_interleave_2
add_executable(xoroshiro128_interleave_2_test
               main.cpp
               ../thirdparty/jump.c
               xoroshiro128_interleave.cpp)

target_link_libraries(xoroshiro128_interleave_2_test
                      ${Boost_LIBRARIES})

target_compile_definitions(xoroshiro128_interleave_2_test PUBLIC
                           "NUM_GENERATORS=2")

target_include_directories(xoroshiro128_interleave_2_test PUBLIC
                           ${CMAKE_SOURCE_DIR}/src
                           ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME xoroshiro128_interleave_2_test COMMAND ./xoroshiro128_interleave_2_test)

# xoroshiro128_interleave_1000
add_executable(xoroshiro128_interleave_1000_test
               main.cpp
               ../thirdparty/jump.c
               xoroshiro128_interleave.cpp)

target_link_libraries(xoroshiro128_interleave_1000_test
                      ${Boost_LIBRARIES})

target_compile_definitions(xoroshiro128_interleave_1000_test PUBLIC
                           "NUM_GENERATORS=1000")

target_include_directories(xoroshiro128_interleave_1000_test PUBLIC
                           ${CMAKE_SOURCE_DIR}/src
                           ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME xoroshiro128_interleave_1000_test COMMAND ./xoroshiro128_interleave_1000_test)

# Philox implementation test
configure_file(test_philox_impl.sh.in
               test_philox_impl.sh)
add_test(NAME test_philox_impl COMMAND bash test_philox_impl.sh)
