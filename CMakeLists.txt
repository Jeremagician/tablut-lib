cmake_minimum_required(VERSION 2.8)

set(TAFL_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR})

if(TESTS)
  add_subdirectory(tests)
endif(TESTS)