#!/bin/bash

[[ "${#}" -eq 1 ]] || { echo "only 1 arg allowed"; exit 1; }

[[ -d $1 ]] || { echo "$1 is not a directory"; exit 2; }

find $1 -maxdepth 1 -type l -exec test -e '{}' ';' -print