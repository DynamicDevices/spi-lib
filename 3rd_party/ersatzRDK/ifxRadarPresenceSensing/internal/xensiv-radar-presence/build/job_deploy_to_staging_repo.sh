#! /bin/bash
#
# expected to be called by the .gitlab-ci.yml file
#

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -eu

if [[ -z ${CI:-} ]]; then
  echo "ERROR: this script is only intended to execute from CI pipeline"
  echo "To run locally, set the required environment variables:"
  echo "CI_PROJECT_PATH: development repo gitlab path (repo/middleware-xxx)"
  echo "CI_COMMIT_REF_NAME: development repo mainline branch (mt2p0)"
  echo "CI_PIPELINE_IID: asset build number - must exist on the staging directory"
  echo "Example:"
  echo "env CI=1 CI_PROJECT_PATH=repo/middleware-xxx CI_COMMIT_REF_NAME=$(git rev-parse --abbrev-ref HEAD) CI_PIPELINE_IID=yyy $0 --no-push"

  exit 1
fi

echo CI_PROJECT_PATH=$CI_PROJECT_PATH
echo CI_COMMIT_REF_NAME=$CI_COMMIT_REF_NAME
echo CI_PIPELINE_IID=$CI_PIPELINE_IID

# set the environment variables required by devops script
export ASSET=${CI_PROJECT_PATH}/${CI_COMMIT_REF_NAME}
export ASSET_BUILD=${CI_PIPELINE_IID}
export ASSET_VERSION=${PKG_MAJOR}.${PKG_MINOR}.${PKG_PATCH}.${CI_PIPELINE_IID}
export STAGING_REPO=repo-staging/${PKG_NAME}
export STAGING_BRANCH=${CI_COMMIT_REF_NAME}

export ASSET_ZIP_FILE=${PKG_NAME}.zip
devops_scripts/job_deploy_to_staging_repo.sh --no-rebase "$@"
