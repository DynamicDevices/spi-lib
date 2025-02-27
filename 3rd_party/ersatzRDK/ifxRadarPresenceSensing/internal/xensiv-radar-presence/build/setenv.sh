#!/bin/bash
#
# This script sets the asset build environment (sourced by other scripts)

# Set path to the root directory of the git repository
ROOT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Windows: convert /cygdrive/d/git to D:/git
if [[ ${OS:-} == Windows_NT ]]; then
    ROOT_DIR="$(cygpath --mixed "$ROOT_PATH")"
else
    ROOT_DIR="$ROOT_PATH"
fi

# Set path to the build scripts
SCRIPTS_DIR="$ROOT_DIR/build"

# Set path to the input directory for downloaded assets
INPUT_DIR="$ROOT_DIR/input"

# Set path to the output directory for generated assets
OUT_DIR="$ROOT_DIR/output"

# Set path to the ModusToolbox tools
export CY_TOOLS_DIR_2_4="$OUT_DIR/tools_2.4"
export CY_TOOLS_DIR_3_0="$OUT_DIR/tools_3.0"

# Set path to the asset libraries
LIB_DIR="$OUT_DIR/libs"

# Set path to the BSP libraries
BSP_DIR="$OUT_DIR/bsps"

# Set paths to the Peripheral Driver Libraries
PDL_CAT1_DIR="$LIB_DIR/mtb-pdl-cat1"
PDL_CAT2_DIR="$LIB_DIR/mtb-pdl-cat2"
XMC_LIB_DIR="$LIB_DIR/mtb-xmclib-cat3"


# Set asset packaging variables:
#
# PKG_NAME - repo-staging/github asset repository name
# LIB_NAME - asset display name (for doxygen)
# PKG_MAJOR - asset major version
# PKG_MINOR - asset minor version
# PKG_PATCH - asset patch version
# PKG_BUILD - asset build number (assigned by GitLab CI pipeline, "0" for local developer build)
#
# More details: https://confluencewikiprod.intra.infineon.com/x/sqSoEw
#
PKG_NAME=xensiv-radar-presence
LIB_NAME="XENSIV RADAR Presence Detection Library"
PKG_MAJOR=1
PKG_MINOR=0
PKG_PATCH=0
PKG_BUILD=${CI_PIPELINE_IID:-0}
PKG_VERSION=${PKG_MAJOR}.${PKG_MINOR}.${PKG_PATCH}.${PKG_BUILD}
PKG_OUT_DIR="${OUT_DIR}/${PKG_NAME}"
PKG_OUT_ZIP="${OUT_DIR}/${PKG_NAME}.zip"


