cmake_minimum_required(VERSION 2.8)
project(simple-chat)

set(CXX_DEBUG "-Wall -Wextra -g -D_GLIBCXX_DEBUG -fsanitize=undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_DEBUG} -std=c++14")

include_directories(libhttp/include)
include_directories(libhttp/libtcp/include)
include_directories(include)
set(SOURCES	
		src/simple_chat_server.cc
		)

add_subdirectory(libhttp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} http)
