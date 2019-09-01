cmake_minimum_required(VERSION 3.14)
project(controller)

set(CMAKE_CXX_STANDARD 17)

add_executable(
  controller

  main.cpp
)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIR})
# Note: a target should be already defined using 'add_executable' or 'add_library'
target_link_libraries(controller ${Boost_LIBRARIES})
