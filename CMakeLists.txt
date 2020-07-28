include(Sanitizers.cmake) # REMOVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

# Specify Minimum Required Version
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

# Specify Project and Language
project(cpp_concurrency LANGUAGES CXX)

# Require compliance with C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE Debug) # REMOVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

# Set Include Directory
include_directories(include)

# Find the threads library, indicating a preference for the pthread library
set(THREADS_PREFER_PTHREAD_FLAG true)
find_package(Threads REQUIRED)

# Add Executable Program
add_executable(test_queue app/test_queue.cpp)

# Set the libraries for the target
target_link_libraries(test_queue Threads::Threads)