# Set OS-specific host tool locations
#
# It is possible to override the default paths defined below by exporting
# the appropriate variable into the environment before running the script.
#
# This is example of .gitlab-ci.yml definition to override PYTHON and IAR_DIR:
# test_sut_iar:
#   variables:
#     PYTHON: python3
#     IAR_DIR: "C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.2/arm"
#
# Make sure the toolchain versions used for CI/CD testing (GCC_DIR, IAR_DIR, ARMCC_DIR)
# match the asset release notes - check "Supported Software and Tools" RELEASE.md section
#
KERNEL="$($(which uname) -s)"
case "$KERNEL" in
    CYGWIN*|MINGW*|MSYS*)
        # Cygwin, MinGW, Git Bash, MSYS2 provide cygpath command
        command -v cygpath >/dev/null 2>&1 || { echo "ERROR: cygpath tool not found."; exit 1; }

        # Windows path conversion rules:
        # "cygpath --unix"  - creates /cygdrive/c/ paths - suitable for inclusion into $PATH etc.
        # "cygpath --mixed" - creates C:/ paths - compatible with all host-native applications
        #
        # /cygdrive/c/ paths are accepted only by Cygwin-native tools, never pass to Win32 tools.

        # Download Windows dependencies
        export CY_DEP_DIR="${CY_DEP_DIR:-$LOCALAPPDATA/cydep}"
        export CY_DEP_MANIFEST="${SCRIPTS_DIR}/manifests/dependencies-windows.txt"
        export PATH="$(cygpath --unix "$CY_DEP_DIR/doxygen-1.8.14.windows.x64.bin"):$PATH"
        export PATH="$(cygpath --unix "$CY_DEP_DIR/cmake-3.21.1-windows-x86_64/bin"):$PATH"
        export PATH="$(cygpath --unix "$CY_DEP_DIR/ninja-1.9.0-win"):$PATH"

        # Set path to GCC toolchain root directory
        export GCC_DIR="$(cygpath --mixed "${GCC_DIR:-$CY_DEP_DIR/gcc-arm-none-eabi-10.3-2021.07-win32}")"

        # Set path to IAR toolchain root directory
        export IAR_DIR="$(cygpath --mixed "${IAR_DIR:-C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.4/arm}")"

        # Set path to ARMCC toolchain root directory
        export ARMCC_DIR="$(cygpath --mixed "${ARMCC_DIR:-C:/Program Files/ARMCompiler6.13}")"

        # Set path to Coverity root directory
        export COVERITY_DIR="$(cygpath --mixed "${COVERITY_DIR:-$LOCALAPPDATA/Programs/Coverity/Coverity Static Analysis}")"

        # Use "python" to find python3 executable
        export PYTHON="${PYTHON:-python}"

        # Set path to Nanopb root directory
        export NANOPB_DIR="$(cygpath --mixed "${NANOPB_DIR:-$CY_DEP_DIR/nanopb-0.4.6-windows-x86}")"

        # Set ModusToolbox tools suffix (used by fetch_tools)
        CY_TOOLS_SUFFIX=windows
        ;;
    Linux*)
        # Download Linux dependencies
        XDG_CACHE_HOME="${XDG_CACHE_HOME:-$HOME/.cache/cydep}"
        export CY_DEP_DIR="${CY_DEP_DIR:-$XDG_CACHE_HOME}"
        export CY_DEP_MANIFEST="${SCRIPTS_DIR}/manifests/dependencies-linux.txt"
        export PATH="$CY_DEP_DIR/doxygen-1.8.14.1.linux.bin/bin:$PATH"
        export PATH="$CY_DEP_DIR/cmake-3.21.1-linux-x86_64/bin:$PATH"
        export PATH="$CY_DEP_DIR/ninja-1.9.0-linux-x86_64/bin:$PATH"

        # Doxygen needs libclang.so.6 but older Ubuntu doesn't have it
        export LD_LIBRARY_PATH="$CY_DEP_DIR/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/lib"

        # Set path to GCC toolchain root directory
        export GCC_DIR="${GCC_DIR:-$CY_DEP_DIR/gcc-arm-none-eabi-10.3-2021.07-x86_64-linux}"

        # Use "python3" to find Python3 executable ("python" is Python2)
        export PYTHON="${PYTHON:-python3}"

        # Set path to Nanopb root directory
        export NANOPB_DIR="${NANOPB_DIR:-$CY_DEP_DIR/nanopb-0.4.6-linux-x86}"

        # Set ModusToolbox tools suffix (used by fetch_tools)
        CY_TOOLS_SUFFIX=linux
        ;;
    Darwin*)
        # Download MacOS dependencies
        XDG_CACHE_HOME="${XDG_CACHE_HOME:-$HOME/.cache/cydep}"
        export CY_DEP_DIR="${CY_DEP_DIR:-$XDG_CACHE_HOME}"
        export CY_DEP_MANIFEST="${SCRIPTS_DIR}/manifests/dependencies-osx.txt"
        export PATH="$CY_DEP_DIR/doxygen-1.8.14-osx/Doxygen.app/Contents/Resources:$PATH"
        export PATH="$CY_DEP_DIR/cmake-3.21.1-osx/Cmake.app/Contents/Resources:$PATH"
        export PATH="$CY_DEP_DIR/ninja-1.9.0-osx/Ninja.app/Contents/Resources:$PATH"

        # Set path to GCC toolchain root directory
        export GCC_DIR="${GCC_DIR:-$CY_DEP_DIR/gcc-arm-none-eabi-10.3-2021.07-mac}"

        # Set path to Coverity root directory
        export COVERITY_DIR="/Applications/cov-analysis-macosx-2021.9.0"

        # Use "python3" to find Python3 executable ("python" is Python2)
        export PYTHON="${PYTHON:-python3}"

        # Set path to Nanopb root directory
        export NANOPB_DIR="${NANOPB_DIR:-$CY_DEP_DIR/nanopb-0.4.6-macos-x86}"

        # Set ModusToolbox tools suffix (used by fetch_tools)
        CY_TOOLS_SUFFIX=macos
        ;;
    *)
        echo "ERROR: unsupported OS: $KERNEL"
        exit 1
        ;;
esac

