cmake_minimum_required(VERSION 3.9)
project(WSNReliability)
message(STATUS "start running cmake...")

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.66.0 COMPONENTS system filesystem REQUIRED)
find_package(RAPIDJSON 1.1.0 REQUIRED)

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
elseif(Boost_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Boost")
else()
    message(STATUS "Optional package Boost was not found")
endif()

if(RAPIDJSON_FOUND)
    message(STATUS "RAPIDJSON_INCLUDE_DIRS: ${RAPIDJSON_INCLUDE_DIRS}")
    message(STATUS "RAPIDJSON_VERSION: ${RAPIDJSON_VERSION}")

    include_directories(${RAPIDJSON_INCLUDE_DIRS})
elseif(RAPIDJSON_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find RAPIDJSON")
else()
    message(STATUS "Optional package RAPIDJSON was not found")
endif()

add_executable(WSNReliability main.cpp)

if(Boost_FOUND)
    target_link_libraries(WSNReliability ${Boost_LIBRARIES})
endif()