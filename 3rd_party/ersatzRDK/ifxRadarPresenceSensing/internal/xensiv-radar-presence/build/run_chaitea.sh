#!/bin/bash

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -$-eu${DEBUG+xv}

# The commit below is the latest stable point of the chaitea-core repo tested to work with this repo.
# Commit link: https://devops-git.aus.cypress.com/repo/chaitea-core/-/commit/ae3cdc09a556b5d37a72a9dda13c082e15d815b4
CT_CORE_COMMIT_ID=${CT_CORE_COMMIT_ID:-'ae3cdc09a556b5d37a72a9dda13c082e15d815b4'}

if [[ -n ${CI:-} ]]; then
    # If running on a pipeline then we want to
    # send email notifications to the following individuals
    export CI_CHAITEA_EMAIL="Volodymyr.Medvid@infineon.com"
fi

# Download third-party toolchains
download_deps

# Download MTB tools (2.4 release)
fetch_tools_2_4

export CY_TOOLS_PATHS=$CY_TOOLS_DIR_2_4

# Download OpenOCD (for "make program")
fetch_openocd_4_3

# Download and extract the dependent assets
fetch_pdl_cat1
fetch_pdl_cat2
fetch_xmclib
fetch_psoc6cm0p
fetch_corelib
fetch_hal
fetch_bsps
fetch_make

# Install the required python modules
setup_python_env

CT_ENABLE_CC=""
if [[ -n ${CI_PIPELINE_IID:-} ]] || [[ "${OS:-}" != Windows_NT ]]; then
CT_ENABLE_CC="--code-coverage"
fi

CT_TEST_PATH="$ROOT_DIR/test/chaitea"
CT_CORE_PATH="$CT_TEST_PATH/chaitea-core"

if [ ! -d $CT_CORE_PATH ]; then
    git clone http://devops-git.aus.cypress.com/repo/chaitea-core.git "test/chaitea/chaitea-core"
fi

pushd $CT_CORE_PATH
git fetch
git checkout $CT_CORE_COMMIT_ID
popd

# Asset specific folder/file locations
CT_OUTPUT_PATH="$CT_TEST_PATH/output"
CT_SUITES_PATH="$CT_TEST_PATH/suites"
CT_ASSET_LIST_PATH="$CT_TEST_PATH/asset-list.ini"

# The getlibs.bash script uses unbounded variables so setting
# +u to allow unbounded variables before invoking the python script.
set +u

"$PYTHON" -u "$CT_CORE_PATH/ci_utils/chaitea_ci.py" \
                --output "$CT_OUTPUT_PATH" \
                --test-root "$CT_SUITES_PATH" \
                --asset-list "$CT_ASSET_LIST_PATH" \
                --skip-getlibs \
                -s Template \
                $CT_ENABLE_CC \
                $@ ${CI_CHAITEA_CUSTOM_ARGS:-}
