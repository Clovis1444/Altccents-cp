#!/bin/bash
# This script builds the project in debug build and runs it

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

# Import build_funcs.sh
source ./build_funcs.sh

TARGET=$(get_target "$1")

# Build
./build.sh || exit 1

# Run
echo
echo "Running ${TARGET} ..." && ${TARGET} || exit 1
