# Spellchecker CI job

## Basic usage

1. Run spellchecker script from the asset root directory:

    ./build/spellcheck/spellcheck.sh

2. Review the test reports:

    cat output/test/spellcheck.log
    cat output/test/spellcheck.txt

3. Add false positives from output/test/spellcheck.txt to build/spellcheck/asset.txt

4. Fix remaining issues

5. Run spellchecker script again, confirm no issues reported (.log and .txt are empty)


## Advanced usage

### Tweak directory/file patterns included in spellchecker scan

Edit search_dirs and search_files in spellcheck.sh

### Check new spelling issues introduced in the sandbox branch:

    CI=1 ./build/spellcheck/spellcheck.sh

(this mode is used by GitLab CI acceptance test pipeline)

### Scan arbitrary directories

    ./build/spellcheck/spellcheck.sh ../paths/to/directories ../to/scan

## Additional information

Reference documentation available in ModusToolbox Knowledge Hub:

https://confluencewikiprod.intra.infineon.com/display/MTBKH/Spellchecker+CI+job
