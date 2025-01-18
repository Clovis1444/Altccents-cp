cmake_minimum_required(VERSION 3.0.0)

add_executable(Altccents-linux 
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Note: Use PRIVATE to use lib as private dep
target_link_libraries(Altccents-linux PRIVATE Altccents)
