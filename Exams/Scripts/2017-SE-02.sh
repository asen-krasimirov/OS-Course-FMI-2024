#!/bin/bash

[[ $# == 3 ]] || { echo 'args should be 3' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be dir' >&2; exit 2; }

[[ -d "$2" ]] || { echo '2nd arg should be dir' >&2; exit 3; }

[[ "$(dir -C "$2")" =~ ^$ ]] || { echo '2nd arg (dir) should not have any files in it' >&2; exit 4; }

if [[ "$(whoami)" != root ]]; then
        echo 'script should be run as a root' >&2;
        exit 5;
fi

while read file; do
        newDest="$(echo "${file}" | sed s/${1}/${2}/)"
        newPath="$(dirname "${newDest}")"

        mkdir -p "${newPath}"

        mv "${file}" "${newDest}"

done < <(find "$1" -type f -name "*${3}*")
