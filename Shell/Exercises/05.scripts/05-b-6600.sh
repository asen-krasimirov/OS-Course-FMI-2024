#!/bin/bash

[[ "$#" == 1 ]] || { echo 'script should get only 1 arg'; exit 1; }

[[ -d "$1" ]] || { echo '1st arg should be a directory'; exit 2; }

while read file1; do
        if [[ ! -e "${file1}" ]]; then
                continue
        fi

        while read file2; do
                if [[ ! -e "${file2}" ]]; then
                        continue
                fi

                if [[ "$file1" == "$file2" ]]; then
                        continue
                fi

                if [[ "$(sha256sum ${file1} | awk '{print $1}')" == "$(sha256sum ${file2} | awk '{print $1}')" ]]; then
                        rm "${file2}"
                fi

        done < <(find . -type f | sort)

done < <(find . -type f | sort)
