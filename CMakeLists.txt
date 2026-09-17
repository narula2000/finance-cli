cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(finance-cli
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

# SQLite
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
    URL_HASH SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# SQLiteCpp
# Pinned to master: reuses the pre-existing SQLite::SQLite3 target above via
# SQLITECPP_FIND_SQLITE=OFF and SQLITECPP_INSTALL=OFF (neither exists on tag 3.3.3).
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG 3c31070f0d1b1c46ce4b5ae48bdd0279e3610fa4
)

FetchContent_MakeAvailable(CLI11 sqlite3)

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
        SQLITE_ENABLE_COLUMN_METADATA
)
target_link_libraries(sqlite3
    PRIVATE
        Threads::Threads
        ${CMAKE_DL_LIBS}
)
add_library(SQLite::SQLite3 ALIAS sqlite3)

# Reuse the SQLite::SQLite3 target built above instead of SQLiteCpp's bundled
# sqlite or a system find_package(SQLite3).
set(SQLITECPP_INTERNAL_SQLITE OFF)
set(SQLITECPP_FIND_SQLITE OFF)
set(SQLITECPP_INSTALL OFF)
FetchContent_MakeAvailable(SQLiteCpp)

add_executable(finance-cli
    src/main.cpp
)

target_include_directories(finance-cli
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(finance-cli
    PRIVATE
        CLI11::CLI11
        SQLiteCpp
        SQLite::SQLite3
)

# Strict compiler warnings for the project
if(MSVC)
  target_compile_options(finance-cli PRIVATE
        /W4
        /WX
    )
else()
  target_compile_options(finance-cli PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()
