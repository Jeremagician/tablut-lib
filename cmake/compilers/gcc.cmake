if(DEBUG)
  add_definitions("-DDEBUG")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -W -Wall -Wextra -Winit-self -Wfatal-errors -pedantic -Werror")
  message(STATUS "GCC: Enabled debug")
else(DEBUG)
  # Explicitely disable debug mode to remove assertions
  add_definitions("-DNDEBUG")
endif(DEBUG)
