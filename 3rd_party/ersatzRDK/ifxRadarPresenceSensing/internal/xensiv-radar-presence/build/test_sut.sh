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

# Download and extract the dependent assets
fetch_hal
fetch_pdl_cat1
fetch_psoc6cm0p
fetch_corelib
fetch_make
fetch_bsps
fetch_cmsis_dsp
fetch_sensor_dsp
fetch_freertos

# Install the required python modules
setup_python_env
# Uncomment to enable testing of PSoC 64 targets
#setup_cysecuretools

# Copy AppleClang runtime libraries
copy_aclang

#rm -rf "$BSP_DIR"
mkdir -p "$BSP_DIR"

# Generate PSoC 6 BSP target
function generate_bsp_target_cat1a()
{
    local bsp=$1
    local family=$2
    local memory=$3
    local device=$4
    local pdl_bsp_dir="$PDL_CAT1_DIR/devices/COMPONENT_CAT1A/templates/COMPONENT_MTB"
    local bsp_dir="$BSP_DIR/TARGET_$bsp"

    [[ -d "$bsp_dir" ]] && { echo "BSP directory $bsp_dir already exists"; return; }
    cp -r "$pdl_bsp_dir" "$bsp_dir"

    # Remove redundant for device linker scripts and startup assembly
    find "$bsp_dir" ! \( -name *${family}*.* -o  -name *${memory}*.* -o -name *system*.* -o -type d  \) -delete

    # Create BSP target makefile
    echo "DEVICE=$device" > "$bsp_dir/$bsp.mk"
    echo "COMPONENTS+=CAT1A" >> "$bsp_dir/$bsp.mk"

    echo "Created $bsp_dir"
}

# Generate MXSv2 BSP target
function generate_bsp_target_cat1b()
{
    local bsp=$1
    local family=$2
    local memory=$3
    local device=$4
    local pdl_bsp_dir="$PDL_CAT1_DIR/devices/COMPONENT_CAT1B/templates/COMPONENT_MTB"
    local bsp_dir="$BSP_DIR/TARGET_$bsp"

    [[ -d "$bsp_dir" ]] && { echo "BSP directory $bsp_dir already exists"; return; }
    cp -r "$pdl_bsp_dir" "$bsp_dir"

    # Remove redundant for device linker scripts and startup assembly
    find "$bsp_dir" ! \( -name *${family}*.* -o  -name *${memory}*.* -o -name *system*.* -o -type d  \) -delete

    # Create BSP target makefile
    echo "DEVICE=$device" > "$bsp_dir/$bsp.mk"
    echo "COMPONENTS+=CAT1B" >> "$bsp_dir/$bsp.mk"

    echo "Created $bsp_dir"
}

# Generate PSoC 4 / PMG1 BSP target
function generate_bsp_target_cat2()
{
    local bsp=$1
    local family=$2
    local memory=$3
    local device=$4
    local pdl_bsp_dir="$PDL_CAT2_DIR/devices/templates/COMPONENT_MTB"
    local bsp_dir="$BSP_DIR/TARGET_$bsp"

    [[ -d "$bsp_dir" ]] && { echo "BSP directory $bsp_dir already exists"; return; }
    cp -r "$pdl_bsp_dir" "$bsp_dir"

    # Remove redundant for device linker scripts and startup assembly
    find "$bsp_dir" ! \( -name *${family}.* -o  -name *${memory}*.* -o -name *system*.* -o -type d  \) -delete

    # Create BSP target makefile
    echo "DEVICE=$device" > "$bsp_dir/$bsp.mk"
    echo "COMPONENTS+=CAT2" >> "$bsp_dir/$bsp.mk"

    echo "Created $bsp_dir"
}

# Generate XMC BSP target
function generate_bsp_target_cat3()
{
    local bsp=$1
    local subseries=$2
    local device=$3
    local linker=$4
    local pdl_bsp_dir="$XMC_LIB_DIR/CMSIS/Infineon/COMPONENT_${subseries}"
    local bsp_dir="$BSP_DIR/TARGET_$bsp"

    [[ -d "$bsp_dir" ]] && { [[ -n ${TEST_SUT_SILENT:-} ]] || echo "BSP directory $bsp_dir already exists"; return; }
    mkdir -p "$bsp_dir"/TOOLCHAIN_{ARM,GCC_ARM}
    cp -r "$pdl_bsp_dir"/Source/TOOLCHAIN_ARM/${linker}_ac6.sct "$bsp_dir"/TOOLCHAIN_ARM/${linker}.sct
    cp -r "$pdl_bsp_dir"/Source/TOOLCHAIN_GCC_ARM/${linker}.ld "$bsp_dir"/TOOLCHAIN_GCC_ARM/${linker}.ld

    # Create BSP target makefile
    echo "DEVICE=$device" > "$bsp_dir/$bsp.mk"
    echo "COMPONENTS+=CAT3" >> "$bsp_dir/$bsp.mk"

    echo "Created $bsp_dir"
}

if [[ $# -eq 0 ]]
then
    echo "Assets were downloaded to the output directory"
    echo
    echo "To run SUT regression per test/sut_make.ini, execute:"
    echo $0 sut_make.ini
    echo
    echo "To run single test case, go to repo root and execute:"
    echo "make -C test/sut/snippet TARGET=CYSBSYSKIT-DEV-01 CONFIG=Debug TOOLCHAIN=GCC_ARM build"
else
    # Set Makefile CFLAGS to treat all warnings as errors, enable LTO
    export WERROR=1
    export LTO=1
    "$PYTHON" -u "$ROOT_DIR/build/test_sut.py" $@
fi
