#!/bin/bash

[[ $# -le 2 ]] || { echo 'script should take at most 2 args'; exit 1; }

[[ $# -ge 1 ]] || { echo 'scirpt should take at leat 1 arg'; exit 2; }

[[ $1 =~ ^[0-9]+$ ]] || { echo '1st arg should be a number'; exit 3; }

[[ $2 =~ ^.$ ]] || { echo '2nd arg should be a single char delimiter'; exit 4; }

wordLen="$(echo $1 | wc -m)"
wordLen="$(( wordLen - 1 ))"

ctr="0"
digitCtr="0"
newNum=""

while read digit; do
        ctr="$(( ctr + 1 ))"
        digitCtr="$(( digitCtr + 1 ))"

        newNum="${newNum}${digit}"

        if [[ $ctr -eq 3 ]]; then
                [[ ${digitCtr} -ne ${wordLen} ]] || { continue; }
                newNum="${newNum}$2"
                ctr="0"
        fi
done < <(echo "$1" | rev | grep -E -o '.')

echo ${newNum} | rev
