cmake_minimum_required(VERSION 3.5.0)
project(Altccents VERSION 3.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Weffc++ -Wno-sign-compare -Wconversion -Wsign-conversion -pedantic-errors")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# IMPORTANT: Generate COMPILE_COMMANDS.JSON file
# This line makes headers visible by LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Define project metadata
set(COMPANY_NAME "Clovis1444")
set(FILE_DESC "Simple utility for typing accented characters")
set(PRODUCT_NAME "Altccents")
set(FILE_ICON "${PROJECT_SOURCE_DIR}/resources/icon.ico")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(COPYRIGHT "Copyright (c) ${CURRENT_YEAR} ${COMPANY_NAME}")


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
