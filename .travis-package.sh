if [ "$TRAVIS_PULL_REQUEST" = "false" ] && [ "$TRAVIS_BRANCH" = "master" ]; then
    GITDATE="`git show -s --date=short --format='%ad' | sed 's/-//g'`"
    GITREV="`git show -s --format='%h'`"

    export REV_NAME="3dsutils-${GITDATE}-${GITREV}"
    mkdir "$REV_NAME"

    cp 3dsutils.* "$REV_NAME"
    cp send-exec.py "$REV_NAME"

    cp license.txt "$REV_NAME"
    cp README.md "$REV_NAME"

    export ARCHIVE_NAME="${REV_NAME}.zip"
    zip -r "$ARCHIVE_NAME" "$REV_NAME"
fi
