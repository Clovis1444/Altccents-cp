#!/bin/bash
# This script builds project and runs it
#
# Args:
# --target:<value>       [OPTIONAL]        build selected target
# --build:<value>        [OPTIONAL]        set build dir

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

BUILD_DIR="./build"
TARGET="/Altccents-linux/Altccents-linux"
RUN_TARGET=${BUILD_DIR}${TARGET}
# USE_SUDO="sudo"

SET_TARGET=""
SET_BUILD_DIR=""
for arg in "$@"; do
    if [[ $arg == --target:* ]]; then
        SET_TARGET="$arg"
        TARGET="${arg#--target:}"
    fi
    if [[ $arg == --build:* ]]; then
        SET_BUILD_DIR="$arg"
        BUILD_DIR="${arg#--build:}"
    fi
done

# Build
./build.sh ${SET_TARGET} ${SET_BUILD_DIR} || exit 1

# Run
${USE_SUDO} ${RUN_TARGET} || exit 1
