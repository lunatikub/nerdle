#!/bin/bash

EQ="$1"
NAME="$2"

echo "/* ${EQ} */"
echo "static const enum symbol ${NAME}[] = {"
echo -n "  "

for i in $(seq 0 $((${#EQ} - 1)))
do
    c="${EQ:$i:1}"

    ([ "$c" = "1" ] && echo -n "1, ") ||
        ([ "$c" = "2" ] && echo -n "2, ") ||
        ([ "$c" = "3" ] && echo -n "3, ") ||
        ([ "$c" = "4" ] && echo -n "4, ") ||
        ([ "$c" = "5" ] && echo -n "5, ") ||
        ([ "$c" = "6" ] && echo -n "6, ") ||
        ([ "$c" = "7" ] && echo -n "7, ") ||
        ([ "$c" = "8" ] && echo -n "8, ") ||
        ([ "$c" = "9" ] && echo -n "9, ") ||
        ([ "$c" = "0" ] && echo -n "0, ") ||
        ([ "$c" = "/" ] && echo -n "D, ") ||
        ([ "$c" = "+" ] && echo -n "P, ") ||
        ([ "$c" = "-" ] && echo -n "M, ") ||
        ([ "$c" = "*" ] && echo -n "X, ") ||
        ([ "$c" = "=" ] && echo -n "E, ")
done

echo ""
echo "};"

exit 0
