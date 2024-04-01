#!/bin/bash

[[ $# == 2 ]] || { echo '2 args should be given'; exit 1; }

[[ -f $1 ]] || { echo '1st arg should be a text file'; exit 2; }

[[ -d $2 ]] || { echo '2st arg should be a dir'; exit 3; }

while read file; do
        while read badWord; do
                astericsCount="$(echo "${badWord}" | wc -m)"

                toReplace="$(echo '*********************************' | grep -E -o "^\*{${astericsCount}}")" # Probably there is a better way to parse the asterics needed.

                # This solutions asumes that the content of the file consists of 1 line.
                line="$(cat ${file})"
                # To support multi-line files another while-construction should be added.

                echo "${line}" | sed -i -E "s/\ ${badWord}\ /\ ${toReplace}\ /" ${file} # Anchor is not working as expected ('\ ' should be replaced with '\<' and '\>').

        done < <(cat $1)

done < <(find . -type f -regex '.*\.txt$')
