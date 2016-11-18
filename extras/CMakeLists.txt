cmake_minimum_required(VERSION 3.1)

project(ok-lib C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# ok-lib
add_library(ok-lib INTERFACE)
target_sources(ok-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../ok_lib.h)
target_include_directories(ok-lib INTERFACE ..)

# Test
file(GLOB test_files "test/*.h" "test/*.c")
add_executable(ok-lib-test ${test_files})
target_link_libraries(ok-lib-test ok-lib)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Weverything -Wno-padded")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Wall -Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Disable 'unknown pragma' and 'shadow variables'
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "/Wall /wd4068 /wd4456")
else()
  set_target_properties(ok-lib-test PROPERTIES COMPILE_FLAGS "-Wall")
endif()

# Example
file(GLOB example_files "example/*.h" "example/*.c")
add_executable(ok-lib-example ${example_files})
target_link_libraries(ok-lib-example ok-lib)

# Wrapper
file(GLOB example_wrapper_files "wrapper/*.h" "wrapper/*.cc")
add_executable(ok-lib-example-wrapper ${example_wrapper_files})
target_link_libraries(ok-lib-example-wrapper ok-lib)
