cmake_minimum_required(VERSION 3.5.0)

add_executable(Altccents-win
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Note: Use PRIVATE to use lib as private dep
target_link_libraries(Altccents-win PRIVATE
  Qt6::Core
  Qt6::Widgets
  Altccents
)

add_custom_command(
    TARGET Altccents-win POST_BUILD
    # windeployqt must be in PATH
    COMMAND windeployqt "$<TARGET_FILE:Altccents-win>"
    COMMENT "Deploying Qt dependencies with windeployqt"
)