# Customize common options for Wget tool
WGET_CMD="${WGET_CMD:-wget}"
# Suppress download progress reporting in CI pipeline log
[[ -n ${CI:-} ]] && WGET_CMD="$WGET_CMD --no-verbose"

# Define FLEXlm network servers to acquire ARM Compiler license
export ARMLMD_LICENSE_FILE="${ARMLMD_LICENSE_FILE:-"3500@auclal11.auc.infineon.com,3500@auclal12.auc.infineon.com,3500@auclal13.auc.infineon.com"}"

# Create directory links in cross-platform way:
# Windows: use "mklink /J" to create NTFS directory junction
# Linux/macOS: use "ln -s" to create POSIX symbolic link
#
# Arguments:
# $1 - source directory (existing)
# $2 - target directoty (new link to create)
function symlink_directory()
{
    local source_dir=$1
    local target_dir=$2

    # Silently return in case the target already exists
    [[ -e "$target_dir" ]] && return

    # Report error in case the source directory doesn't exist
    [[ -d "$source_dir" ]] || { echo "ERROR: $$source_dir directory doesn't exist."; exit 1; }

    # Create parent directory
    mkdir -p $(dirname "$target_dir")

    case "$KERNEL" in
        CYGWIN*|MINGW*|MSYS*)
            source_dir="$(cygpath -w "$source_dir")"
            target_dir="$(cygpath -w "$target_dir")"
            mklink_cmd="mklink /J \"$target_dir\" \"$source_dir\""
            cmd <<< $mklink_cmd
            ;;
        *)
            ln -s "$source_dir" "$target_dir"
            ;;
    esac
}

# Check if the .timestamp file is up to date
# Arguments:
# 1 - path to the .timestamp file
# 2 - the threshold in seconds to decide if the timestamp is new enough
#     default threshold: 86400 seconds = 60*60*24 = 1 day
# Returns:
# 0 - the timestamp is up to date
# 1 - the timestamp is out of date
#
function check_timestamp()
{
    local timestamp_file=$1
    local threshold_sec=${2:-86400}
    # when the timestamp file doesn't exist, always download the artifacts
    [[ -f "$timestamp_file" ]] || return 1
    # read the timestamp from file
    local timestamp_date=$(< "$timestamp_file")
    # convert timestamp to UNIX time in seconds.
    # GNU date: use "date -d", BSD date: use "date -j -f"
    local timestamp_sec=$(date -d "$timestamp_date" "+%s" 2>/dev/null || \
    date -j -f "%Y-%m-%d %H:%M:%S" "$timestamp_date" "+%s" || echo 0)
    # determine the current UNIX time in seconds
    local current_sec=$(date "+%s")
    # check if the current time exceeds timestamp by threshold
    return $(( $current_sec - $timestamp_sec >= $threshold_sec ))
}

# Save the timestamp to file in the following format:
# 2019-04-03 14:09:53
function save_timestamp()
{
    local timestamp_file=$1
    local current_date=$(date "+%Y-%m-%d %H:%M:%S")
    echo "$current_date" > "$timestamp_file"
}

# Download third-party dependencies
function download_deps()
{
    local cmd=bash
    # Use flock(1) in case the host platform supports directory locking
    # Lock CY_DEP_DIR to avoid simultaneous download from multiple jobs
    # Wait 10 minutes for the CY_DEP_DIR to become available
    mkdir -p "$CY_DEP_DIR"
    type flock >/dev/null 2>&1 && cmd="flock -w 600 $CY_DEP_DIR bash"
    $cmd "${SCRIPTS_DIR}/downloaddeps.sh" "$CY_DEP_MANIFEST"
}

# Download and extract tar.gz artifact from WEB location
function fetch_tgz()
{
    local asset_dir=$1
    local asset_url=$2
    local asset_tgz=$(basename $asset_url)
    local asset_timestamp="$asset_dir/.timestamp"

    # Check if the timestamp is up to date
    [[ ! -n ${CI:-} ]] && check_timestamp "$asset_timestamp" && return

    echo "Fetch $asset_url"
    mkdir -p "$INPUT_DIR"
    ${WGET_CMD} "$asset_url" -O "$INPUT_DIR/$asset_tgz"

    echo "Extract $asset_tgz to $asset_dir"
    rm -rf "$asset_dir"
    mkdir -p "$(dirname "$asset_dir")"
    (cd "$(dirname "$asset_dir")" && tar -xf "$INPUT_DIR/$asset_tgz")

    save_timestamp "$asset_timestamp"
}

