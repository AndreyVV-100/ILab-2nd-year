NUM_TESTS=$((10))

for ((i_test=1; i_test <= NUM_TESTS; i_test++))
do
    answer=$(../a.out < Tasks/$i_test.task)
    right_answer=$(cat Answers/$i_test.answer)
    if [ $answer = $right_answer ]
    then
        echo "Test $i_test Passed."
    else
        echo "Test $i_test Failed."
    fi
done
