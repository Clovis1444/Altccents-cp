cmake_minimum_required(VERSION 3.0.0)

# Find the libevdev package
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

find_package(X11 REQUIRED)

add_executable(Altccents-linux 
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/Utils.h
)

# Note: Use PRIVATE to use lib as private dep
target_link_libraries(Altccents-linux PRIVATE
Altccents
${LIBEVDEV_LIBRARIES}
${X11_LIBRARIES}
Xtst
Xext
)

# Include the headers for libevdev
# target_include_directories(Altccents-linux PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
# target_include_directories(Altccents-linux PRIVATE ${X11_INCLUDE_DIR})