# Download and extract ZIP artifact from WEB location
function fetch_zip()
{
    local asset_dir=$1
    local asset_url=$2
    local asset_zip=$(basename $asset_url)
    local asset_timestamp="$asset_dir/.timestamp"

    # Check if the timestamp is up to date
    check_timestamp "$asset_timestamp" && return

    echo "Fetch $asset_url"
    mkdir -p "$INPUT_DIR"
    curl --fail -L -s --connect-timeout 10 "-#" "$asset_url" -o "$INPUT_DIR/$asset_zip"

    echo "Extract $asset_zip to $asset_dir"
    rm -rf "$asset_dir"
    mkdir -p "$(dirname "$asset_dir")"
    unzip -qbo "$INPUT_DIR/$asset_zip" -d "$asset_dir"

    save_timestamp "$asset_timestamp"
}

# Base URL to download staging asset ZIP files
ASSET_BASEURL="http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS"

# Download and extract the asset ZIP from the staging directory at ASSET_BASEURL
# Arguments:
# 1: local asset directory (absolute, or relative to ROOT_DIR)
# 2: remote asset path, relative to ASSET_BASEURL (for example: repo/mtb-pdl-cat1/develop/Latest/deploy/mtb-pdl-cat1.zip)
# 3: top-level directory name inside ZIP file (optional, only needed for configurator tools).
function fetch_asset()
{
    local asset_dir=$1
    local asset_addr=$2
    local zip_subdir=${3:-/} # by default, assume no nested sub-directory

    local asset_zip=$(basename "$asset_addr")
    local asset_http="$ASSET_BASEURL/$asset_addr"
    local asset_timestamp="$asset_dir/$zip_subdir/.timestamp"

    # Never remove existing git clone
    if [[ -e "$asset_dir/.git" ]]; then
        echo "Found existing git checkout at $asset_dir"
        echo "To bootstrap the $asset_addr artifact from asset directory:"
        echo rm -rf "$asset_dir"
        return
    fi

    # Check if the timestamp is up to date
    if [[ ! -n ${CI:-} ]]; then
        check_timestamp "$asset_timestamp" && return
    else
        echo "CI Build, bypassing timestamp check"
    fi

    # Check if the asset staging directory is accessible by performing HEAD request to base URL
    # Do not remove existing artifacts when offline
    if [[ ! -n ${CI:-} ]] && [[ -d "$asset_dir" ]] && ! ${WGET_CMD} -q --spider --tries 1 "$ASSET_BASEURL" >/dev/null; then
        echo "Skip $asset_dir download: $ASSET_BASEURL is not accessible"
        return
    fi

    local zip_deploy_regex='(.*)\/(.*)\/deploy\/(.*)'
    [[ $asset_http =~ $zip_deploy_regex ]] || { echo "$asset_http is not a valid asset deployment URL"; return 1; }
    local asset_http_prefix="${BASH_REMATCH[1]}"
    local asset_http_build="${BASH_REMATCH[2]}"
    local asset_http_suffix="${BASH_REMATCH[3]}"
    local asset_bld_num=$(${WGET_CMD} -q ${asset_http_prefix}/${asset_http_build}/_bld_info.txt -O - | head -1 | sed -e "s,.* ,,")
    local asset_url="${asset_http_prefix}/${asset_bld_num}/deploy/${asset_http_suffix}"
    # Replace the "Latest" in the zip file part of the URL
    if [[ $asset_http_build == Latest ]]; then
        asset_url=${asset_url/Latest/$asset_bld_num}
    fi

    if [[ -d "$asset_dir/$zip_subdir" ]]; then
        if [[ ! -f "$asset_dir/$zip_subdir/version.xml" ]]; then
            echo "$asset_addr asset time stamp out of date, downloading $asset_bld_num"
            rm -rf "$asset_dir/$zip_subdir"
            rm -f "$INPUT_DIR/$asset_zip"
        fi

        local asset_version_xml=$(< "$asset_dir/$zip_subdir/version.xml")
        local asset_version_regex="<version>[0-9]+\.[0-9]+\.[0-9]+\.${asset_bld_num}</version>"
        if [[ $asset_version_xml =~ $asset_version_regex ]]; then
            echo "$asset_addr asset version at $asset_dir is up to date: $asset_bld_num, skip asset download"
            save_timestamp "$asset_timestamp"
            return 0
        fi

        echo "$asset_addr asset version at $asset_dir is out of date, downloading $asset_bld_num"
        rm -rf "$asset_dir/$zip_subdir"
        rm -f "$INPUT_DIR/$asset_zip"
    fi

    echo "Fetch $asset_url"
    mkdir -p "$INPUT_DIR"
    ${WGET_CMD} "$asset_url" -O "$INPUT_DIR/$asset_zip"

    echo "Extract $asset_zip to $asset_dir"
    mkdir -p "$(dirname "$asset_dir")"
    unzip -qbo "$INPUT_DIR/$asset_zip" -d "$asset_dir"
    save_timestamp "$asset_timestamp"
}

