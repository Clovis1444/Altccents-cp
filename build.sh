# !bash
# This script builds the project

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

BUILD_DIR='./build'
BUILD_GEN="Ninja"
BILD_TYPE="Debug" # Debug build by default

# Check if the user passed "release" as an argument
if [ "$1" = "release" ]; then
    BUILD_TYPE="Release"
elif [ "$1" = "debug" ]; then
    BUILD_TYPE="Debug"
else
    echo "Usage: $0 [debug|release]"
    echo "Defaulting to Debug build."
fi

# Clean and recreate build directory (optional)
# rm -rf "${BUILD_DIR}" && mkdir -p "${BUILD_DIR}"
# Configure
cmake -S . -B "${BUILD_DIR}" -G "${BUILD_GEN}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" || exit 1
# Build
cmake --build "${BUILD_DIR}" || exit 1
