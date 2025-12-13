#!/bin/env bash
# This script contains helper build function

# This function returns build target depending on the OS
get_target(){
    LINUX_TARGET=$(get_build_dir)"Altccents-linux/Altccents-linux"
    WIN_TARGET=$(get_build_dir)"Altccents-win.exe"

    # Manually set target
    if [ -n "$1" ]; then
        echo ${BUILD_DIR}${1}
        return 0
    fi
    # Determine target through determining OS
    case "$(uname -s)" in
        Linux*)
            echo ${LINUX_TARGET}
            ;;
        MINGW*|MSYS*|CYGWIN*)
            echo ${WIN_TARGET}
            ;;
        *)
            echo "Failed to determine target. You may pass target name as first argument." >&2
            return 1
            ;;
    esac
}

# This function returns build directory
get_build_dir(){
    BUILD_DIR="./build/"
    echo $BUILD_DIR
    return 0
}
