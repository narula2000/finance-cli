cmake_minimum_required(VERSION 3.20)

project(finance-tui
    VERSION 0.1.0
    LANGUAGES C CXX
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

# SQLite
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
    URL_HASH SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(CLI11 ftxui sqlite3)

if(UNIX)
    find_package(Threads REQUIRED)
endif()

add_library(sqlite3 STATIC
    "${sqlite3_SOURCE_DIR}/sqlite3.c"
)
target_include_directories(sqlite3
    PUBLIC
        "${sqlite3_SOURCE_DIR}"
)
target_compile_definitions(sqlite3
    PRIVATE
        SQLITE_THREADSAFE=1
        SQLITE_ENABLE_FTS5
        SQLITE_ENABLE_JSON1
)
target_link_libraries(sqlite3
    PRIVATE
        Threads::Threads
        ${CMAKE_DL_LIBS}
)
add_library(SQLite::SQLite3 ALIAS sqlite3)

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
        SQLite::SQLite3
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
