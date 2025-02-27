#!/bin/bash

# Enable strict error checking
set -$-ue${DEBUG+xv}

# Source asset build environment
build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/../setenv.sh"

UNCRUSTIFY_BRANCH=${UNCRUSTIFY_BRANCH:-"develop"}
UNCRUSTIFY_BUILD=${UNCRUSTIFY_BUILD:-"17"}
UNCRUSTIFY_DIR="$OUT_DIR/uncrustify"

echo "Uncrustify Branch: ${UNCRUSTIFY_BRANCH}"
echo "Uncrustify Build: ${UNCRUSTIFY_BUILD}"

case "$(uname -s)" in

   Darwin)
    # Mac OS X
    fetch_asset "$UNCRUSTIFY_DIR" \
        "repo/uncrustify/${UNCRUSTIFY_BRANCH}/${UNCRUSTIFY_BUILD}/deploy/uncrustify_mac.zip"
    uncrustify_exe="$UNCRUSTIFY_DIR/uncrustify"
    ;;

   Linux)
    # Linux
    fetch_asset "$UNCRUSTIFY_DIR" \
        "repo/uncrustify/${UNCRUSTIFY_BRANCH}/${UNCRUSTIFY_BUILD}/deploy/uncrustify_linux.zip"
    uncrustify_exe="$UNCRUSTIFY_DIR/uncrustify"
    ;;

   CYGWIN*|MINGW32*|MINGW64*|MSYS*)
    # Windows
    fetch_asset "$UNCRUSTIFY_DIR" \
        "repo/uncrustify/${UNCRUSTIFY_BRANCH}/${UNCRUSTIFY_BUILD}/deploy/uncrustify_win.zip"
    uncrustify_exe="$UNCRUSTIFY_DIR/uncrustify.exe"
    ;;

   *)
    echo "Unsupported OS - $(uname -s)"
    exit 1
    ;;
esac

excludeList=$build_dir/uncrustify_exclude.txt
file_list_normal=$UNCRUSTIFY_DIR/processed_files.txt
file_list_no_func_header=$UNCRUSTIFY_DIR/processed_files_no_func_header.txt

if [ ! -s $excludeList ]; then
    echo "ERROR: $excludeList is missing or empty"
    exit 1
fi

# Get a list of all *.c and *.h files
find . -name '*.[ch]' -print > $file_list_normal

if [ ! -s $file_list_normal ]; then
    echo "ERROR: No *.c or *.h files to process"
    exit 1
fi

while read line; do
    # Format special characters in the exclude list file where they can be used by sed
    line=${line//[$'\r\n']}
    line=${line// /\\\ }
    line=${line//\./\\\.}
    line=${line//\//\\/}

    sed -i.bak "/$line/d" $file_list_normal
done < $excludeList

# Increase verbosity
set -x

grep "\.\/test\/sut" $file_list_normal > $file_list_no_func_header
sed -i.bak "/\.\/test\/sut\/hal/d" $file_list_normal

# Switch to bsp_csp__no_align.cfg to add all missing function headers
"$uncrustify_exe" -c $build_dir/bsp_csp__no_align_no_header.cfg \
    --no-backup -F $file_list_normal

"$uncrustify_exe" -c $build_dir/bsp_csp__no_align_no_header.cfg \
    --no-backup -F $file_list_no_func_header

if ! git diff --exit-code --diff-filter=d --color; then
    echo "ERROR: Uncrustify changes detected"
    exit 1
fi
