cmake_minimum_required(VERSION 3.20)

project(finance-tui
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.5.0
)

# FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(CLI11 ftxui)

add_executable(finance-tui
    src/main.cpp
)

target_include_directories(finance-tui
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(finance-tui
    PRIVATE
        CLI11::CLI11
        ftxui::component
        ftxui::dom
        ftxui::screen
)

# Strict compiler warnings for the project
if(MSVC)
    target_compile_options(finance-tui PRIVATE
        /W4
        /WX
    )
else()
    target_compile_options(finance-tui PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()
