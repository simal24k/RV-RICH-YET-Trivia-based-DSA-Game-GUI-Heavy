cmake_minimum_required(VERSION 3.31.1)
project(rvrichyet VERSION 0.1.0 LANGUAGES C CXX)

find_package(raylib CONFIG REQUIRED)
add_executable(rvrichyet 
main.cpp
data_structures.cpp
game_controller.cpp
game_engine.cpp
game_logic.cpp
game_state.cpp
gui_main.cpp
leaderboard.cpp
player_profile.cpp
question_bank.cpp
timer.cpp

)
  target_link_libraries(rvrichyet PRIVATE raylib)