# Download CAT1 PDL
function fetch_pdl_cat1()
{
    local branch=${1:-"rel/mtb-pdl-cat1-2.4.0"}
    local build=${2:-"Latest"}

    fetch_asset "$PDL_CAT1_DIR" "repo/mtb-pdl-cat1/${branch}/${build}/deploy/mtb-pdl-cat1.zip"
}

# Download CAT2 PDL
function fetch_pdl_cat2()
{
    local branch=${1:-"rel/mtb-pdl-cat2-1.6.0"}
    local build=${2:-"Latest"}

    fetch_asset "$PDL_CAT2_DIR" "repo/mtb-pdl-cat2/${branch}/${build}/deploy/mtb-pdl-cat2.zip"
}

# Download XMCLib (CAT3)
function fetch_xmclib()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$XMC_LIB_DIR" "repo/mtb-xmclib-cat3/${branch}/${build}/deploy/mtb-xmclib-cat3.zip"
}

# Download PSoC 6 CM0+ prebuilt images
function fetch_psoc6cm0p()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/psoc6cm0p" "repo/psoc6cm0p/${branch}/${build}/deploy/psoc6cm0p.zip"
}

# Download FreeRTOS
function fetch_freertos()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/freertos" "repo/freertos/${branch}/${build}/deploy/freertos.zip"
}

# Download USB Device middleware
function fetch_usbdev()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/usbdev" "repo/middleware-usbdev/${branch}/${build}/deploy/usbdev.zip"
}

# Download CapSense middleware
function fetch_capsense()
{
    local branch=${1:-"develop"}
    local build=${2:-"1545"} # Latest build compatible with MTB 2.3

    fetch_asset "$LIB_DIR/capsense" "repo/middleware-capsense/${branch}/${build}/deploy/capsense.zip"
}

# Download Core Library
function fetch_corelib()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/core-lib" "repo/bsp_csp/${branch}/${build}/deploy/libs/core-lib.zip"
}

# Download HAL and supplemental board libraries
function fetch_hal()
{
    local branch=${1:-"release/2022.q1"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/mtb-hal-cat1" "repo/bsp_csp/${branch}/${build}/deploy/libs/mtb-hal-cat1.zip"
    fetch_asset "$LIB_DIR/retarget-io" "repo/bsp_csp/${branch}/${build}/deploy/libs/retarget-io.zip"
    fetch_asset "$LIB_DIR/clib-support" "repo/bsp_csp/${branch}/${build}/deploy/libs/clib-support.zip"
    fetch_asset "$LIB_DIR/abstraction-rtos" "repo/bsp_csp/${branch}/${build}/deploy/libs/abstraction-rtos.zip"
}

# Download BSP asset to BSP_DIR
function fetch_bsp()
{
    local bsp=${1}
    local branch=${2:-"release/2022.q1"}
    local build=${3:-"Latest"}
    fetch_asset "$BSP_DIR/TARGET_$bsp" "repo/bsp_csp/${branch}/${build}/deploy/bsps/TARGET_$bsp.zip"
}

# Download BSP assets required by FUT/chaitea
function fetch_bsps()
{
    # Define the list of BSPs to download for testing
    BSP_LIST=(
        CYSBSYSKIT-DEV-01
    )

    for bsp in ${BSP_LIST[@]}; do
        fetch_bsp "$bsp" "$@"
    done
}

# Download ModusToolbox build system
function fetch_make()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/core-make" "repo/mtbide/make-infrastructure/develop/2249/deploy/core-make.zip"
    fetch_asset "$LIB_DIR/recipe-make-cat1a" "repo/mtbide/make-infrastructure/develop/2249/deploy/recipe-make-cat1a.zip"

    # tools-make versions match the specific ModusToolbox releases
    fetch_asset "$CY_TOOLS_DIR_2_4" "repo/mtbide/make-infrastructure/develop/2249/deploy/tools-make.zip" "make"
    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtbide/make-infrastructure/${branch}/${build}/deploy/tools-make.zip" "make"

    # tools-make>=2.0 depends on MtbQueryAPI tools
    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtbide/mtb-env/develop/414/deploy/mtbsearch-1.0.0.414-${CY_TOOLS_SUFFIX}.zip" "mtbsearch"

    # MtbQueryAPI tools check for modus-shell presence
    if [[ "$CY_TOOLS_SUFFIX" == "windows" ]]; then
        fetch_asset "$CY_TOOLS_DIR_3_0" "repo/modus-shell/master/65/deploy/modus-shell-1.3.0.65-windows.zip" "modus-shell"
    else
        fetch_tgz "$CY_TOOLS_DIR_3_0/modus-shell" "$ASSET_BASEURL/repo/modus-shell/master/65/deploy/modus-shell-1.3.0.65-linux.tar.gz"
        bash "$CY_TOOLS_DIR_3_0/modus-shell/postinstall"
    fi
}

