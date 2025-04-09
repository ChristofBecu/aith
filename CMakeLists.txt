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
    src/api.cpp
    src/history.cpp
    src/markdown.cpp
)

# Add executable
add_executable(ai ${SOURCES})

# Find and link libraries
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Link the required libraries
target_link_libraries(ai PRIVATE md4c md4c-html CURL::libcurl nlohmann_json::nlohmann_json)

# Manually find and link JsonCpp library
find_library(JSONCPP_LIB jsoncpp REQUIRED)
if (JSONCPP_LIB)
    target_link_libraries(ai PRIVATE ${JSONCPP_LIB})
else()
    message(FATAL_ERROR "JsonCpp library not found")
endif()