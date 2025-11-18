cmake_minimum_required(VERSION 3.31.1)
project(rvrichyet VERSION 0.1.0 LANGUAGES C CXX)

find_package(raylib CONFIG REQUIRED)
add_executable(rvrichyet 
main.cpp
gui_main.cpp


)
  target_link_libraries(rvrichyet PRIVATE raylib)
