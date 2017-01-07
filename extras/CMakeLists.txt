cmake_minimum_required(VERSION 2.8.7) # Version available on Travis CI

project(ok-lib C CXX)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
else ()
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 11)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# ok-lib
include_directories(..)

# Test
file(GLOB test_files "test/*.h" "test/*.c")
add_executable(ok-lib-test ${test_files})
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Weverything -Wno-padded")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Disable 'shadow variables'
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "/Wall /wd4456")
endif()

# Example
file(GLOB example_files "example/*.h" "example/*.c")
add_executable(ok-lib-example ${example_files})

# Wrapper
file(GLOB example_wrapper_files "wrapper/*.h" "wrapper/*.cc")
add_executable(ok-lib-example-wrapper ${example_wrapper_files})
