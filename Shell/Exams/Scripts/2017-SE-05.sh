#!/bin/bash

[[ $# == 2 ]] || { echo 'script should get 2 args' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be a dir' >&2; exit 2; }

getGlobalVersion() {
        version="$(echo "$1" | cut -d '-' -f 2)"
        echo $version
}

sortFile="$(mktemp)"

while read file; do

        echo "${file} $(getGlobalVersion "${file}")" >> "$sortFile"

done < <(find "$1" -maxdepth 1 -regex ".*vmlinuz\-[0-9]+\.[0-9]+\.[0-9]+\-${2}$" -exec basename "{}" \;)

cat "${sortFile}" | sort -V -t ' ' -k 2,2 | tail -n 1 | cut -d ' ' -f 1

rm "$sortFile"
