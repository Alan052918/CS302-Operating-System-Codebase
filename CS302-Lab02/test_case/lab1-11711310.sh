#!/bin/bash
dirc=0
filc=0
output=$2
declare -a queue

push_queue() {
    element="$1"
    queue+=("$element")
}

pop_queue() {
    args=${queue[0]}
    queue=("${queue[@]:1}")
}

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
            push_queue "$entry"
            echo "$(
                cd "$(dirname "$entry")"
                pwd -P
            )/$(basename "$entry")" >>$output
        fi
    done
    echo >>$output
}

[ -e $output ] && rm $output # remove $2 if it already exists
push_queue "$1"
while [[ "${#queue[@]}" -ne 0 ]]; do
    curdir=${queue[0]}
    pop_queue
    walk "$curdir"
done
printf "[Directories Count]:%d\n" "$dirc" >>$output
printf "[Files Count]:%d\n" "$filc" >>$output
