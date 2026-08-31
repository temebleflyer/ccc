project(Myserver)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
add_executable(Myserver epoll_demo1.cpp)
