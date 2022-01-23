cmake_minimum_required(VERSION 3.21)
project(WordleSolver)
set(CMAKE_CXX_STANDARD 20)
find_package(fmt REQUIRED)
find_package(TBB REQUIRED)

set(DICTIONARY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/limited_words.txt")

add_compile_definitions(DICTIONARY_FILE="${DICTIONARY_FILE}")

add_executable(main main.cpp)
target_link_libraries(main fmt::fmt-header-only)
target_link_libraries(main TBB::tbb)
