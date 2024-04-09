#!/bin/bash

[[ $# == 2 ]] || { echo 'script should take 2 args' >&2; exit 1; }

[[ -f "$1" ]] || { echo '1st arg should be a file' >&2; exit 2; }

[[ -d "$2" ]] || { echo '2nd arg should be a dir' >&2; exit 3; }

while read word; do
        replace="$(echo "$word" | tr '[a-zA-Z0-9_]' '*')"

        while read file; do

                sed -E -i "s:\<${word}\>:${replace}:gi" "$file"

        done < <(find "$2" -type f -name "*.txt")
done < <(cat "$1")
