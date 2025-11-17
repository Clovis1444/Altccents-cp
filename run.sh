# !bash
# This script builds the project in debug build and runs it

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

BUILD_DIR="./build/"
LINUX_TARGET="Altccents-linux/Altccents-linux"
WIN_TARGET="Altccents-win.exe"
TARGET=""

if [ "$1" ]; then # Manually set target
    TARGET=${BUILD_DIR}${1}
else # Determine target through determining OS
    case "$(uname -s)" in
        Linux*)   TARGET=${BUILD_DIR}${LINUX_TARGET} ;;
        MINGW*|MSYS*|CYGWIN*) TARGET=${BUILD_DIR}${WIN_TARGET} ;;
        *)       echo "Failed to determine target. You may pass target name as first argument." && exit 1 ;;
    esac
fi

echo ${TARGET}

# Build
./build.sh || exit 1

# Run
echo "Running ${TARGET} ..." && ${TARGET} || exit 1
