#!/bin/bash

[[ $# == 3 ]] || { echo 'script should take 3 args' >&2; exit 1; }

[[ -f "$1" ]] || { echo '1st arg should be a file' >&2; exit 2; }

[[ -f "$2" ]] || { echo '2nd arg should be a file' >&2; exit 3; }

firstPoint=""
secondPoint=""

while read row; do
        name="$(echo "$row" | cut -d ':' -f 1)"
        distance="$(echo "$row" | cut -d ':' -f 2 | cut -d ' ' -f 2)"

        if [[ "$name" == "$3" ]]; then
                firstPoint="$distance"
                break
        fi
done < "$1"

while read row; do
        name="$(echo "$row" | cut -d ':' -f 1)"
        distance="$(echo "$row" | cut -d ':' -f 2 | cut -d ' ' -f 2)"

        if [[ "$name" == "$3" ]]; then
                secondPoint="$distance"
                break
        fi
done < "$2"

echo "$firstPoint $secondPoint"

if [[ "$firstPoint" -gt "$secondPoint" ]]; then
        echo "$2"
else
        echo "$1"
fi
