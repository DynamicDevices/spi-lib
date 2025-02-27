#!/bin/bash

# Spell checker based on PyEnchant
# https://doc.cypress.com/dms3/memodetails?memoNum=khso-42
# https://confluencewikiprod.intra.infineon.com/display/MTBKH/Spellchecker+CI+job

set -$-eu${DEBUG+xv}
build_dir="$(dirname "${BASH_SOURCE[0]}")/.."
source "$build_dir/setenv.sh"

# Create Python virtual environment
setup_python_env

# Install PyEnchant
"$PYTHON" -m pip list | grep pyenchant || "$PYTHON" -m pip install pyenchant

# Define the directories to scan
# when the script is executed with no arguments
search_dirs=(
    '.'
    'test/sut'
)

# Define the file patterns to scan
search_files=(
    '*.md'
    '*.c'
    '*.cpp'
    '*.h'
    '*.s'
    '*.S'
    '*.ld'
    '*.sct'
    '*.icf'
    '*.cypersonality'
)

report_log="$OUT_DIR/test/spellcheck.log"
report_txt="$OUT_DIR/test/spellcheck.txt"
user_dict="$SCRIPTS_DIR/spellcheck"

function spellcheck()
{
    local dirs=$*

    rm -f "$report_log" "$report_txt"
    mkdir -p "$OUT_DIR/test"
    echo "Run spell checker in directories:" $dirs
    cd "$ROOT_DIR" # search_dirs patterns are relative to ROOT_DIR
    set -x
    "$PYTHON" -u "$SCRIPTS_DIR/spellcheck/spellcheck.py" -p $dirs -l "$report_log" -t "$report_txt" -m "${search_files[@]}" -d "$user_dict"
    set +x
    cd - >/dev/null
}

function check_report()
{
    # Check if the txt log is empty
    if [[ -s "$report_txt" ]]; then
        echo
        echo "ERROR - found spelling issues:"
        cat "$report_log"
        echo
        echo "To suppress the false positives, add the below lines to build/spellcheck/asset.txt:"
        cat "$report_txt"
        exit 1
    else
        echo "SUCCESS"
    fi
}

if [[ $# -eq 0 ]]; then
    # No arguments: search in standard directories
    spellcheck "${search_dirs[@]}"
else
    # Custom arguments: search in defined directories
    spellcheck $*
fi

check_report
