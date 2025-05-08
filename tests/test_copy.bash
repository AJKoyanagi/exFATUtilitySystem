#!/bin/bash
echo "Copy Functionality Test"

# This test will check and make sure that the program successfully copies the inputfile to the outputfile when -c copy flag is specified
# If the inputfile was successfully copied to the output file, then they would both have identical md5sum

# First, store the md5sum of the inputfile in a variable without even running the program
result1=($(/usr/bin/md5sum test.image))

# run the program with inputfile and outputfile specified along with -c copy flag
# store the output in a variable so it is not printed out during the test
output=$(./extfat -i test.image -o test2.image -c)

# store the md5sum of the outputfile after running the program and copying the inputfile to the outputfile
result2=($(/usr/bin/md5sum test2.image))

# clean up since test2.image won't be needed anymore
$(rm test2.image)

# compare both md5sums and make sure they match
if [ "${result1[0]}" = "${result2[0]}" ]; 
then
    echo "Copy test passed: input and output files have identical md5sums!"
    result=0
else
    echo "Copy test failed: input and output files do not have identical md5sums!"
    result=1
fi
exit $result