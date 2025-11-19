cmake_minimum_required(VERSION 3.5.0)

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

add_custom_command(
  TARGET Altccents-linux POST_BUILD
  COMMENT "Deploy Qt dependencies and resources into build dir"
  # Download linuxdeployqt
  COMMAND bash -c "wget https://github.com/probonopd/linuxdeployqt/releases/latest/download/linuxdeployqt-continuous-x86_64.AppImage"
  COMMAND bash -c "chmod a+x linuxdeployqt-continuous-x86_64.AppImage"
  COMMAND bash -c "mv linuxdeployqt-continuous-x86_64.AppImage linuxdeployqt"
  # Deploy qt dependencies; linuxdeployqt will fail on new systems
  COMMAND bash -c linuxdeployqt '$<TARGET_FILE:Altccents-linux>' -always-overwrite || echo 'linuxdeployqt failed successfully!'
  # # Deploy resources dir
  COMMAND bash -c "rm -rf '$<TARGET_FILE_DIR:Altccents-linux>/resources'"
  COMMAND bash -c "cp -r '${PROJECT_SOURCE_DIR}/resources' '$<TARGET_FILE_DIR:Altccents-linux>/resources'"
)

# If Release build type - deploy project into release dir
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RELEASE_DIR "$<TARGET_FILE_DIR:Altccents-linux>/release/")
    set(T_RELEASE_DIR "$<TARGET_FILE_DIR:Altccents-linux>/release/Altccents-linux/")

    add_custom_command(
    TARGET Altccents-linux POST_BUILD
    COMMENT "Deploy project into release dir"
    # Remove release dir if it already exists
    COMMAND bash -c "rm -rf ${RELEASE_DIR}"
    # Create release dir
    COMMAND bash -c "mkdir -p ${T_RELEASE_DIR}"
    # Copy binary to release dir
    COMMAND bash -c "cp '$<TARGET_FILE:Altccents-linux>' '${T_RELEASE_DIR}/$<TARGET_FILE_NAME:Altccents-linux>'"
    # Copy resources dir to release dir
    COMMAND bash -c "cp -r '${PROJECT_SOURCE_DIR}/resources' '${T_RELEASE_DIR}/resources'"
    # Copy profiles dir to release dir
    COMMAND bash -c "cp -r '${PROJECT_SOURCE_DIR}/profiles' '${T_RELEASE_DIR}/profiles'"
    # Deploy Qt dependencies to release dir; linuxdeployqt will fail on new systems
    COMMAND bash -c linuxdeployqt ${T_RELEASE_DIR}/'$<TARGET_FILE_NAME:Altccents-linux>' -always-overwrite || echo 'linuxdeployqt failed successfully!'
    # Deploy project into archive; 7z must be in PATH
    COMMAND bash -c "7z a '${RELEASE_DIR}/Altccents-linux.zip' '${T_RELEASE_DIR}'"
  )
endif()
