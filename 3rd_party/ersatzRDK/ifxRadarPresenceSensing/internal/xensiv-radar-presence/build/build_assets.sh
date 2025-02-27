#!/bin/bash
#
# Copy files from a source path to an output directory, as defined by the packaging manifest
#
# The manifest contains 0 or more lines, each of the form sourcePattern:outputPath (where
#  sourcePattern uses standard shell globbing operators). All files which match the pattern will
#  be copied into the output path. The pattern is relative to sourceDir and the the output
#  is relative to out_dir.
#  A line prefixed with a '-' will remove the matching files from the specified directory
#  A line prefixed with '#' is ignored as a comment

build_dir="$(dirname "${BASH_SOURCE[0]}")"
source "$build_dir/setenv.sh"

set -$-ue${DEBUG+xv}

# This function copies the files from the source directory
# to the output directory, as defined by the packaging manifest
# Arguments:
# $1: source base directory
# $2: output base directory
# $3: path to the manifest
# $4: optional: is this a recursive call?
function perform_copy()
{
    local manifest=$1
    local src_dir=$2
    local out_dir=$3
    local is_recursive=${4:-}

    echo "[INFO] Processing ${manifest}"
    [[ -n ${is_recursive} ]] || rm -rf "${out_dir}"
    mkdir -p "${out_dir}"

    if [[ ! -f "${manifest}" ]] ; then
        echo "File ${manifest} does not exist, aborting."
        exit 1
    fi

    # Read the manifest line by line
    while IFS=$'\r' read -r line || [[ -n "${line}" ]]
    do
        # Skip empty and commented lines
        if [[ -n "${line}" ]] && [[ "${line}" != \#* ]]; then
            # Parse format: "${sourcePattern}:${destPath}"
            IFS=': ' read -r sourcePattern destPath <<<"$line"

            # Check if sourcePattern starts with "-":
            # "-xxx/yyy" removes ${out_dir}/xxx/yyy
            # "-*/yyy" removes ${out_dir}/*/yyy by wildcard
            if [[ "${sourcePattern}" == -* ]]; then
                # Remove leading "-" from sourcePattern
                sourcePattern=${sourcePattern#-}
                # Check if sourcePattern contains "*" wildcard
                if [[ "${sourcePattern}" =~ '*' ]]; then
                    echo "[INFO] Remove ${sourcePattern}"
                    eval rm -rf "${out_dir}"/${sourcePattern}
                else
                    # Check that sourcePattern exists in the output directory
                    if [[ -e "${out_dir}/${sourcePattern}" ]]; then
                        echo "[INFO] Remove ${sourcePattern}"
                        rm -rf "${out_dir}/${sourcePattern}"
                    elif [[ "${sourcePattern}" == *doxygen.md ]]; then
                        # Ignore missing doxygen documentation
                        echo "[WARNING] ${sourcePattern} doesn't exist"
                    else
                        echo "[ERROR] ${sourcePattern} doesn't exist, fix the asset manifest"; exit 1;
                    fi
                fi
            elif [[ "${sourcePattern}" == @* ]]; then
                # Remove leading "@" from sourcePattern
                sourcePattern=${sourcePattern#@}
                # Copy files per manifest referred by sourcePattern
                if [[ -n "${destPath}" ]]; then
                    local dest_dir="${out_dir}/${destPath}"
                else
                    local dest_dir="${out_dir}"
                fi
                mkdir -p "${dest_dir}"
                perform_copy "$(dirname ${manifest})/${sourcePattern}" "${src_dir}" "${dest_dir}" 1
            elif [[ -d "${src_dir}/${sourcePattern}" ]]; then
                # Copy directory recursively
                echo "[INFO] Copy ${sourcePattern}"
                # destPath is optional, use sourcePattern if not set
                if [[ -n "${destPath}" ]]; then
                    local dest_dir="${out_dir}/${destPath}"
                else
                    local dest_dir="${out_dir}/${sourcePattern}"
                fi
                mkdir -p "${dest_dir}"
                cp -r "${src_dir}/${sourcePattern}"/* "${dest_dir}"
            elif [[ -e "${src_dir}/${sourcePattern}" ]]; then
                # Copy single file
                echo "[INFO] Copy ${sourcePattern}"
                # destPath is optional, use sourcePattern if not set
                if [[ -n "${destPath}" ]]; then
                    local dest_file="${out_dir}/${destPath}"
                else
                    local dest_file="${out_dir}/${sourcePattern}"
                fi
                mkdir -p "$(dirname "${dest_file}")"
                cp -r "${src_dir}/${sourcePattern}" "${dest_file}"
            elif [[ "${sourcePattern}" =~ '*' ]]; then
                # Copy files by wildcard pattern
                echo "[INFO] Copy ${sourcePattern}"
                # destPath is optional, use sourcePattern if not set
                if [[ -n "${destPath}" ]]; then
                    local dest_dir="${out_dir}/${destPath}"
                else
                    local dest_dir="${out_dir}/$(dirname "${sourcePattern}")"
                fi
                mkdir -p "${dest_dir}"
                eval cp -r "${src_dir}"/${sourcePattern} "${dest_dir}"
            elif [[ "${sourcePattern}" == *docs ]]; then
                # Ignore missing doxygen documentation
                echo "[WARNING] ${sourcePattern} doesn't exist, skipped doxygen html documentation"
            else
                echo "[ERROR] ${src_dir}/${sourcePattern} doesn't exist, fix the asset manifest"
                return 1
            fi
        fi
    done < "${manifest}"
}

# This function creates the asset ZIP archive, with version.xml
# Arguments:
# $1: directory to package into the archive (output/${PKG_NAME})
# $2: path to the ZIP archive to save (output/${PKG_NAME}-${PKG_VERSION}.zip)
# $3: package version (1.0.0.10) - optional, to generate version.xml
# $4: directory to save version.xml (optional, defaults to $1)
# $5: list of the ZIP directory patterns to include (optional, defaults to '.')
function create_zip()
{
    local pkg_dir=$1
    local pkg_zip=$2
    local pkg_ver=${3:-}
    local version_dir=${4:-}
    local zip_patterns=${5:-.}

    [[ -d "${pkg_dir}" ]] || { echo "[ERROR] asset packaging directory ${pkg_dir} doesn't exist"; exit 1; }

    # Remove existing asset ZIP package
    rm -rf "$pkg_zip"

    # Create asset ZIP package
    echo "[INFO] Create ${pkg_zip} ${pkg_ver:-}"
    [[ -n "${pkg_ver}" ]] && echo "<version>${pkg_ver}</version>" > "${pkg_dir}/${version_dir}/version.xml"
    (cd "${pkg_dir}" && zip -rqX "${pkg_zip}" ${zip_patterns})
}

function build_assets()
{
    # Create asset directory at output/$PKG_NAME
    perform_copy "$SCRIPTS_DIR/manifests/$PKG_NAME.txt" "$ROOT_DIR" "$PKG_OUT_DIR"
    # Compress docs directory as output/$PKG_NAME-docs.zip
    create_zip "$PKG_OUT_DIR" "$OUT_DIR/$PKG_NAME-docs.zip" "$PKG_VERSION" "" "docs version.xml"
    # Remove XML documentation from the asset directory
    rm -rf "$PKG_OUT_DIR/docs/xml"
    # Compress asset directory as output/$PKG_NAME.zip
    create_zip "$PKG_OUT_DIR" "$PKG_OUT_ZIP" "$PKG_VERSION"
}

# Do not execute build_assets function when sourced by another script
if [[ "$0" == "$BASH_SOURCE" ]]; then
    build_assets $@
fi
