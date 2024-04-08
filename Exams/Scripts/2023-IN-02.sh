#!/bin/bash

markLinksForDeletion() {
        while read inode; do
                find . -inum "${inode}" | head -n 1 >> "$2"
        done < <(echo "$1" | cut -d ' ' -f 1 | sort | uniq)
}

markForDeletion() {
        regContent="$(cat "$1")"
        linkContent="$(cat "$2")"

        if [[ "$regContent" != "" ]] && [[ "$linkContent" == "" ]]; then
                echo "$regContent" >> "$3"
        elif [[ "$regContent" == "" ]] && [[ "$linkContent" != "" ]]; then
                markLinksForDeletion "$linkContent" "$3"
        elif [[ "$regContent" != "" ]] && [[ "$linkContent" != "" ]]; then
                echo "$regContent" >> "$3"
                markLinksForDeletion "$linkContent" "$3"
        fi
}

[[ $# == 1 ]] || { echo 'script should get 1 arg' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be a dir' >&2; exit 2; }

fileSums="$(mktemp)"

while read file; do
        echo "$(sha256sum "$file")" >> "$fileSums"
done < <(find . -type f)

toDel="$(mktemp)"

regFiles="$(mktemp)"
linkFiles="$(mktemp)"
checkName=""
checkSum=""

while read line; do
        curName="$(echo "$line" | awk '{print $2}')"
        curSum="$(echo "$line" | awk '{print $1}')"

        if [[ "$checkSum" == "$curSum" ]]; then
                curInode="$(stat --printf "%i" "$curName")"
                if [[ "$curInode" == "$(stat --printf "%i" "$checkName")" ]]; then
                        echo "$curInode $curName" >> "$linkFiles"
                else
                        echo "$curName" >> "$regFiles"
                fi
        else
                markForDeletion "$regFiles" "$linkFiles" "$toDel"

                rm "$regFiles"
                rm "$linkFiles"

                regFiles="$(mktemp)"
                linkFiles="$(mktemp)"
        fi

        checkName="$curName"
        checkSum="$curSum"

done < <(cat "$fileSums" | sort)

markForDeletion "$regFiles" "$linkFiles" "$toDel"

rm "$fileSums"
rm "$regFiles"
rm "$linkFiles"

while read file; do
        echo "$file"
done < <(cat "$toDel")

rm "$toDel"
