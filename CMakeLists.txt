cmake_minimum_required(VERSION 3.10)

# Project name
project(aith)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wno-uninitialized)

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
)

set(MARKDOWN_SOURCES
    src/markdown/markdown.cpp
    src/markdown/terminal_markdown_renderer.cpp
    src/markdown/common/ansi_colors.cpp
    src/markdown/common/text_utils.cpp
    src/markdown/common/word_wrapper.cpp
    src/markdown/blocks/header_block_handler.cpp
    src/markdown/blocks/paragraph_block_handler.cpp
    src/markdown/blocks/code_block_handler.cpp
    src/markdown/blocks/list_block_handler.cpp
    src/markdown/blocks/blockquote_block_handler.cpp
    src/markdown/blocks/horizontal_rule_block_handler.cpp
    src/markdown/blocks/table_block_handler.cpp
    src/markdown/factory/block_handler_factory.cpp
)

set(UTILS_SOURCES
    src/utils/system_utils.cpp
    src/utils/string_utils.cpp
    src/utils/file_operations.cpp
    src/utils/directory_operations.cpp
    src/utils/temp_file_manager.cpp
    src/utils/json_file_handler.cpp
    src/utils/config_file_handler.cpp
    src/utils/file_permissions.cpp
    src/utils/filename_generator.cpp
)

set(COMMANDS_SOURCES
    src/commands/command_line_parser.cpp
    src/commands/command_factory.cpp
    src/commands/list_command.cpp
    src/commands/history_command.cpp
    src/commands/chat_command.cpp
    src/commands/benchmark_command.cpp
    src/commands/blacklist_command.cpp
    src/commands/help_command.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${BLACKLIST_SOURCES}
    ${HTTP_SOURCES}
    ${API_SOURCES}
    ${BENCHMARK_SOURCES}
    ${CHAT_SOURCES}
    ${MARKDOWN_SOURCES}
    ${UTILS_SOURCES}
    ${COMMANDS_SOURCES}
)

# Add executable
add_executable(aith ${ALL_SOURCES})

# Find OpenSSL for HTTPS support
find_package(OpenSSL REQUIRED)

# Fetch cpp-httplib for modern HTTP client functionality
include(FetchContent)

# Fetch httplib with SSL support
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.21.0
)

# Enable HTTPS support for httplib
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE ON CACHE BOOL "Enable OpenSSL support for httplib" FORCE)

# Fetch jsoncpp for JSON handling - disable tests and examples
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 1.9.6
)

# Fetch md4c for C++ markdown rendering - replace mdcat external dependency
FetchContent_Declare(
    md4c
    GIT_REPOSITORY https://github.com/mity/md4c.git
    GIT_TAG release-0.5.2
)

# Configure jsoncpp to not build tests/examples
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Build jsoncpp tests" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Automatically run unit-tests as a post build step" FORCE)
set(JSONCPP_WITH_EXAMPLE OFF CACHE BOOL "Build jsoncpp examples" FORCE)

# Configure md4c to only build what we need
set(MD4C_BUILD_MD2HTML OFF CACHE BOOL "Don't build md2html utility" FORCE)
set(MD4C_BUILD_TESTS OFF CACHE BOOL "Don't build md4c tests" FORCE)

# Temporarily suppress deprecation warnings from external dependencies
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

# Make all dependencies available
FetchContent_MakeAvailable(httplib jsoncpp md4c)
set_target_properties(md4c PROPERTIES COMPILE_FLAGS "-Wno-maybe-uninitialized")

# Restore normal warning behavior after fetching external dependencies
set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)

# Link the required libraries
target_link_libraries(aith PRIVATE httplib::httplib)

# Link OpenSSL for HTTPS support
target_link_libraries(aith PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Link JsonCpp - use the static library target and surpress warnings
target_link_libraries(aith PRIVATE jsoncpp_static)

# Link md4c for markdown rendering
target_link_libraries(aith PRIVATE md4c)

# Add md4c include directory
target_include_directories(aith PRIVATE ${md4c_SOURCE_DIR}/src)