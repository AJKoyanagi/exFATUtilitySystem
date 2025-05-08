#!/bin/bash

echo "Help Functionality Test"

# This test checks if the help flag is stored inside a struct and is implemented with correct functionality
# This test will make sure that a help menu is printed if help flag is specified on command line
# Finally, this test will also make sure that the help menu prints how to execute the program and allowed options
# Once it prints the menu, this test will also make sure that the program should exit without doing anything else

output=$(./extfat -h 2>&1)

# Check if the output contains the expected error message
if echo "$output" | grep -q "Hello! All the compilation process has been embedded in the makefile so please type the following in this order on command line to execute the program:"; 
then
    # now make sure that the program does not do anything else (it should not have any more output)
    # check if the output is 14 lines, this is supposed to be the number of lines when -h flag is set
    if [[ $(echo "$output" | wc -l) -eq 17 ]];
    then
        echo "Help test passed!"
        result=0
    else
        echo "Test failed: program does not exit after printing help information"
        echo "Expected output: $output"
        result=1
    fi
else
    echo "Test failed: program did not produce expected output with help flag specified"
    echo "Expected output: $output"
    result=1
fi

# 0 means pass and 1 means fail
exit $result