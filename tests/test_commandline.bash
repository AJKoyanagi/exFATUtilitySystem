#!/bin/bash

echo "Command Line Functionality Test"

# This test checks and make sures that the program produces expected output and exits without doing anything else if command line is empty or there are invalid command line args

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

# if no command line args are given, the program should output the expected output and exit without doing anything else
# store the output in a variable and redirect the stderr stream to same destination as stdout stream
output=$(./extfat 2>&1)

# Check if the output contains the expected error message
if echo "$output" | grep -q "Input file must be specified."; 
then
    echo "Test passed: program produced expected output with no command line arguments!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with no command line arguments"
    echo "Expected output: $output"
fi

# if invalid args are given that are not flags, the program should output the expected output and exit without doing anything else
output=$(./extfat jkfdkla ewifgna sapoe 2>&1)

# Check if the output contains the expected error message
if echo "$output" | grep -q "Input file must be specified."; 
then
    echo "Test passed: program produced expected output with invalid command line arguments!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with invalid command line arguments"
    echo "Expected output: $output"
fi

# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "2" ]; 
then
    result=0
    echo "All command line tests passed!"
else
    result=1
    echo "1 or more command line tests failed!"
fi

# 0 means pass and 1 means fail
exit $result