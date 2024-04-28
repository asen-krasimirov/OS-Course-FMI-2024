#!/bin/bash

[[ ${#} -eq 2 ]] || { echo 'args should be 2'; exit 1; }

[[ -d ${1} ]] || { echo '1st arg should be a dir'; exit 2; }

[[ -d ${2} ]] || { echo '2nd arg should be a dir'; exit 3; }


while read file; do
        if [[ ${file} =~ \..*\.swp$ ]]; then
                fileToCheck="$(echo "${file}" | cut -d '.' -f 2)"

                if [[ -e ${fileToCheck} ]]; then
                        continue
                fi
        fi

        newFile="$(echo ${file} | sed s:${1}:${2}:)"

        mkdir -p $(dirname ${newFile})

        cp ${file} ${newFile}

done < <(find ${1} -type f )