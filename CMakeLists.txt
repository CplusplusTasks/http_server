cmake_minimum_required(VERSION 2.8.4)
project(final)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -DDEBUG -D_FORTIFY_SOURCE=2 -fsanitize=address -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op")

FIND_PACKAGE(Threads)
FIND_PACKAGE(Boost COMPONENTS program_options system filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(SOURCE_FILES final.cpp
                 http_server.cpp)

set(dir "${CMAKE_CURRENT_SOURCE_DIR}/build")
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})

add_executable(final ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(final ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(final ${CMAKE_THREAD_LIBS_INIT})
