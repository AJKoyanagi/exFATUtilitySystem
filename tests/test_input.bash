#!/bin/bash

echo "Input Functionality Test"

# this test checks and makes sure that -i flag accepts an inputfile
# this test also checks that passed in inputfile is valid or not and if it is being stored in a struct or not

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

# if the user specifies the -i flag but does not provide the inputfile, the program should not work
# if this test passes, then that means that -i flag must accept an inputfile for rest of the program to work
output=$(./extfat -i 2>&1)

if echo "$output" | grep -q "Provided flag needs a filename!"; 
then
    echo "Test passed: program produced expected output with no input file specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with no input file specified!"
    echo "Expected output: $output"
fi

# now check even if the user does provide an inputfile, that it is being stored inside a struct
# if the inputfile is being printed out to stdout, then it is being stored in the struct
# since that print statement only references the struct, not any other variable

output=$(./extfat -i test.image 2>&1)

if echo "$output" | grep -q "This is my input filename: test.image"; 
then
    echo "Test passed: program produced expected output with input file specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with input file specified!"
    echo "Expected output: $output"
fi

# now finally, check if the provided inputfile is valid or not
# if invalid, the program should not do anything else and just exit
output=$(./extfat -i jflkda 2>&1)

if echo "$output" | grep -q "input file failed to open: No such file or directory"; 
then
    echo "Test passed: program produced expected output with invalid input file specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed: program did not produce expected output with invalid input file specified!"
    echo "Expected output: $output"
fi

# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "3" ]; 
then
    result=0
    echo "All input tests passed!"
else
    result=1
    echo "1 or more input tests failed!"
fi

# 0 means pass and 1 means fail
exit $result