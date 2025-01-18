cmake_minimum_required(VERSION 3.0.0)

add_executable(Altccents-win
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Note: Use PRIVATE to use lib as private dep
target_link_libraries(Altccents-win PRIVATE Altccents)
