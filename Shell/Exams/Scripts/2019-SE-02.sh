#!/bin/bash

getDate() {
        echo "$1" | cut -d ' ' -f 1,2
}

getRestOfData() {
        echo "$1" | cut -d ' ' -f 3-
}

N=10

if [[ "$1" == "-n" ]]; then
        [[ "$2" =~ ^[0-9]+$ ]] || { echo 'if -n is given, N should be a number' >&2; exit 1; }
        N="$2"
        shift 2
fi

tempFile="$(mktemp)"

while [[ "$1" != "" ]]; do
        while read line; do
                fileName="$(echo "$1" | cut -d '.' -f 1)"

                line="$(getDate "${line}") ${fileName} $(getRestOfData "${line}")"

                echo "$line" >> "$tempFile"
        done < <(cat "$1" | tail -n "$N")

        shift 1
done

cat "$tempFile" | sort

rm "$tempFile"
