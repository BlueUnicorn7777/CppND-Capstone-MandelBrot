cmake_minimum_required(VERSION 3.11.3)
set(CMAKE_CXX_STANDARD 17)
project(MandelBrot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

find_package(OpenCV 4.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})


file(GLOB project_SRCS MandelBrot/*.cpp )

add_executable(MandelBrot ${project_SRCS})
target_link_libraries(MandelBrot ${OpenCV_LIBRARIES})
