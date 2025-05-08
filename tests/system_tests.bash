#!/bin/bash
echo "Running system tests."

# first, need to make sure the system is in a consistent state,
# meaning no extfat drives are mounted, and /dev/loop2 does not exist
# it will also delete a "test.image" file, it it exists

bash "tests/clean_up.bash"

TestCount=0
TestFailed=0
TestPassed=0

for ImageSize in "1M" "10M"
do
     for TestScript in $(ls tests/test_*.bash)
     do
        TestCount=$((TestCount+1))

        echo ">>>>>>>>>  $TestScript <<<<<<<<   $ImageSize  <<<<<<<<<<"

        # create the disk image file
        dd if=/dev/zero of=test.image count=1 bs=$ImageSize
        sudo losetup /dev/loop2 test.image
        sudo /usr/sbin/mkexfatfs /dev/loop2

        # mount the drive (may not always be needed)
        mkdir /tmp/d
        sudo mount /dev/loop2 /tmp/d


        bash $TestScript
        if [ $? -eq 0 ]
        then
            TestPassed=$((TestPassed+1))
        else
            TestFailed=$((TestFailed+1))
        fi
   
     # after the test completes, need to clean up
     bash "tests/clean_up.bash"
     done
done
echo "$TestCount ran $TestFailed failed $TestPassed passed."

if [ "$TestPassed" = "$TestCount" ]; 
then
    echo "All system tests have passed!"
else
    echo "1 or more system tests have failed!"
fi