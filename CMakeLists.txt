cmake_minimum_required(VERSION 3.10)

# Project name
project(aith)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(include/core)
include_directories(include/blacklist)
include_directories(include/http)
include_directories(include/api)
include_directories(include/benchmark)
include_directories(include/chat)
include_directories(include/utils)
include_directories(include/commands)

# Source files organized by domain
set(CORE_SOURCES
    src/core/api_manager.cpp
    src/core/config_manager.cpp
    src/core/provider_manager.cpp
    src/core/blacklist_manager.cpp
    src/core/application_setup.cpp
    src/core/application_controller.cpp
)

set(BLACKLIST_SOURCES
    src/blacklist/blacklist_file_manager.cpp
    src/blacklist/blacklist_parser.cpp
    src/blacklist/blacklist_operation.cpp
    src/blacklist/blacklist_operation_factory.cpp
    src/blacklist/blacklist_check_operation.cpp
    src/blacklist/blacklist_add_operation.cpp
    src/blacklist/blacklist_remove_operation.cpp
    src/blacklist/blacklist_list_operation.cpp
)

set(HTTP_SOURCES
    src/http/http_client.cpp
)

set(API_SOURCES
    src/api/api_models.cpp
    src/api/api_helpers.cpp
)

set(BENCHMARK_SOURCES
    src/benchmark/benchmark.cpp
    src/benchmark/benchmark_config.cpp
    src/benchmark/benchmark_request.cpp
    src/benchmark/benchmark_response.cpp
    src/benchmark/benchmark_reporter.cpp
    src/benchmark/benchmark_runner.cpp
    src/benchmark/performance_timer.cpp
)

set(CHAT_SOURCES
    src/chat/history.cpp
    src/chat/markdown.cpp
)

set(UTILS_SOURCES
    src/utils/system_utils.cpp
    src/utils/string_utils.cpp
    src/utils/file_utils.cpp
    src/utils/file_operations.cpp
    src/utils/directory_operations.cpp
    src/utils/temp_file_manager.cpp
    src/utils/json_file_handler.cpp
    src/utils/config_file_handler.cpp
    src/utils/file_permissions.cpp
)

set(COMMANDS_SOURCES
    src/commands/command_line_parser.cpp
    src/commands/command_factory.cpp
    src/commands/list_command.cpp
    src/commands/history_command.cpp
    src/commands/chat_command.cpp
    src/commands/benchmark_command.cpp
    src/commands/blacklist_command.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${BLACKLIST_SOURCES}
    ${HTTP_SOURCES}
    ${API_SOURCES}
    ${BENCHMARK_SOURCES}
    ${CHAT_SOURCES}
    ${UTILS_SOURCES}
    ${COMMANDS_SOURCES}
)

# Add executable
add_executable(aith ${ALL_SOURCES})

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