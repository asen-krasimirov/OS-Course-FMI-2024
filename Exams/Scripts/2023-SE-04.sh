#!/bin/bash

[[ $# == 1 ]] || { echo 'script should take 1 arg' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be a dir' >&2; exit 2; }

heshes="$(mktemp)"

while read -r file; do
        echo "$(sha1sum "$file")" >> "$heshes"
done < <(find "$1" -type f)

checkName=""
checkHesh=""
checkBytes=""

groupCount=0    # of depricated files
hasDelYet=0
memoryFreed=0

while read data; do
        curName="$(echo "$data" | awk '{print $2}')"
        curHesh="$(echo "$data" | awk '{print $1}')"

        if [[ "$curHesh" == "$checkHesh" ]]; then
                rm "$curName"
                ln "$checkName" "$curName"

                if [[ "$hasDelYet" == 0 ]]; then
                        groupCount="$(echo "$groupCount + 1" | bc)"
                        hasDelYet=1
                fi

                memoryFreed="$(echo "$memoryFreed + $checkBytes" | bc)"
        else
                checkName="$curName"
                checkHesh="$curHesh"
                checkBytes="$(wc -c "$curName" | cut -d ' ' -f 1)"
                hasDelYet=0
        fi

done < <(cat "$heshes" | sort)

echo "Dedublicated groups: $groupCount"
echo "Freed memory: $memoryFreed"
