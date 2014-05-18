add_definitions(-DTEST)
include_directories(${TAFL_LIB_ROOT})

add_executable(test_struct_io test_struct_io.c ${TAFL_LIB_ROOT}/struct_io.c)
add_test(test_struct_io test_struct_io)

add_executable(test_packet test_packet.c ${TAFL_LIB_ROOT}/packet.c ${TAFL_LIB_ROOT}/struct_io.c)
add_test(test_packet test_packet)
