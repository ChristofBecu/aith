cmake_minimum_required(VERSION 3.10)

# Project name
project(ai)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(ai main.cpp)

# Find and link libraries
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Link the md4c and md4c-html libraries explicitly
target_link_libraries(ai PRIVATE md4c md4c-html)

# Manually find and link JsonCpp library
find_library(JSONCPP_LIB jsoncpp REQUIRED)
if (JSONCPP_LIB)
    target_link_libraries(ai PRIVATE ${JSONCPP_LIB})
else()
    message(FATAL_ERROR "JsonCpp library not found")
endif()

# Ensure CURL and nlohmann_json are linked
target_link_libraries(ai PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)