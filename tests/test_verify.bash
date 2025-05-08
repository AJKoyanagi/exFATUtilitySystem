#!/bin/bash

echo "Verify Functionality Test"

# This test will check if verify functionality works as expected
# First, it will make sure that input file is required
# Then, it will make sure that main and backup boot sectors are read and they are same

# Variable to keep track of how many tests have passed in this bash script
TestPassed=0

# if input flag and inputfile are not specified along with verify flag, then program produces expected output and exit
output=$(./extfat -v 2>&1)

# Check if the output contains the expected error message
if echo "$output" | grep -q "Verify flag needs an input file!"; 
then
    echo "Test passed with no inputfile specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed with no inputfile specified"
    echo "Expected output: $output"
fi

# Now, if inputfile is there along with verify flag, then check that main and backup boot sectors are read and they are same
# to check this, there is only one correct output if verify flag works successfully
output=$(./extfat -v -i test.image 2>&1)

# Check if the output contains the expected error message
if echo "$output" | grep -q "Checksum values between Main_Boot and Backup_Boot are the same"; 
then
    echo "Test passed with inputfile specified!"
    TestPassed=$((TestPassed+1))
else
    echo "Test failed with inputfile specified"
    echo "Expected output: $output"
fi

# return the appropriate value based on number of tests passed
if [ "$TestPassed" = "2" ]; 
then
    result=0
    echo "All verify tests passed!"
else
    result=1
    echo "1 or more verify tests failed!"
fi

# 0 means pass and 1 means fail
exit $result