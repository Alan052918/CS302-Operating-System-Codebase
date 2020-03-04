#!/bin/bash
NAME=cs302

echo 1 $NAME
echo 2 ${NAME}

V1=hello $NAME          # Invalid, alert command not found
V2='hello $NAME'        # Single quote: 
V3="hello $NAME"        #
V4="hello $NAMEabc"     #
V5="hello ${NAME}abc"   #

echo 3 $V1
echo 4 $V2
echo 5 $V3
echo 6 $V4
echo 7 $V5
