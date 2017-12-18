cmake_minimum_required(VERSION 2.8.7) # Version available on Travis CI

project(ok-lib C CXX)

option(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
else()
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 11)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

if (TREAT_WARNINGS_AS_ERRORS)
  if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(TREAT_WARNINGS_AS_ERRORS_FLAG "/WX")
  else()
    set(TREAT_WARNINGS_AS_ERRORS_FLAG "-Werror")
  endif()
else()
    set(TREAT_WARNINGS_AS_ERRORS_FLAG "")
endif()

# ok-lib
include_directories(..)
source_group("" FILES ../ok_lib.h)

# Test
file(GLOB test_files "test/*.h" "test/*.c")
add_executable(ok-lib-test ../ok_lib.h ${test_files})
if (CMAKE_C_COMPILER_ID MATCHES "Clang")
  # Enable -Wwrite-strings because -Weverything doesn't enable it in all versions of Clang
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Weverything -Wwrite-strings -Wno-padded ${TREAT_WARNINGS_AS_ERRORS_FLAG}")
  target_link_libraries(ok-lib-test pthread)
elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
  # Disable unused-functions because of this GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64079
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wwrite-strings -Wno-unused-function ${TREAT_WARNINGS_AS_ERRORS_FLAG}")
  target_link_libraries(ok-lib-test pthread)
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
  # Disable 'shadow variables', 'function not inlined', and 'struct padding'
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "/Wall /wd4456 /wd4710 /wd4820 ${TREAT_WARNINGS_AS_ERRORS_FLAG}")
endif()

# CTest setup, using valgrind if found. Do not use valgrind on macOS, which crashes when starting threads
enable_testing()
if (NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
  find_program(MEMCHECK_COMMAND valgrind)
  set(MEMCHECK_COMMAND_OPTIONS "--error-exitcode=1" "--leak-check=full")
endif()
if (NOT MEMCHECK_COMMAND)
  add_test(NAME ok-lib-test COMMAND ok-lib-test)
else()
  add_test(NAME ok-lib-test-memcheck COMMAND ${MEMCHECK_COMMAND} ${MEMCHECK_COMMAND_OPTIONS} ./ok-lib-test)
endif()

# Example
file(GLOB example_files "example/*.h" "example/*.c")
add_executable(ok-lib-example ../ok_lib.h ${example_files})
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set_target_properties(ok-lib-example PROPERTIES COMPILE_FLAGS "-Wno-unused-value")
endif()

# Wrapper
file(GLOB example_wrapper_files "wrapper/*.h" "wrapper/*.cc")
add_executable(ok-lib-example-wrapper ../ok_lib.h ${example_wrapper_files})
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set_target_properties(ok-lib-example-wrapper PROPERTIES COMPILE_FLAGS "-Wno-unused-value")
endif()
