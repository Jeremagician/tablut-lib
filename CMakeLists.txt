cmake_minimum_required(VERSION 2.8)
project(tafl-lib)

include_directories(.)
set(
  src_lib
  packet.c packet.h packet.def.h
  struct_io.c struct_io.h struct_io.def.h struct_io.func.h
)

enable_testing()
add_subdirectory(tests)
