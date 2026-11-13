#!/bin/bash
# Prints the dev build's VERSION (<base>.<n>) and, for the first build of a bump, NEW_MARKER (dev-v<base>), as
# lines for $GITHUB_ENV. dev-build.yml says what the base and n are. Reads DEV_VERSION from the environment; run
# in a checkout with its history and tags.
set -euo pipefail

later() {   # true when version $1 comes after version $2
    [ "$1" != "$2" ] && [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | tail -1)" = "$1" ]
}

release_tag=$(git describe --tags --abbrev=0 --match 'CoE5SE-v[0-9]*.[0-9]*.[0-9]*' HEAD)
release=${release_tag#CoE5SE-v}
base=$release
since=$release_tag
marker=""
if [ -n "${DEV_VERSION:-}" ]; then
    if ! [[ "$DEV_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        echo "DEV_VERSION is '$DEV_VERSION': give three numbers, like 3.6.3" >&2
        exit 1
    fi
    if later "$DEV_VERSION" "$release"; then
        base=$DEV_VERSION
        if git rev-parse -q --verify "refs/tags/dev-v$base" > /dev/null; then
            since="dev-v$base"
        else
            since=HEAD
            marker="dev-v$base"
        fi
    fi
fi
n=$(git rev-list --count "$since..HEAD")
echo "dev build: last release $release, base $base, $n since $since" >&2
echo "VERSION=$base.$n"
if [ -n "$marker" ]; then echo "NEW_MARKER=$marker"; fi
