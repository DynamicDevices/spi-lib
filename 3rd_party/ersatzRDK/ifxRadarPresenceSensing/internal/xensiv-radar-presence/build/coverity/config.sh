# Asset-specific coverity configuration.
# The bash variables defined in this file are loaded to the main script: run_coverity.sh
#
# For details, refer to Coverity Quick Start Guide in the ModusToolbox Knowledge Hub:
# https://confluencewikiprod.intra.infineon.com/x/1KaoEw

# Set the project name in the Coverity Connect server.
# Typically, coverity project name matches the GitHub asset name,
# as defined by the $PKG_NAME variable in build/setenv.sh.
# As a fallback, use GitLab repository name (CI_PROJECT_NAME) set by CI pipeline
cov_project="${PKG_NAME:-$CI_PROJECT_NAME}"
echo "cov_project: $cov_project"

# Determine the current git repository branch from "git rev-parse" output.
# GitLab CI variable CI_COMMIT_REF_NAME always overrides the git command:
# https://docs.gitlab.com/ee/ci/variables/predefined_variables.html
cov_branch="${CI_COMMIT_REF_NAME:=$(git rev-parse --abbrev-ref HEAD)}"
echo "cov_branch: $cov_branch"

# Set the stream name in the Coverity Connect server.
# Each asset mainline branch should use a separate stream
#
# The stream name is produced by combining the project name and branch name
# The '/' symbols are replaced with '-'.
# For example: analysis results for project mtb-pdl-cat2, branch rel/1.0.0
# will be uploaded to coverity stream "mtb-pdl-cat1-rel-1.0.0"
#
# Note: stream uploading is enabled only for the mainline branches,
# as the coverity server limits the total number of the project streams.
# !!! Do NOT enable coverity upload for temporary sandbox branches !!!
#
# Mainline branches specification:
# - develop branch - primary development mainline
# - rel/* branches - release stabilization
#
# - coverity/* branches - to debug issues related to defects reported by Coverity Connect server
#   upload to the dedicated $cov_project-coverity-debug stream (common for all coverity/* branches)
#
if [[ ${cov_branch} = "develop" ]] || [[ ${cov_branch} = "rel/"* ]]; then
    cov_stream="$cov_project-${cov_branch//\//-}"
elif [[ ${cov_branch} = "coverity/"* ]]; then
    cov_stream="$cov_project-coverity-debug"
else
    cov_stream= # disable stream upload
fi

# Disable stream upload by clearing cov_stream variable
# Comment out the below line once the outstanding coding issues are addressed
cov_stream=

# Set the component map: https://jira.cypress.com/browse/DEVOPS-1373
# https://confluence.cypress.com/display/IOTSCM/Template+Request+for+Coverity+backend+support
# The argument passed to devops_scripts/coverity/create_stream.sh
cov_component_map="--set component-map:mtb-asset"

# Set the default MISRA standard config.
# The JSON config should exist in the script directory.
# Standard MISRA configs are provided with the Coverity tool installation:
# $COVERITY_DIR/config/coding-standards/misrac2012
#
# To use the standard MISRA 2012 config instead of the custom config, copy
# one of the provided MISRA configs to the script directory:
#
# - misrac2012-advisory-only.config
# - misrac2012-all.config
# - misrac2012-mandatory-advisory.config
# - misrac2012-mandatory-only.config
# - misrac2012-mandatory-required.config
# - misrac2012-required-advisory.config
# - misrac2012-required-only.config
#
misra_config=misrac2012-custom.config

# Set the source filters to include/exclude sources
# cov_source_filter and cov_exclude_filter use different syntax, need to be kept in sync

# This filter is used by cov-analyze tool (--tu-pattern)
# Translation unit pattern matching syntax reference:
# http://iot-coverity-01.aus.cypress.com:8080/doc/en/cov_command_ref.html#cme_patterns
#cov_source_filter="! file('\/output\/') && ! file('\/test\/')"
cov_source_filter="! file('\/output\/')"

# This filter is used by cov-format-errors tool (--exclude-files)
# Regular expression syntax reference:
# http://iot-coverity-01.aus.cypress.com:8080/doc/en/cov_command_ref.html#cov-format-errors
#cov_exclude_filter='/output/.*|/test/.*'
cov_exclude_filter='/output/.*'
