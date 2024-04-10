#!/bin/bash

moveImages() {
        # 1- imagesFile, 2- where
        while read -r file; do
                mv "$file" "$2"
        done < <(cat "$1")
}

[[ $# == 2 ]] || { echo 'script takes 2 args' >&2; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be a dir' >&2; exit 2; }

[[ ! -e "$2" ]] || { echo '2nd arg should not exist yet' >&2; exit 3; }

mkdir "$2"

toMove="$(mktemp)"

intStart=""
intEnd=""

checkDate=""
checkNextDate=""

while read -r fullDate name; do
        fullDate="$(echo "$fullDate" | cut -d '.' -f 1)"
        date="$(echo "$fullDate" | cut -d '_' -f 1)"

        if [[ "$date" != "$checkDate" ]] && [[ "$date" != "$checkNextDate" ]]; then
                if [[ "$intStart" != "" ]]; then
                        dirName="${2}/${intStart}_${intEnd}"
                        mkdir "$dirName"

                        moveImages "$toMove" "$dirName"
                        echo -n "" > "$toMove"
                fi

                intStart="$date"
        fi

        intEnd="$date"
        checkDate="$date"
        checkNextDate="$(date -d "$date + 1 day" +'%Y-%m-%d')"

        mv "$name" "${fullDate}.jpg"

        echo "${fullDate}.jpg" >> "$toMove"
done < <(find "$1" -type f -name '*.jpg' -printf "%TF_%TT %p\n" | sort)

if [[ "$intStart" != "" ]]; then
        dirName="${2}/${intStart}_${intEnd}"
        mkdir "$dirName"
        moveImages "$toMove" "$dirName"
        echo -n "" > "$toMove"
fi

rm "$toMove"
