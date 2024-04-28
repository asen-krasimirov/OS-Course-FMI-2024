#!/bin/bash

[[ $# == 2 ]] || { echo 'script takes 2 args' >&2; exit 1; }

[[ -f "$1" ]] || { echo '1st arg should be a file' >&2; exit 2; }

tempFile="$(mktemp)"

while read data; do
        if [[ "$(echo "$data" | awk -v "TYPE=$2" -v 'FS=,' '$5 == TYPE {print "true"}')" == 'true' ]]; then
                echo "$(echo "$data" | awk -v 'FS=,' '{print $4}')" >> "$tempFile";
        fi
done < <(cat "$1")

constalation="$(cat "$tempFile" | sort | uniq -c | head -n 1 | awk '{print $2}')"

cat "$1" | awk -v "CONST=$constalation" -v 'FS=,' 'CONST == $4 {print $0 }' | sort -r -t ',' -k 7,7 | head -n 1 | cut -d ',' -f 1

rm "$tempFile"
