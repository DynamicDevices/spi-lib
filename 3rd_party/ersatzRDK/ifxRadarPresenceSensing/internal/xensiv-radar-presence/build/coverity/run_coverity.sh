#!/bin/bash

# The scripts in this directory are based on template maintained by DevOps:
# http://devops-git.aus.cypress.com/devops/templates
#
# Latest DevOps documentation:
# https://confluencewikiprod.intra.infineon.com/x/LAyjHQ
#
# Coverity Quick Start Guide in the ModusToolbox Knowledge Hub:
# https://confluencewikiprod.intra.infineon.com/x/1KaoEw

# This build would be performed as:
#   ./build/coverity/run_coverity.sh <misra_config>
# This build would result in performing a
# build (using a previously installed compiler)
# and if the <cov_stream> is defined in config.sh, it would post these results

###
set -$-e${DEBUG+xv}
build_dir="$(dirname "${BASH_SOURCE[0]}")/.."
source "$build_dir/setenv.sh"
source "$build_dir/coverity/config.sh"
cd "$ROOT_DIR"

restore_xtrace="set +x"
test -o xtrace && restore_xtrace="set -x"

# Allow overriding of the MISRA config by passing the optional argument
[[ $# -ge 1 ]] && misra_config=$1

COVERITY_CODING_STANDARD=./build/coverity/$misra_config
echo "COVERITY_CODING_STANDARD: $COVERITY_CODING_STANDARD"

[[ -f $COVERITY_CODING_STANDARD ]] || { echo "$COVERITY_CODING_STANDARD doesn't exist"; exit 1; }

# http://iot-coverity.aus.cypress.com:8080/doc/en/cov_command_ref.html#cov-build
coverity_build_args=( \
    "--return-emit-failures"
)

# http://iot-coverity.aus.cypress.com:8080/doc/en/cov_command_ref.html#cov-analyze
coverity_analyze_args=( \
    "--all"
    "-en ENUM_AS_BOOLEAN"
    "-en HFA"
    "-n SECURE_CODING"
    "-n MISRA_CAST"
    "--one-tu-per-psf false"
)

set -x

# Download/update devops_scripts
if [[ -d devops_scripts/.git ]]; then
    cd devops_scripts
    git fetch origin && git merge --ff-only origin/master || true
    cd - >/dev/null
else
    rm -rf devops_scripts
    git clone --depth=1 http://devops-git.aus.cypress.com/devops/devops_scripts.git
fi
chmod 0600 devops_scripts/coverity_preco_user_prod_auth_key.txt

case "$KERNEL" in
    CYGWIN*|MINGW*|MSYS*|Darwin*)
        # Windows/macOS: assume the coverity tool is already installed to COVERITY_DIR
        coverity_bin_path="$COVERITY_DIR/bin"
        ;;
    Linux*)
        # Linux dependencies
        # Install GCC toolchain to shared cache
        devops_scripts/install_tool.sh git@devops-git.aus.cypress.com:devops/tools/ARM_GNU.git gcc_921 Linux64
        export GCC_DIR=$(pwd)/ARM_GNU/Linux64

        # Install Coverity tool
        coverity_bin_path="./Coverity/Linux64/bin"
        time devops_scripts/install_tool.sh git@devops-git.aus.cypress.com:devops/tools/Coverity.git coverity_2021_9_0_linux Linux64
        cp -pf devops_scripts/coverity_license.dat ${coverity_bin_path}/license.dat
        ;;
esac

${restore_xtrace}

# Delete the results of the previous analysis runs
rm -rf _coverity_ deploy/coverity coverity_defects*.*

set -x
"${coverity_bin_path}"/cov-configure --config _coverity_/coverity_config.xml --template --comptype gcc --compiler arm-none-eabi-gcc

"${coverity_bin_path}"/cov-configure --config _coverity_/coverity_config.xml --list-configured-compilers text

# Delete SUT artifacts directory to avoid incremental build
rm -rf "$OUT_DIR/test"

