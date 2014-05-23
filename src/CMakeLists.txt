set(
  src_lib
  packet.c
  struct_io.c
  struct_io.func.c
  )

add_library(tafl STATIC ${src_lib})
