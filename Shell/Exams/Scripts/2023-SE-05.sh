#!/bin/bash

comOc="$(mktemp)"

ctr=0

while true; do
        processes="$(ps -o comm=,rss= | awk '$2 > 65536 {print $1}')"

        if [[ "$processes" == "" ]]; then
                break;
        fi

        for com in "$(echo "$processes")"; do
                echo "$com" >> "$comOc"
        done

        ctr="$(echo "$ctr + 1" | bc)"
        sleep 1
done

halfOc="$(echo "$ctr / 2" | bc)"

cat "$comOc" | sort | uniq -c | awk -v "hc=$halfOc" '$1 >= hc {print $1}'
