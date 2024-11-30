# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
# プロジェクト名と使用する言語を設定
project(tcp_server CXX)
# a.outという実行ファイルをmain.cppとhello.cppから作成
# add_executable(a.out main.cpp hello.cpp)
add_executable(tcp_server tcp_server.cpp configuration.cpp)
target_compile_options(tcp_server PUBLIC -lpthread -g)