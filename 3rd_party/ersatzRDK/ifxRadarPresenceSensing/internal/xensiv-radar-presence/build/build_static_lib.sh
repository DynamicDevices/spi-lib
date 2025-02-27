#!/bin/bash

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -$-ue${DEBUG+xv}

echo "GCC_DIR=${GCC_DIR:-}"
echo "IAR_DIR=${IAR_DIR:-}"
echo "ARMCC_DIR=${ARMCC_DIR:-}"

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

function build_static_lib()
{
    local make_args=$@

    (set -x; make -C build/static_build build $make_args)
}

if [[ $# -eq 0 ]]
then
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=GCC_ARM VFP_SELECT=softfp CONFIG=Release
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=GCC_ARM VFP_SELECT=hardfp CONFIG=Release
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=ARM VFP_SELECT=softfp CONFIG=Release
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=ARM VFP_SELECT=hardfp CONFIG=Release
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=IAR VFP_SELECT=softfp CONFIG=Release
    build_static_lib TARGET=CYSBSYSKIT-DEV-01 TOOLCHAIN=IAR VFP_SELECT=hardfp CONFIG=Release
else
    # Execute single test case, as defined by the command line arguments
    build_static_lib "$@"
fi
