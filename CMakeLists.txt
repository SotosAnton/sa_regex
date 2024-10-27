cmake_minimum_required (VERSION 3.2)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Eigen3 REQUIRED)

project (sregex)

include_directories(include)

set(SOURCES
  
  # src//til.cpp
  src/state_machine.cpp
  src/builder.cpp
  src/state_machine_export.cpp
  src/tree_machine.cpp
  src/reTree.cpp
  src/opcode.cpp
  
)

set(HEADERS
  include/regex/util.hpp
  include/regex/state_machine.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_executable(${PROJECT_NAME}_run test.cpp )
add_executable(${PROJECT_NAME}_test run_tests.cpp )

set_property(TARGET PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME}_run ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})