cmake_minimum_required(VERSION 3.10)
project(WSNReliability)
message(STATUS "start running cmake...")

set(CMAKE_CXX_STANDARD 11)

# Require Boost for this project:
find_package(Boost 1.66.0 COMPONENTS log log_setup thread system REQUIRED)
set(Boost_USE_MULTITHREADED ON)

# Require RapidJSON for this project:
find_package(RAPIDJSON 1.1.0 REQUIRED)

# Require RapidJSON for this project:
find_package(OPENCV 3.4.1 REQUIRED)

# Require MPI for this project:
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

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

if(MPI_FOUND)
    message(STATUS "MPI_INCLUDE_DIRS: ${MPI_INCLUDE_DIRS}")
    message(STATUS "MPI_VERSION: ${MPI_VERSION}")

    include_directories(MPI_INCLUDE_PATH)
elseif(MPI_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find MPI")
else()
    message(STATUS "Optional package MPI was not found")
endif()

if(OPENCV_FOUND)
    message(STATUS "OPENCV_VERSION: ${OPENCV_VERSION}")
elseif(OPENCV_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find OPENCV")
else()
    message(STATUS "Optional package OPENCV was not found")
endif()

add_library(simpleLogger utils/libs/simpleLogger/simpleLogger.cpp)

add_executable(WSNReliability main.cpp Controllers/Methods/AccurateMethod.cpp)#Models/Graph.h Models/Node.h Controllers/Methods/AccurateMethod.cpp

if(Boost_FOUND)
    target_link_libraries(WSNReliability ${Boost_LIBRARIES})
    target_link_libraries(WSNReliability
            ${Boost_LOG_SETUP_LIBRARY}
            ${Boost_LOG_LIBRARY}
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_THREAD_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            )
    target_link_libraries(WSNReliability simpleLogger)
endif()

if(MPI_FOUND)
    target_link_libraries(WSNReliability ${MPI_LIBRARIES})
endif()

if(OPENCV_FOUND)
    target_link_libraries( WSNReliability ${OpenCV_LIBS} )
endif()