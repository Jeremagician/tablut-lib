add_executable(test_struct_io test_struct_io.c)
target_link_libraries(test_struct_io tafl)
add_test(test_struct_io test_struct_io)

add_executable(test_packet test_packet.c )
target_link_libraries(test_packet tafl)
add_test(test_packet test_packet)
