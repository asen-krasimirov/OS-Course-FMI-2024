#!/bin/bash

[[ $# -ge 1 ]] || { echo 'script should take at least 1 arg'; exit 1; }

[[ $# -le 2 ]] || { echo 'script should take at most 2 args'; exit 2; }

[[ -d $1 ]] || { echo '1st arg should be a directory'; exit 3; }

if [[ $# == 2 ]]; then
        [[ -f $2 ]] || { echo '2nd arg should be a regular file'; exit 4; }
fi

brokenLinks="0"
while read link object; do
        # echo "link ${i}: ${link} -> ${object}"
        if [[ -e ${object} ]]; then
                if [[ $# == 2 ]]; then
                        echo "${link} -> ${object}" >> $2
                else
                        echo "${link} -> ${object}"
                fi
        else
                brokenLinks=$(( brokenLinks+1 ))
        fi
done < <(find $1 -type l -printf '%p %l\n')

if [[ $# == 2 ]]; then
        echo "Broken symlinks: ${brokenLinks}" >> $2
else
        echo "Broken symlinks: ${brokenLinks}"
fi
