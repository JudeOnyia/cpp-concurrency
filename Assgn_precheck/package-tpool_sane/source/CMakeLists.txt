cmake_minimum_required(VERSION 3.10)
project(concurrency LANGUAGES CXX)

include(Sanitizers.cmake)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)


if(EXISTS ${CMAKE_SOURCE_DIR}/include/ra/queue.hpp OR
  EXISTS ${CMAKE_SOURCE_DIR}/include/ra/thread_pool.hpp OR
  EXISTS ${CMAKE_SOURCE_DIR}/app/test_queue.cpp OR
  EXISTS ${CMAKE_SOURCE_DIR}/app/test_thread_pool.cpp)
	set(TPOOL_FOUND 1)
else()
	set(TPOOL_FOUND 0)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/include/ra/julia_set.hpp OR
  EXISTS ${CMAKE_SOURCE_DIR}/app/test_julia_set.cpp)
	set(FRACTAL_FOUND 1)
else()
	set(FRACTAL_FOUND 0)
endif()

message(STATUS "Thread pool found: ${TPOOL_FOUND}")
message(STATUS "Fractal found: ${FRACTAL_FOUND}")

if(EXISTS ${CMAKE_SOURCE_DIR}/lib/thread_pool.cpp)
	set(THREAD_POOL_CPP lib/thread_pool.cpp)
else()
	set(THREAD_POOL_CPP "")
endif()

find_package(Threads REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU OR
  CMAKE_CXX_COMPILER_ID MATCHES Clang)
	add_compile_options("-pedantic-errors")
endif()

include_directories(include)

if(TPOOL_FOUND)
	add_executable(test_queue app/test_queue.cpp)
	target_link_libraries(test_queue Threads::Threads)
	add_executable(test_thread_pool ${THREAD_POOL_CPP} app/test_thread_pool.cpp)
	target_link_libraries(test_thread_pool Threads::Threads)
	if(EXISTS ${CMAKE_SOURCE_DIR}/app/trivial_queue.cpp OR
	  EXISTS ${CMAKE_SOURCE_DIR}/app/trivial_thread_pool.cpp)
		add_executable(trivial_queue app/trivial_queue.cpp)
		target_link_libraries(trivial_queue Threads::Threads)
		add_executable(trivial_thread_pool
		  ${THREAD_POOL_CPP} app/trivial_thread_pool.cpp)
		target_link_libraries(trivial_thread_pool Threads::Threads)
	endif()
	if(EXISTS ${CMAKE_SOURCE_DIR}/app/demo_queue.cpp OR
	  EXISTS ${CMAKE_SOURCE_DIR}/app/demo_thread_pool.cpp)
		find_package(Boost COMPONENTS REQUIRED thread)
		add_executable(demo_queue app/demo_queue.cpp)
		target_link_libraries(demo_queue Boost::thread Threads::Threads)
		# Note: Is this a bug in CMake?
		# I do not understand why the following line is needed.
		target_link_libraries(demo_queue pthread)
		add_executable(demo_thread_pool
		  ${THREAD_POOL_CPP} app/demo_thread_pool.cpp)
		target_link_libraries(demo_thread_pool Threads::Threads)
	endif()
endif()

if(FRACTAL_FOUND)
	add_executable(test_julia_set ${THREAD_POOL_CPP} app/test_julia_set.cpp)
	target_link_libraries(test_julia_set Threads::Threads)
	if(EXISTS ${CMAKE_SOURCE_DIR}/app/trivial_julia_set.cpp)
		add_executable(trivial_julia_set
		  ${THREAD_POOL_CPP} app/trivial_julia_set.cpp)
		target_link_libraries(trivial_julia_set Threads::Threads)
	endif()
	if(EXISTS ${CMAKE_SOURCE_DIR}/app/demo_julia_set.cpp)
		add_executable(demo_julia_set ${THREAD_POOL_CPP} app/demo_julia_set.cpp)
		target_link_libraries(demo_julia_set Threads::Threads)
	endif()
endif()
