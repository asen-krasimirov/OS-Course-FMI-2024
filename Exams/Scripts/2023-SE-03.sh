#!/bin/bash

[[ $# == 1 ]] || { echo 'script gets only 1 arg' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st args should be a dir' >&2; exit 2; }

fileOc="$(mktemp)"
totalOc="$(mktemp)"

fileCount=0

while read -r file; do
        while read -r count word; do
                echo "$word" >> "$fileOc"

                res="$(grep -E "^[0-9]+ $word$" "$totalOc")"

                if [[ "${?}" -eq 0 ]]; then
                        oldOc="$(echo "$res" | cut -d ' ' -f 1)"
                        newOc="$(( oldOc + count ))"
                        sed -i -E "s:$res:${newOc} ${word}:" "$totalOc"
                else
                        echo "$count $word" >> "$totalOc"
                fi

        done < <(cat "$file" | grep -E -o "\<[a-z]+\>" | sort | uniq -c | sort -n | awk '$1 >= 3 {print $1" "$2}')

        fileCount="$(echo "$fileCount + 1" | bc)"
done < <(find "$1" -type f)


fileCount="$(echo "$fileCount / 2" | bc)"

allowedWords="$(cat "$fileOc" | sort | uniq -c | sort -n | awk -v "fc=$fileCount" '$1 >= fc {print $2}')"

sort -n -r "$totalOc" -o "$totalOc"

ctr=0

while read word; do
        if [[ "$(echo "$allowedWords" | grep -E "\<${word}\>")" != "" ]]; then
                echo "$word"
                ctr="$(echo "$ctr + 1" | bc)"
        fi

        if [[ "$ctr" -ge 10 ]]; then
                break
        fi

done < <(cat "$totalOc" | cut -d ' ' -f 2)

rm "$fileOc"
rm "$totalOc"
