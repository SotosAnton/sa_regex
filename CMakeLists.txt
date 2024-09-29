cmake_minimum_required (VERSION 3.2)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Eigen3 REQUIRED)

project (test_project)

include_directories(include)

set(SOURCES
  test.cpp
  # src/re_util.cpp
  src/re_compiler.cpp
)

set(HEADERS
  include/regex/re_util.hpp
  include/regex/re_compiler.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_property(TARGET PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME})