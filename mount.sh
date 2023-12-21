#!/bin/bash
if  [   -z   "$1"  ] || [  -z   "$2"  ]; then
                    echo "Usage:  $0  /path/to/img  /mnt/dir"
                    exit
fi

mount  -o  loop,offset=$((2048*512)) $1 $2