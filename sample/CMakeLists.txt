# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)
# プロジェクト名と使用する言語を設定
project(sample CXX)
# a.outという実行ファイルをmain.cppとhello.cppから作成
# add_executable(a.out main.cpp hello.cpp)
add_executable(sender sender.cpp ../multicast.cpp ../ring_buffer.cpp)
add_executable(reciever receiver.cpp ../multicast.cpp ../ring_buffer.cpp)