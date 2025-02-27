#!/bin/bash

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -$-ue${DEBUG+xv}

echo "GCC_DIR=${GCC_DIR:-}"
echo "IAR_DIR=${IAR_DIR:-}"
echo "ARMCC_DIR=${ARMCC_DIR:-}"

# CI pipeline: extract the artifact from the build stage
[[ -n ${CI:-} ]] && extract_artifacts

# Download third-party toolchains
download_deps

# Download MTB tools (3.0 nightly build)
# Comment out in case the FUT projects do not use custom design.modus
fetch_tools_2_4
symlink_tools_2_4

# Download OpenOCD (for "make program")
fetch_openocd_4_3

# Download and extract the dependent assets
fetch_pdl_cat1
fetch_psoc6cm0p
#fetch_freertos
fetch_corelib
fetch_hal
fetch_bsps
fetch_make
fetch_cmsis_dsp
fetch_sensor_dsp

# Switch to the repo root - to enable relative fut_path
cd "$ROOT_DIR"

function usage()
{
    cat <<EOF
Usage: $0 [ -g | --gen ] [ -d | --device ] <fut_path> <make_args>
  <fut_path>  - path to the FUT directory (relative from test/fut)
  <make_args> - arguments passed to make tool

Examples:
Update FUT design.modus GeneratedSource:
$0 -g fut01
$0 -gen fut01

Open design.modus in Device Configurator:
$0 -d fut01
$0 --device fut01

Build the FUT project:
$0 fut01 build

Program the FUT project:
$0 fut01 program

Export FUT project to Eclipse:
$0 fut01 eclipse

Export FUT project to VSCode:
$0 fut01 vscode

Export FUT project to Keil uVision 5:
$0 fut01 uvision5

Export FUT project to IAR EWARM 8:
$0 fut01 ewarm8

EOF
    exit 1
}

function test_fut()
{
    local help_arg=
    local gen_arg=
    local device_arg=
    local fut_path=
    local make_args=

    [[ $# -eq 0 ]] && usage

    echo "${BASH_SOURCE[0]}" $@

    # parse command line inputs
    while (( $# > 0 )); do
        case "$1" in
            -h | --help)
                usage
                ;;
            -g | --gen)
                gen_arg=true
                ;;
            -d | --device)
                device_arg=true
                ;;
            -*)
                usage
                ;;
            *)
                fut_path=$1
                shift
                make_args=$@
                break
                ;;
        esac
        shift
    done

    # Exporting to Keil uVision: set TOOLCHAIN=ARM
    [[ $make_args == *"uvision5"* ]] && make_args="$make_args TOOLCHAIN=ARM"

    # Exporting to EWARM: set TOOLCHAIN=IAR
    [[ $make_args == *"ewarm8"* ]] && make_args="$make_args TOOLCHAIN=IAR"

    # fut_path can be either absolute, or relative to test/fut or ROOT_DIR
    [[ -d $fut_path ]] || fut_path="test/fut/$fut_path"
    [[ -d $fut_path ]] || fut_path="$ROOT_DIR/$fut_path"
    [[ -d $fut_path ]] || { echo "ERROR: directory $fut_path doesn't exist"; exit 1; }

    # Check if the TARGET= is supplied in make_args (needed for config and gen_config targets)
    local target_arg=$(echo $make_args | tr ' ' '\n' | grep "TARGET=" || true)

    if [[ -n $gen_arg ]]; then
        echo "Update $fut_path GeneratedSource"
        find "$fut_path" -type d -name GeneratedSource -prune -exec rm -rf '{}' '+'
        (set -x; make -C $fut_path gen_config VERBOSE=1 $target_arg)
    fi

    if [[ -n $device_arg ]]; then
        echo "Edit $fut_path design.modus"
        (set -x; make -C $fut_path config VERBOSE=1 $target_arg)
    fi

    if [[ -n $make_args ]]; then
        if ! (set -x; make -C "$fut_path" $make_args); then
            FAILURE_COUNT=$((FAILURE_COUNT+1))
            FAILURE_LIST+="make -C $fut_path $make_args\n"
        fi
    else
        echo $'\n'$'\n'"To build the $fut_path test case, execute:"
        echo "make -C $fut_path build -j"
    fi
}

FAILURE_COUNT=0
FAILURE_LIST=""

if [[ $# -eq 0 ]]
then
    # Script executed without arguments: execute complete test suite
    test_fut -g fut01 # update GeneratedSource
    test_fut fut01 build TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=GCC_ARM CONFIG=Debug
    test_fut fut01 build TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=GCC_ARM CONFIG=Release
else
    # Execute single test case, as defined by the command line arguments
    test_fut "$@"
fi

if [[ $FAILURE_COUNT -eq 0 ]]; then
    echo "test_fut: SUCCESS"
else
    echo $'\n'$'\n'"test_fut: ${FAILURE_COUNT} failures:"
    echo -e $FAILURE_LIST
fi
exit $FAILURE_COUNT
