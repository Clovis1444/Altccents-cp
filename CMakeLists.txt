cmake_minimum_required(VERSION 3.5.0)
project(Altccents VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Weffc++ -Wno-sign-compare -Wconversion -Wsign-conversion -pedantic-errors")

# IMPORTANT: Generate COMPILE_COMMANDS.JSON file
# This line makes headers visible by LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


set(CMAKE_PREFIX_PATH $ENV{QT_PATH})
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Application library
add_subdirectory(Altccents)

# Platform specific actions
if(WIN32)
    # Application Executable
    add_subdirectory(Altccents-win)
elseif(APPLE)
    # Add executable here
elseif(UNIX AND NOT APPLE)
    add_subdirectory(Altccents-linux)
else()
    message(STATUS "UNKNOWN OS")
endif()
