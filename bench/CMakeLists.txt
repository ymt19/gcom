cmake_minimum_required(VERSION 3.13)
project(microbench CXX)
set(CMAKE_CXX_STANDARD 20)

# thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(LIBRARY_NAME cereal/)

add_executable(tcp_server tcp_server.cpp tcp_connection_manager.cpp configuration.cpp transaction.cpp background.cpp)
target_link_libraries(tcp_server
    Threads::Threads
)