#!/bin/bash

[[ "${#}" -eq 2 ]] || { echo '2 args should be given'; exit 1; }

[[ "${1}" =~ ^[0-9]*$ ]] || { echo '1st arg should be a number'; exit 2; }

[[ "${2}" =~ ^[0-9]*$ ]] || { echo '2nd arg should be a number'; exit 3; }

[[ "${1}" -lq "${2}" ]] || { echo '1st arg should be less then or equal to 2nd arg'; exit 4; }

[[ -e a ]] || { mkdir a; }
[[ -e b ]] || { mkdir b; }
[[ -e c ]] || { mkdir c; }

while read -r file; do
        lines=$(wc -l "${file}")

        if [[ ${lines} -lt "${1}" ]]; then
                mv "${file}" "a";
        elif [[ ${lines} -le "${2}" ]]; then
                mv "${file}" "b";
        else
                mv "${file}" "c";
        fi
done < <(find . -type f)