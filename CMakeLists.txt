cmake_minimum_required(VERSION 3.10)

# Project name
project(ai)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directory
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/system_utils.cpp
    src/config_manager.cpp
    src/provider_manager.cpp
    src/model_blacklist.cpp
    src/api.cpp
    src/history.cpp
    src/markdown.cpp
    src/benchmark.cpp
)

# Add executable
add_executable(ai ${SOURCES})

# Find and link libraries
find_package(CURL REQUIRED)

# Link the required libraries
target_link_libraries(ai PRIVATE CURL::libcurl)

# Manually find and link JsonCpp library
find_library(JSONCPP_LIB jsoncpp REQUIRED)
if (JSONCPP_LIB)
    target_link_libraries(ai PRIVATE ${JSONCPP_LIB})
else()
    message(FATAL_ERROR "JsonCpp library not found")
endif()