cmake_minimum_required(VERSION 2.8)
project(tafl-lib)

set(
  src_lib
  packet.c packet.h packet.def.h
  struct_io.c struct_io.h struct_io.def.h struct_io.func.h
)

add_library(tafl-lib STATIC ${src_lib})

set_target_properties(tafl-lib PROPERTIES OUTPUT_NAME tafl)