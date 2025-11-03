cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_VERSION 0.0.1)

# Note: library is declared static
add_library(Altccents STATIC
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/Altccents.h
${CMAKE_CURRENT_SOURCE_DIR}/src/Altccents.cpp
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/Settings.h
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/AccentProfile/AccentProfile.h
${CMAKE_CURRENT_SOURCE_DIR}/src/AccentProfile/AccentProfile.cpp
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/Popup.h
${CMAKE_CURRENT_SOURCE_DIR}/src/Popup.cpp
${CMAKE_CURRENT_SOURCE_DIR}/include/Altccents/ArgManager/ArgManager.h
${CMAKE_CURRENT_SOURCE_DIR}/src/ArgManager/ArgManager.cpp
)

# Make PROJECT_VERSION accessible in cpp
target_compile_definitions(Altccents PUBLIC PROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(Altccents PRIVATE Qt6::Core Qt6::Widgets)

# Note: use PUBLIC to propagate include dir to the parent
target_include_directories(Altccents PUBLIC include)
