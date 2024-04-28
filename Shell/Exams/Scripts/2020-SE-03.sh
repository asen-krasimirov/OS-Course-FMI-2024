#!/bin/bash

[[ $# == 2 ]] || { echo 'script takes 2 args' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be dir' >&2; exit 2; }

[[ -d "$2" ]] || { echo '2nd arg should be dir' >&2; exit 3; }

tar -caf "tree.tar.xz" "${2}/tree"

newVersion="$(cat "${2}/version")"

displayNameVersion="$(basename "$2")-${newVersion}"

result="$(cat "${1}/db" | grep -F "${displayNameVersion}")"

archivedName="$(sha256sum "tree.tar.xz" | awk '{print $1}')"

mv "tree.tar.xz" "${1}/packages/${archivedName}.tar.xz"

if [[ "$result" == "" ]]; then
        echo "${displayNameVersion} ${archivedName}" >> "${1}/db"
else
        newName="${displayNameVersion} ${archivedName}"
        sed -E -i "s/^${displayNameVersion}.*/${newName}/" "${1}/db"

        toRemove="$(echo "$result" | cut -d ' ' -f 2)"
        find "${1}/packages" -name "*${toRemove}\.tar\.xz" -exec rm '{}' \;
fi

sort -r -t ' ' -k 1,1 "${1}/db" -o "${1}/db"
