#!/bin/bash
if [ $# -eq 0 ]
then
    echo "No arguments provided, defaulting to 4 iterations"
    num_iterations=4
else
    num_iterations=$1
fi

if [ $num_iterations -gt 4 ]
then
    echo "Error: Maximum number of iterations is 4"
    exit 1
fi

for i in $(seq 0 $((num_iterations-1)))
do
    #echo -e "\n-----------------------"
    echo -e "Running make ARGS=DPIM$i:\n"
    make ARGS="DPIM$i"
    echo -e "\nFinished running make ARGS=DPIM$i"
    echo -e "---------------------------------\n"
done