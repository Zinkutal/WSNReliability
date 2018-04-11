cmake_minimum_required(VERSION 3.10)
project(WSNReliability)
message(STATUS "start running cmake...")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Xpreprocessor -fopenmp -lomp -Wall")

# Require OpenCL for this project:
find_package(OpenCL REQUIRED )

# Require Boost for this project:
find_package(Boost 1.66.0 COMPONENTS log log_setup thread system REQUIRED)
set(Boost_USE_MULTITHREADED ON)

# Require RapidJSON for this project:
find_package(RAPIDJSON 1.1.0 REQUIRED)

# Require RapidJSON for this project:
find_package(OPENCV 3.4.1 REQUIRED)

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

if(OPENCV_FOUND)
    message(STATUS "OPENCV_VERSION: ${OPENCV_VERSION}")
elseif(OPENCV_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find OPENCV")
else()
    message(STATUS "Optional package OPENCV was not found")
endif()

#Attach custom logger
add_library(simpleLogger Utils/libs/simpleLogger/simpleLogger.cpp)

add_executable(WSNReliability main.cpp)

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

if(OPENCV_FOUND)
    target_link_libraries(WSNReliability ${OpenCV_LIBS})
endif()

if(OPENCL_FOUND)
    message(STATUS "OPENCL_VERSION: ${OPENCL_VERSION}")

    if(OPENCL_FOUND)
        target_link_libraries(WSNReliability OpenCL::OpenCL)
    endif()
elseif(OPENCL_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find OPENCL")
else()
    message(STATUS "Optional package OPENCL was not found")
endif()
