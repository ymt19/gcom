# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
# プロジェクト名と使用する言語を設定
project(bench CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(tcp_server tcp_server.cpp tcp_connection_manager.cpp configuration.cpp transaction.cpp)
target_link_libraries(tcp_server
    Threads::Threads
)