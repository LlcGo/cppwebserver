cmake_minimum_required(VERSION 3.10)
#cmake_minimum_required(VERSION 3.14)
project(BXC_HttpServer)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread ")

set(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(INCLUDE_DIR /usr/local/include)
set(LIB_DIR /usr/local/lib)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

find_package(OpenCV REQUIRED)


add_executable(HttpServer HttpServer.cpp main.cpp)
target_link_libraries(HttpServer event jsoncpp ${OpenCV_LIBS})