# Download required ModusToolbox configurators from 2.4 release
function fetch_tools_2_4()
{
    # Use branch/build matching MTB 2.4 public release
    local branch="develop"
    local build="6117"

    # Skip tools download when running on Raspberry Pi host
    [[ $OSTYPE == linux-gnueabihf ]] && return

    fetch_asset "$CY_TOOLS_DIR_2_4" "repo/mtb-configurators/packager-configurators/${branch}/${build}/deploy/device-configurator-3.10.0.${build}-${CY_TOOLS_SUFFIX}.zip" "device-configurator"
}

# Download required ModusToolbox configurators from 3.0 nightly build
function fetch_tools_3_0()
{
    # Use branch/build matching MTB 3.0 nightly builds
    local branch="develop"
    local build="Latest"

    # Skip tools download when running on Raspberry Pi host
    [[ $OSTYPE == linux-gnueabihf ]] && return

    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtb-configurators/packager-configurators/${branch}/${build}/deploy/device-configurator-${CY_TOOLS_SUFFIX}.zip" "device-configurator"
    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtb-configurators/packager-configurators/${branch}/${build}/deploy/capsense-configurator-${CY_TOOLS_SUFFIX}.zip" "capsense-configurator"
    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtb-configurators/packager-configurators/${branch}/${build}/deploy/smartio-configurator-${CY_TOOLS_SUFFIX}.zip" "smartio-configurator"
    fetch_asset "$CY_TOOLS_DIR_3_0" "repo/mtb-configurators/packager-configurators/${branch}/${build}/deploy/seglcd-configurator-${CY_TOOLS_SUFFIX}.zip" "seglcd-configurator"
}

# Create the ModusToolbox 2.4 tools symlinks necessary for FUT debugging
function symlink_tools_2_4()
{
    # Create modus-shell symlink - Windows-specific
    # Note: on Windows (Cygwin) - "cygpath -w /" resolves to C:\cygwin64
    # Therefore, $OUT_DIR/tools_2.4/modus-shell/bin/bash executes C:\cygwin64\bin\bash.exe
    [[ $CY_TOOLS_SUFFIX == windows ]] && symlink_directory / "$CY_TOOLS_DIR_2_4/modus-shell"

    # Create GCC symlink - needed for interactive GDB debugging
    # of the exported FUT projects in the Eclipse and VSCode IDEs
    # Skip when running on Raspberry Pi host
    [[ $OSTYPE == linux-gnueabihf ]] || symlink_directory "$GCC_DIR" "$CY_TOOLS_DIR_2_4/gcc"
}

# Create the ModusToolbox 3.0 tools symlinks necessary for FUT debugging
function symlink_tools_3_0()
{
    # Create modus-shell symlink - Windows-specific
    # Note: on Windows (Cygwin) - "cygpath -w /" resolves to C:\cygwin64
    # Therefore, $OUT_DIR/tools_3.0/modus-shell/bin/bash executes C:\cygwin64\bin\bash.exe
    [[ $CY_TOOLS_SUFFIX == windows ]] && symlink_directory / "$CY_TOOLS_DIR_3_0/modus-shell"

    # Create GCC symlink - needed for interactive GDB debugging
    # of the exported FUT projects in the Eclipse and VSCode IDEs
    # Skip when running on Raspberry Pi host
    [[ $OSTYPE == linux-gnueabihf ]] || symlink_directory "$GCC_DIR" "$CY_TOOLS_DIR_3_0/gcc"
}

