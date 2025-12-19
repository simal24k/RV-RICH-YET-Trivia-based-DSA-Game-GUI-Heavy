cmake_minimum_required(VERSION 3.31.1)
project(wwtbam VERSION 0.1.0 LANGUAGES C CXX)

find_package(raylib CONFIG REQUIRED)
add_executable(wwtbam 
    main.cpp
    buttons.cpp
    data_structures.cpp
    game_controller.cpp
    game_engine.cpp
    game_logic.cpp
    game_state.cpp
    leaderboard.cpp
    player_profile.cpp
    question_bank.cpp
    timer.cpp
    raylib_renderer.cpp
    app.rc
    )
target_link_libraries(wwtbam PRIVATE raylib)