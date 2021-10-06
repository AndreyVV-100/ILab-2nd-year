#!/bin/bash

# script path, copypasted from internet
BASEDIR=$(dirname $(realpath "$0"))
NUM_TESTS=$((10))

for ((i_test=1; i_test <= NUM_TESTS; i_test++))
do
    answer=$($BASEDIR/../a.out < $BASEDIR/Tasks/$i_test.task)
    right_answer=$(cat $BASEDIR/Answers/$i_test.answer)
    if [ $answer = $right_answer ]
    then
        echo "Test $i_test Passed."
    else
        echo "Test $i_test Failed."
    fi
done
