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
  COMMENT "Deploy Qt dependencies and resources dir into build"
  # Deploy qt dependencies; windeployqt must be in PATH
  COMMAND powershell -Command "windeployqt '$<TARGET_FILE:Altccents-win>'"
  # Deploy resources dir
  COMMAND powershell -Command "if (Test-Path '$<TARGET_FILE_DIR:Altccents-win>/resources/') { Remove-Item '$<TARGET_FILE_DIR:Altccents-win>/resources/' -Recurse -Force }"
  COMMAND powershell -Command "Copy-Item '${PROJECT_SOURCE_DIR}/resources' -Destination '$<TARGET_FILE_DIR:Altccents-win>/resources' -Recurse -Force"
)

# If Release build type - deploy project into build/release dir
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RELEASE_DIR "$<TARGET_FILE_DIR:Altccents-win>/release/")
    set(T_RELEASE_DIR "$<TARGET_FILE_DIR:Altccents-win>/release/Altccents-win/")

    add_custom_command(
    TARGET Altccents-win POST_BUILD
    COMMENT "Deploy project into build/release"
    # Remove release dir if it already exists
    COMMAND powershell -Command "if (Test-Path '${RELEASE_DIR}') { Remove-Item '${RELEASE_DIR}' -Recurse -Force }"
    # Create release dir
    COMMAND powershell -Command "New-Item '${T_RELEASE_DIR}' -ItemType Directory -Force"
    # Copy binary to release dir
    COMMAND powershell -Command "Copy-Item '$<TARGET_FILE:Altccents-win>' -Destination '${T_RELEASE_DIR}/$<TARGET_FILE_NAME:Altccents-win>'"
    # Copy resources dir to release dir
    COMMAND powershell -Command "Copy-Item '${PROJECT_SOURCE_DIR}/resources' -Destination '${T_RELEASE_DIR}/resources' -Recurse -Force"
    # Copy profiles dir to release dir
    COMMAND powershell -Command "Copy-Item '${PROJECT_SOURCE_DIR}/profiles' -Destination '${T_RELEASE_DIR}/profiles' -Recurse -Force"
    # Deploy Qt dependencies to release dir; windeployqt must be in PATH
    COMMAND powershell -Command windeployqt "${T_RELEASE_DIR}/$<TARGET_FILE_NAME:Altccents-win>"
    # Deploy project into archive; 7z must be in PATH
    COMMAND powershell -Command 7z a "'${RELEASE_DIR}/Altccents-win.zip' '${T_RELEASE_DIR}'"
  )
endif()
