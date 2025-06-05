cmake_minimum_required(VERSION 3.0.0)

# Note: library is declared static
add_library(Altccents STATIC
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/Altccents.h
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/AccentProfile/AccentProfile.h
${CMAKE_CURRENT_SOURCE_DIR}/src/Altccents.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/AccentProfile/AccentProfile.cpp
)

target_link_libraries(Altccents PRIVATE Qt6::Core)

# Note: use PUBLIC to propagate include dir to the parent
target_include_directories(Altccents PUBLIC include)
