cmake_minimum_required(VERSION 3.21)
project(WordleSolver)
set(CMAKE_CXX_STANDARD 20)

set(DICTIONARY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/words.txt")

add_compile_definitions(DICTIONARY_FILE="${DICTIONARY_FILE}")

add_executable(main main.cpp)

