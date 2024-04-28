#!/bin/bash

[[ $# == 2 ]] || { echo 'script should get 2 args' >&2; exit 1; }

[[ -f "${1}" ]] || { echo '1st args should be a file' >&2; exit 2; }

touch "$2"
tempFile="$(mktemp)"

while read line; do
        echo "$line" >> "$tempFile"
done < <(cat "${1}" | sort -n -t ',' -k 1,1 | sort -u -t ',' -k 2)

while read line; do
        if [[ "$(grep -F "$line" "$tempFile")" != "" ]]; then
                echo "$line" >> "$2"
        fi
done < <(cat "$1")
