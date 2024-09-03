cmake_minimum_required(VERSION 3.16)
project(my_bench VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${PROJECT_NAME}-googletest ring_buffer.cpp test/test_ring_buffer.cpp)
target_link_libraries(${PROJECT_NAME}-googletest gtest_main)
target_include_directories(${PROJECT_NAME}-googletest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_test(NAME test COMMAND ${PROJECT_NAME}-googletest)
enable_testing()