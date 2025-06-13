cmake_minimum_required(VERSION 3.10)

# Project name
project(aith)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directory
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/system_utils.cpp
    src/string_utils.cpp
    src/file_utils.cpp
    src/config_manager.cpp
    src/provider_manager.cpp
    src/model_blacklist.cpp
    src/api.cpp
    src/history.cpp
    src/markdown.cpp
    src/benchmark.cpp
    src/benchmark_config.cpp
    src/benchmark_request.cpp
    src/benchmark_response.cpp
    src/performance_timer.cpp
    src/benchmark_reporter.cpp
    src/http_client.cpp
    src/api_models.cpp
    src/api_helpers.cpp
)

# Add executable
add_executable(aith ${SOURCES})

# Find and link libraries
find_package(CURL REQUIRED)

# Link the required libraries
target_link_libraries(aith PRIVATE CURL::libcurl)

# Manually find and link JsonCpp library
find_library(JSONCPP_LIB jsoncpp REQUIRED)
if (JSONCPP_LIB)
    target_link_libraries(aith PRIVATE ${JSONCPP_LIB})
else()
    message(FATAL_ERROR "JsonCpp library not found")
endif()