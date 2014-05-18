set(SRC "${PROJECT_SOURCE_DIR}")
add_definitions(-DTEST)

add_executable(test_struct_io test_struct_io.c ${SRC}/struct_io.c)
add_test(test_struct_io test_struct_io)

add_executable(test_packet test_packet.c ${SRC}/packet.c ${SRC}/struct_io.c)
add_test(test_packet test_packet)
