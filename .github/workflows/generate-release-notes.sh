#!/bin/bash

set -e

python << END
try:
    with open('CHANGELOG.md', 'r') as file:
        changelogContent = file.read()

    unreleasedTitle = "## [Unreleased]"
    previousReleaseTitle = "$1"[1:]
    previousReleaseTitle = "## [%s]" % (previousReleaseTitle)
    startIndex = changelogContent.index(unreleasedTitle) + len(unreleasedTitle)
    endIndex = changelogContent.index(previousReleaseTitle, startIndex)

    changelogContent = changelogContent[startIndex:endIndex].strip()
    print(changelogContent)
except:
    pass
END

echo
echo "<details> <summary>Commit History</summary>"
echo
git log --pretty=format:"* %s (%h)" $1...HEAD
echo
echo "</details>"
