#!/bin/bash

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -$-ue${DEBUG+xv}

# Initialize environment variables used by the doxyfiles
export DOXY_DIR="$SCRIPTS_DIR/doxygen"
export LOG_DIR="$OUT_DIR/doxygen"

# Run doxygen tool, check for warnings/errors
function generate_docs_common()
{
    local resource=$1
    local conf=$2
    local out_dir=$3

    # Create directory for logs/reports
    mkdir -p "$LOG_DIR"

    echo "[INFO] generating $resource docs"
    rm -rf "$out_dir"
    mkdir -p "$out_dir"
    local doxy_log="$LOG_DIR/$resource.log"
    if ! doxygen "$DOXY_DIR/doxyfile_$conf.conf" > "$doxy_log"; then
        echo "[ERROR] $resource doxygen generation failed"
        echo "$doxy_log" contents:
        cat "$doxy_log"
        echo "report contents:"
        cat "$LOG_DIR/$resource.txt"
        exit 1
    fi

    # List doxygen warnings in case WARN_AS_ERROR was disabled in doxyfile
    [[ -s "$LOG_DIR/$resource.txt" ]] && cat "$LOG_DIR/$resource.txt"

    # Remove dir_* index files
    rm -f "$out_dir"/html/dir_*.html

    # Remove doxygen object databases
    rm -f "$out_dir"/doxygen_{entrydb,objdb}*.tmp

    # Copy redirect HTML file to output parent directory
    cp -v "$DOXY_DIR/api_reference_manual.html" "$out_dir/"
}

# First argument is the input file
# Second argument is the fixed up file
function fixup_readme()
{
    local input_readme=$1
    local fixed_up=$2

    sed "/api_reference_manual/d;\
           s/\`#include/#\`include/g;\
           s/\`cy_rslt_t\`/@ref cy_rslt_t/g;\
           s/docs\/html\///" "$input_readme" > "$fixed_up"
}

# Configure environment and run the doxygen tool
function generate_lib_docs()
{
    # Initialize environment variables used by doxyfile_<xxxx>.conf
    export LIB=$1
    local lib_dir=$2
    local dest_folder=$3
    local conf_type=$4
    local lib_name=$5

    export LIB_NAME="$lib_name"
    export LIB_DIR="$lib_dir"
    export LIB_README="$LIB_DIR/README.md"
    export LIB_MAINPAGE="$LIB_DIR/README.doxygen.md"
    fixup_readme "$LIB_README" "$LIB_MAINPAGE"
    export LIB_OUTPUT="$dest_folder"
    export PKG_VERSION="$PKG_VERSION"

    generate_docs_common "$LIB" "$conf_type" "$LIB_OUTPUT/docs"

    rm -rf "$LIB_MAINPAGE"

    # Cleanup the environment
    unset LIB
    unset LIB_NAME
    unset LIB_DIR
    unset LIB_MAINPAGE
    unset LIB_OUTPUT
    unset PKG_VERSION
}

# Only execute main function when not sourced by another script
if [[ "$0" == "$BASH_SOURCE" ]]; then
    download_deps # download doxygen tool
    generate_lib_docs "$PKG_NAME" "$ROOT_DIR" "$ROOT_DIR" "lib" "$LIB_NAME"
fi