# Download OpenOCD tool to enable FUT/chaitea program/debug
function fetch_openocd_4_3()
{
    # Use branch/build matching MTB 2.4 public release
    local branch="icw/4.3"
    local build="1746"

    # Check if running on Raspberry Pi host
    if [[ $OSTYPE == linux-gnueabihf ]]; then
        # Copy ARM build of openocd from /opt into ModusToolbox tools directory
        mkdir -p "$CY_TOOLS_DIR_2_4"
        cp -r /opt/openocd-2.2 "$CY_TOOLS_DIR_2_4/openocd"
    else
        fetch_asset "$CY_TOOLS_DIR_2_4" "repo/cyopenocd/${branch}/${build}/deploy/openocd-4.3.0.${build}-${CY_TOOLS_SUFFIX}.zip" "openocd"
    fi
}

# Download OpenOCD tool to enable FUT/chaitea program/debug
function fetch_openocd_4_4()
{
    # Latest OpenOCD URLs can be found in mtb_installers deploy dir:
    # http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/mtb_installers/ModusToolbox_3.0.0/Latest/deploy/
    # OpenOCD asset deployment directory:
    # http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/cyopenocd/icw/4.4
    local branch="icw/4.4"
    local build="Latest"

    # Check if running on Raspberry Pi host
    if [[ $OSTYPE == linux-gnueabihf ]]; then
        # Copy ARM build of openocd from /opt into ModusToolbox tools directory
        mkdir -p "$CY_TOOLS_DIR_3_0"
        cp -r /opt/openocd-2.2 "$CY_TOOLS_DIR_3_0/openocd"
    else
        fetch_asset "$CY_TOOLS_DIR_3_0" "repo/cyopenocd/${branch}/${build}/deploy/openocd-4.4.0.${build}-${CY_TOOLS_SUFFIX}.zip" "openocd"
    fi
}

function fetch_cmsis_dsp()
{
    # Note: keep the version in sync with test/Makefile, SEARCH_cmsis_dsp
    local release_tag=${1:-"v1.10.1"}
    local release_url="https://github.com/ARM-software/CMSIS-DSP/archive/refs/tags/$release_tag.zip"
    fetch_zip "$LIB_DIR/cmsis-dsp" "$release_url"
}

# cmsis_dsp uses some headers from cmsis_5
function fetch_cmsis_5()
{
    # Note: keep the version in sync with test/Makefile, SEARCH_cmsis_dsp
    local release_tag=${1:-"5.9.0"}
    local release_url="https://github.com/ARM-software/CMSIS_5/archive/refs/tags/$release_tag.zip"
    fetch_zip "$LIB_DIR/cmsis-5" "$release_url"
}

function fetch_sensor_dsp()
{
    local branch=${1:-"develop"}
    local build=${2:-"Latest"}

    fetch_asset "$LIB_DIR/sensor-dsp" "pss/sensys/csk/sensor-dsp/${branch}/${build}/deploy/sensor-dsp.zip"
}

# Copy A_Clang-specific libraries and scripts to $LIB_DIR
function copy_aclang()
{
    # Check if running under macOS
    if [[ $KERNEL == Darwin* ]]; then
        # AppleClang C runtime pulled by downloaddeps.sh
        export ACLANG_LIBS_DIR="$CY_DEP_DIR/aclang-libs-0.3"
        # Check if the library files are not yet copied
        if [[ -d "$LIB_DIR/TOOLCHAIN_A_Clang" ]]; then
            echo "$LIB_DIR/TOOLCHAIN_A_Clang already exists"
        else
            cp -rv "$ACLANG_LIBS_DIR/TOOLCHAIN_A_Clang" "$LIB_DIR/"
            cp -rv "$ACLANG_LIBS_DIR/psoc6make/make/scripts/m2bin" "$LIB_DIR/recipe-make-cat1a/make/scripts/m2bin"
        fi
    fi
}

# Extract output/<asset>.zip as output/<asset>
function extract_artifacts()
{
    [[ -f "$PKG_OUT_ZIP" ]] || { echo "ERROR: artifact $PKG_OUT_ZIP does not exist"; exit 1; }

    # Override ASSET_LIB_DIR to point to extracted asset
    export ASSET_LIB_DIR="$PKG_OUT_DIR"

    echo "Extract $PKG_OUT_ZIP to $PKG_OUT_DIR"
    mkdir -p "$OUT_DIR"
    unzip -qbo "$PKG_OUT_ZIP" -d "$PKG_OUT_DIR"
}

