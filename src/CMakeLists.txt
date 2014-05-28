set(
  src_lib
  packet.c
  struct_io.c
  struct_io.func.c
  )

add_library(tafl-lib STATIC ${src_lib})
set_target_properties(tafl-lib PROPERTIES OUTPUT_NAME tafl)