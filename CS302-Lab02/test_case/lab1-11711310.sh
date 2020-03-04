#!/bin/bash
dirc=0
filc=0
output=$2

walk() {
    printf "[%s]\n" "$(basename "$1")" >>$output

    for entry in "$1"/*; do
        if [[ -f "$entry" ]]; then
            filc=$(($filc + 1))
            echo "$(
                cd "$(dirname "$entry")"
                pwd -P
            )/$(basename "$entry")" >>$output
        elif [[ -d "$entry" ]]; then
            dirc=$(($dirc + 1))
            echo "$(
                cd "$(dirname "$entry")"
                pwd -P
            )/$(basename "$entry")" >>$output
        fi
    done
    echo >>$output

    for entry in "$1"/*; do
        if [[ -d "$entry" ]]; then
            walk "$entry"
        fi
    done
}

[ -e $output ] && rm $output # remove $2 if it already exists
walk "$1"
printf "[Directories Count]:%d\n" "$dirc" >>$output
printf "[Files Count]:%d\n" "$filc" >>$output