# SUT Makefile logic is different in CI pipeline and developer build.
# Unset 'CI' variable to ensure the asset sources are found in the repo root
# instead of ./output/<asset> (as extracted from output/<asset>.zip)
#
# Also, set COVERITY variable to enable coverity-specific CFLAGS
#
# See https://community.synopsys.com/s/article/How-to-write-a-function-model-to-eliminate-a-false-positive-in-a-C-applilcation
# Added to avoid Dereference after null check (FORWARD_NULL) false positives
#
time "${coverity_bin_path}"/cov-make-library \
    --compiler-opt -I --compiler-opt "${coverity_bin_path}"/../library \
    --output-file $build_dir/coverity/modelassert.xmldb $build_dir/coverity/modelassert.c

time "${coverity_bin_path}"/cov-build \
    --config  _coverity_/coverity_config.xml \
    --dir     _coverity_/analyze-data \
    ${coverity_build_args[*]} \
    env -u CI COVERITY=1 $build_dir/test_sut.sh coverity.ini

time "${coverity_bin_path}"/cov-analyze \
    --dir     _coverity_/analyze-data \
    --user-model-file $build_dir/coverity/modelassert.xmldb \
    --force \
    --strip-path "$ROOT_DIR/" \
    ${coverity_analyze_args[*]} \
    --tu-pattern "${cov_source_filter}" \
    --coding-standard-config $COVERITY_CODING_STANDARD \
    --ignore-deviated-findings

${restore_xtrace}

echo "Coverity logs"
mkdir -p deploy/coverity
defects_all_txt=coverity_defects_all.txt
defects_all_csv=coverity_defects_all.csv
defects_ignore=coverity_defects_ignore.csv

"$coverity_bin_path"/cov-format-errors \
    --dir _coverity_/analyze-data \
    --emacs-style \
    --exclude-files ${cov_exclude_filter} \
    &>$defects_all_txt

# Upload the analysis results to the Coverity Connect project stream
# Check if the cov_stream was defined for the current branch in config.sh
if [[ -n ${cov_stream} ]]; then
    echo "cov_stream: $cov_stream"
    unset http_proxy
    unset https_proxy
    bash -eu devops_scripts/coverity/create_project.sh  "${coverity_bin_path}" ${cov_project}
    bash -eu devops_scripts/coverity/create_stream.sh   "${coverity_bin_path}" ${cov_project} ${cov_stream} ${cov_component_map}
    bash -eu devops_scripts/coverity/update_stream.sh   "${coverity_bin_path}" ${cov_stream} ${cov_component_map}
    bash -eu devops_scripts/coverity/commit_defects.sh  "${coverity_bin_path}" ${cov_stream}  _coverity_/analyze-data

    devops_dir=./devops_scripts
    coverity_host="--host iot-coverity.aus.cypress.com"
    coverity_port=""

    "${coverity_bin_path}"/cov-manage-im ${coverity_host} ${coverity_port}  \
        --auth-key-file ${devops_dir}/coverity_preco_user_prod_auth_key.txt  \
        --mode defects --show --stream ${cov_stream} \
        --fields cid,severity,checker,file,function,stream-name --output-file $defects_all_csv

    "${coverity_bin_path}"/cov-manage-im  ${coverity_host} ${coverity_port}  \
        --auth-key-file ${devops_dir}/coverity_preco_user_prod_auth_key.txt  \
        --mode defects --show --stream ${cov_stream} \
        --action Ignore --fields cid,severity,checker,file,function,stream-name --output-file $defects_ignore
fi

# Copy the artifacts produced by cov-format-errors and cov-manage-im to deploy directory
cp -v coverity_defects*.* deploy/coverity
# Copy the artifacts produced by cov-analyze to deploy directory
find _coverity_/analyze-data/output \( -name "deviations*.txt" -o -name "summary.txt" \) -exec cp -v {} deploy/coverity \;

# Check if the coverity defect list is empty
if [[ -s "deploy/coverity/coverity_defects_all.txt" ]]; then
    echo "FAIL: Found Defect(s)"
    # Display all coverity defects
    cat deploy/coverity/coverity_defects_all.txt
    # Uncomment to report the failure status to acceptance test pipeline
    #exit 1
else
    echo "SUCCESS"
    exit 0
fi
