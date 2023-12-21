#!/bin/bash
# Program:
#	Build gem5, user can type
        # $1 = arch 
        # $2 = gem5 binary type
        # $3 = build variables
#   Example: scons build/{X86}/gem5.{opt} -j9
# History:
# 2021/11/26    johnnycck   First release

COMPILE_OPTIONS='--gold-linker --colors'
COMPILE_JOBS=$((`nproc`+1))
GLOBAL_BUILD_VARIABLES=""

function compile_gem5()
{
    if [ -z "$1" ] || [ -z "$2" ]; then
        echo "Usage: compile_gem5 ARCH BINARY_TYPE BUILD_VARIABLES"
    fi

    if [ "$3" ]; then
        GLOBAL_BUILD_VARIABLES="$GLOBAL_BUILD_VARIABLES $3"
    fi
    
    scons \
        ./build/"$1"/gem5."$2" \
        -j "$COMPILE_JOBS" \
        $GLOBAL_BUILD_VARIABLES
}
compile_gem5 $1 $2 $3