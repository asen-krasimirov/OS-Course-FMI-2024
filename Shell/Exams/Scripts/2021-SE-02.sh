#!/bin/bash

isSoaRowCorrect() {
        echo "$1" | grep -E "^([a-z0-9]+\.)+[ ]+[0-9]*[ ]+IN[ ]+SOA[ ]+([a-z0-9]+\.)+[ ]+([a-z0-9]+\.)+[ ]+([0-9]+[ ]+){4}[0-9]+$"
}

isMultiLineSoaRowCorrect1() {
        echo "$1" | grep -E "^([a-z0-9]+\.)+[ ]+[0-9]*[ ]+IN[ ]+SOA[ ]+([a-z0-9]+\.)+[ ]+([a-z0-9]+\.)+[ ]+\("
}

isMultiLineSoaRowCorrect2() {
        echo "$1" | grep -E "^[ ]+[0-9]+[ ]+\;[ ]+serial"
}

isMultiLineSoaRowCorrect3() {
        echo "$1" | grep -E "^[ ]+[0-9]+[ ]+\;[ ]+refresh"
}

isMultiLineSoaRowCorrect4() {
        echo "$1" | grep -E "^[ ]+[0-9]+[ ]+\;[ ]+retry"
}

isMultiLineSoaRowCorrect5() {
        echo "$1" | grep -E "^[ ]+[0-9]+[ ]+\;[ ]+expire"
}

isMultiLineSoaRowCorrect6() {
        echo "$1" | grep -E "^[ ]+[0-9]+[ ]+\;[ ]+negative TTL"
}

isMultiLineSoaRowCorrect() {
        result1="$(isMultiLineSoaRowCorrect1 "$(cat "$file" | head -n 1 )")"
        result2="$(isMultiLineSoaRowCorrect2 "$(cat "$file" | head -n 2 | tail -n 1)")"
        result3="$(isMultiLineSoaRowCorrect3 "$(cat "$file" | head -n 3 | tail -n 1)")"
        result4="$(isMultiLineSoaRowCorrect4 "$(cat "$file" | head -n 4 | tail -n 1)")"
        result5="$(isMultiLineSoaRowCorrect5 "$(cat "$file" | head -n 5 | tail -n 1)")"
        result6="$(isMultiLineSoaRowCorrect6 "$(cat "$file" | head -n 6 | tail -n 1)")"

        if [[ "$result1" == "" ]] || [[ "$result2" == "" ]] || [[ "$result3" == "" ]] || [[ "$result4" == "" ]] || [[ "$result5" == "" ]] || [[ "$result6" == "" ]]; then
                echo ""
        else
                echo "true"
        fi
}

incrementSerialNum() {  # serial, curDate, file
        serialNum="$(echo "$1" | grep -E -o '[0-9]{2}$')"
        serialDate="$(echo "$1" | grep -E -o '^[0-9]{8}')"

        if [[ "$serialDate" -eq "$2" ]]; then
                serialNum="$(( serialNum+1 ))"
                if [[ "$serialNum" -lt 10 ]]; then
                        serialNum="0${serialNum}"
                fi
        elif [[ "$serialDate" -lt "$2" ]]; then
                serialDate="$2"
                serialNum="00"
        fi

        newSerial="${serialDate}${serialNum}"

        sed -i -E "s/${serial}/${newSerial}/" "$file"
}

for file in "$@"; do
        [[ -f "$file" ]] || { echo 'only files allowed' >&2; exit 1; }

        result1="$(isSoaRowCorrect "$(cat "$file" | head -n 1)")"
        result2="$(isMultiLineSoaRowCorrect "$(cat "$file")")"

        curDate="$(date '+%Y%m%d')"

        if [[ "$(wc -l "$file" | cut -d ' ' -f 1)" == 1 ]]; then
                if [[ "$result1" == "" ]]; then
                        echo "format is not correct for ${file}" >&2; exit 2;
                fi

                serial="$(echo "$result1" | grep -E -o '[0-9]{10}')"
                incrementSerialNum "$serial" "$curDate" "$file"
        else
                if [[ "$result2" == "" ]]; then
                        echo "format is not correct for ${file}" >&2; exit 3;
                fi

                serial="$(cat "$file" | head -n 2 | tail -n 1 | tr -d ' ' | cut -d ';' -f 1)"
                incrementSerialNum "$serial" "$curDate" "$file"
        fi
done
