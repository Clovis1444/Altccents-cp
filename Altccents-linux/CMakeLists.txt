cmake_minimum_required(VERSION 3.0.0)

qt_add_executable(Altccents-linux 
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/AltccentsLinux.h
${CMAKE_CURRENT_SOURCE_DIR}/src/AltccentsLinux.cpp
)

target_include_directories(Altccents-linux PRIVATE include)

# Note: Use PRIVATE to use lib as private dep
target_link_libraries(Altccents-linux PRIVATE
Qt6::Core
Qt6::Widgets
Altccents
)
