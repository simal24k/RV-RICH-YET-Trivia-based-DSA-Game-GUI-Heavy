cmake_minimum_required(VERSION 3.31.1)
project(rvrichyet VERSION 0.1.0 LANGUAGES C CXX)

find_package(raylib CONFIG REQUIRED)
add_executable(rvrichyet 
main.cpp
timer.cpp
question_bank.cpp
player_profile.cpp
leaderboard.cpp
game_state.cpp
game_logic.cpp
game_engine.cpp
game_controller.cpp 
data_structures.cpp 
gui_main.cpp


)
  target_link_libraries(rvrichyet PRIVATE raylib)

