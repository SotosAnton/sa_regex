cmake_minimum_required (VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

option ( TRACY_ENABLE "" OFF )

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# Ensure debug symbols are included
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_DEBUG "-g")

project (sregex)
include_directories(include)

set(SOURCES  
  # src//til.cpp
  src/state_machine.cpp
  src/builder.cpp
  src/state_machine_export.cpp
  src/reTree.cpp
  src/opcode.cpp
  src/sa_regex.cpp
  src/state_machine_executor.cpp  
)

if(TRACY_ENABLE)
  option ( TRACY_ON_DEMAND "" ON )
  option ( TRACY_NO_EXIT "" ON )
  add_subdirectory ( external/tracy ) # target : TracyClient or alias Tracy :: TracyClient
endif()

# Add Tracy's source if enabled
if(TRACY_ENABLE)
  list(APPEND SOURCES external/tracy/public/TracyClient.cpp)
endif()

set(HEADERS
  include/regex/util.hpp
  include/regex/state_machine.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(TRACY_ENABLE)
  target_link_libraries(${PROJECT_NAME} PUBLIC Tracy::TracyClient)
endif()

add_executable(${PROJECT_NAME}_run test.cpp )
add_executable(${PROJECT_NAME}_test run_tests.cpp )
add_executable(${PROJECT_NAME}_benchmark benchmark.cpp )

set_property(TARGET PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME}_run ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_benchmark ${PROJECT_NAME})
