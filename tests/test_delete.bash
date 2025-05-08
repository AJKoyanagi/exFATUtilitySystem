#!/bin/bash

echo "Delete Functionality Test"

# this test checks and makes sure that -D flag accepts an delete file and deletes the file from the image

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

# if the user specifies the -D flag but does not provide the delete file, the program should not work
# if this test passes, then that means that -D flag must accept a delete file for rest of the program to work
output=$(./extfat -i test.image -D 2>&1)

if echo "$output" | grep -q "Provided flag needs a filename!"; 
then
    echo "Test passed: program produced expected output with no delete file specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with no delete file specified!"
    echo "Expected output: $output"
fi


# if the program enters delete directory handler only then all the deletion implementation will work
output=$(./extfat -i test.image -D routines.c 2>&1)

if echo "$output" | grep -q "Inside delete directory handler..."; 
then
    echo "Test passed: program produced expected output with delete file specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with delete file specified!"
    echo "Expected output: $output"
fi


# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "2" ]; 
then
    result=0
    echo "All delete tests passed!"
else
    result=1
    echo "1 or more delete tests failed!"
fi

# 0 means pass and 1 means fail
exit $result