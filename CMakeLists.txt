cmake_minimum_required(VERSION 2.8)

project(tafl-lib)
set(TAFL_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH "${TAFL_LIB_ROOT}/cmake/modules")

include(cmake/options.cmake)
include(${CMAKE_MODULE_PATH}/CheckCompiler.cmake)
include_directories(include/)

if(TESTS)
  enable_testing()
  add_definitions("-DTEST")
endif(TESTS)

add_subdirectory(src)
if(TESTS)
  add_subdirectory(tests)
endif(TESTS)