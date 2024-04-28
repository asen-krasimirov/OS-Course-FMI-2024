#!/bin/bash

[[ "$#" == 3 ]] || { echo 'script takes exactly 3 args'; exit 1; }

[[ -f "$1" ]] || { echo '1st arg should be a regular file'; exit 2; }

[[ "$2" =~ ^[^=]*$ ]] || { echo '2nd arg should be a string and not contain "="'; exit 3; }

[[ "$3" =~ ^[^=]*$ ]] || { echo '3rd arg should be a string and not contain "="'; exit 4; }

str1Val=""
str2Val=""

while read line; do
        key=$( echo "$line" | cut -d '=' -f 1 )
        val=$( echo "$line" | cut -d '=' -f 2 )

        if [[ "$key" == "$2" ]]; then
                str1Val="$val"
        elif [[ "$key" == "$3" ]]; then
                str2Val="$val"
        fi
done < <(cat "$1")

while read term2; do
        if [[ "$(echo "$str1Val" | grep -E -o "$term2")" ]]; then
                str2Val="$(echo "$str2Val" | sed -e s/"\ *$term2\ *"//)"
        fi

done < <(echo "$str2Val" | grep -E -o '[^\ ]+')

if [[ "$(cat "$1" | grep -E "${3}")" ]]; then
        sed -i -e s/"${3}=.*"/"${3}=${str2Val}"/ "${1}"
else
        echo "$3=$str2Val" >> "${1}"
fi
