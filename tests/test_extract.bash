#!/bin/bash

echo "Extract Functionality Test"

# this test checks and makes sure that -d flag 

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

output=$(./extfat -x mmap.c -o out.txt 2>&1)

if echo "$output" | grep -q "Input file needed to extract information."; 
then
    echo "Test passed: program produced expected output!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output"
    echo "Expected output: $output"
fi

output=$(./extfat -i test.image -x mmap.c 2>&1)

if echo "$output" | grep -q "Output file needed for file to be printed out to."; 
then
    echo "Test passed: program produced expected output!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output"
    echo "Expected output: $output"
fi

output=$(./extfat -i test.image -x mmap.c -o out.txt 2>&1)

if echo "$output" | grep -q "Inside extract directory handler"; 
then
    echo "Test passed: program produced expected output!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with extract file specified!"
    echo "Expected output: $output"
fi

# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "3" ];
then
    result=0
    echo "All extract tests passed!"
else
    result=1
    echo "1 or more extract tests failed!"
fi

# 0 means pass and 1 means fail
exit $result