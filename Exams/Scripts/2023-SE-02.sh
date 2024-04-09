#!/bin/bash

[[ $# -ge 2 ]] || { echo 'args should be at least 2' >&2; exit 1; }

[[ "$1" =~ [0-9]+ ]] || { echo '1st arg should be a num' >&2; exit 2; }

seconds="$1"

shift 1

com="$1"

shift 1

args="$1"

shift 1

for arg in "$@"; do
        args="${args} ${arg}"
done

elapsed="0"
runTimes="0"

while true; do
        s="$(date '+%s.%N' | grep -E -o '[0-9]+\.{0,1}[0-9]{0,2}' | head -n 1)"

        "$com" "$args"
        runTimes="$(( runTimes+1 ))"

        e="$(date '+%s.%N' | grep -E -o '[0-9]+\.{0,1}[0-9]{0,2}' | head -n 1)"

        elapsed="$(echo "scale=2; ${elapsed}+${e}-${s}" | bc)"

        if (( $(echo "$elapsed >= $seconds" | bc) )); then
                break
        fi
done

echo "Run the command '$com $args' $runTimes times for $elapsed seconds."

echo "Average runtime: $(echo "scale=2; $elapsed / $runTimes" | bc) seconds."