# Configure Python virtual environment
function setup_python_env()
{
    # Check if running on Raspberry Pi host
    if [[ $OSTYPE == linux-gnueabihf ]]; then
        echo "Running on raspberry pi; skipping python virtualenv installation"
        export PYTHON=python3
        "$PYTHON" --version
        return
    fi

    # Set path to the python environment
    local env_dir="$OUT_DIR/env"

    # Check if 'pip install' is needed
    # Skip on consecutive invocations to speedup local build
    # Can be overridden by FORCE_PIP_INSTALL=1 !!
    local need_pip_install=${FORCE_PIP_INSTALL:=0}

    # Create and activate the environment
    # Note: under Windows use virtualenv, under UNIX - venv
    case "$KERNEL" in
        CYGWIN*|MINGW*|MSYS*)
            # Check if the host system lacks virtualenv
            # assume all required modules are pre-installed
            if ! "$PYTHON" -m virtualenv --version; then
                echo "Warning: virtualenv module is not found"
                echo "Virtual environment was not created, pre-install all packages manually"
                return
            fi

            if ! [[ -d "${env_dir}" ]]; then
                echo "Creating python virtual environment in ${env_dir}"
                "$PYTHON" -m virtualenv "${env_dir}"
                need_pip_install=1
            fi
            source "${env_dir}/Scripts/activate"
            ;;
        Linux*|Darwin*)
            if ! [[ -d "${env_dir}" ]]; then
                echo "Creating python virtual environment in ${env_dir}"
                "$PYTHON" -m venv "${env_dir}"
                need_pip_install=1
            fi
            [[ -d "${env_dir}" ]] || "$PYTHON" -m venv "${env_dir}"
            set +u # activate script uses unreferenced variables
            source "${env_dir}/bin/activate"
            set -u
            ;;
        *)
            echo >&2 "ERROR: unsupported OS: $KERNEL"
            exit 1
            ;;
    esac

    if [[ ${need_pip_install} -eq 1 ]]; then
        # Update pip, setuptools and wheel
        python -m pip install --upgrade pip setuptools wheel

        # Install required python modules
        python -m pip install --upgrade -r "$SCRIPTS_DIR/requirements.txt"
    fi

    # Once the environment is activated, the 'python' binary always points to env_dir
    export PYTHON=python
    "$PYTHON" --version
}

# Install cysecuretools into the python environment
function setup_cysecuretools()
{
    # Skip cysecuretools installation when running on Raspberry Pi host
    [[ $OSTYPE == linux-gnueabihf ]] && return

    # Check the python virtual environment is loaded to avoid system-wide install
    if ! [[ -n ${VIRTUAL_ENV:-} ]]; then
        echo "Warning: virtual environment was not created, cysecuretools package installation skipped"
        return
    fi

    # Define the package version as seen reported by "pip freeze"
    local cysecuretools_version=3.1.0

    # Install cysecuretools module, in case the correct version is not yet installed
    # Checking the package version by grepping "pip show" output
    if ! "$PYTHON" -m pip show cysecuretools 2>/dev/null | grep "^Version:" | grep -q "$cysecuretools_version"; then
        # true: install official package from pypi.org
        # false: install directly from gitlab development repo
        local use_pypi=true

        if [[ $use_pypi == true ]]; then
            echo "Installing cysecuretools-$cysecuretools_version from pypi.org"
            "$PYTHON" -m pip install --upgrade "cysecuretools==$cysecuretools_version"
        else
            # Use fixed tag to avoid sudden breakage of bsp_csp CI due to incompatible cysecuretools changes
            # Lookup the recent tags: http://devops-git.aus.cypress.com/repo/cysecuretools/-/tags
            local cysecuretools_url=http://devops-git.aus.cypress.com/repo/cysecuretools.git
            local cysecuretools_tag=CYSECURETOOLS_3.0.0_MTB_INTEGRATION # ROMR-84

            # cysecuretools dev package requires new setuptools/pip
            "$PYTHON" -m pip install --upgrade setuptools pip

            echo "Installing cysecuretools#$cysecuretools_tag from $cysecuretools_url"
            "$PYTHON" -m pip install --upgrade git+$cysecuretools_url@$cysecuretools_tag
        fi
    fi
}
