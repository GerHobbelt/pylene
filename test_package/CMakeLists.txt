cmake_minimum_required(VERSION 3.17)
project(PyleneTest)

find_package(pylene REQUIRED COMPONENTS io-freeimage)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC pylene::io-freeimage pylene::scribo)
