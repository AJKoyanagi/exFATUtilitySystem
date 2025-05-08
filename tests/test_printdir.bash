#!/bin/bash

echo "Print Dir Functionality Test"

# this test checks and makes sure that -d flag 

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

output=$(./extfat -i test.image -d 2>&1)

if echo "$output" | grep -q "The directory listing"; 
then
    echo "Test passed: program produced expected output!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with no delete file specified!"
    echo "Expected output: $output"
fi

# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "1" ]; 
then
    result=0
    echo "All printdir tests passed!"
else
    result=1
    echo "1 or more printdir tests failed!"
fi

# 0 means pass and 1 means fail
exit $result