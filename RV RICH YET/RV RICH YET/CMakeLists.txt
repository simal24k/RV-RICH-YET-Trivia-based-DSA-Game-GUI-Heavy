cmake_minimum_required(VERSION 3.10)
project(kon_banega_crorepati)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_executable(game
    src/main.cpp
    src/Question.cpp
    src/Questionbank.cpp
    src/BaseFunctions.cpp
    src/LifeLines.cpp
)
