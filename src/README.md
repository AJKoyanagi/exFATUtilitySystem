# Command Options:
< Keep this up to date >
-h   // this is help flag and it will show how to print the help text on how to execute the program and the allowed options, and then exit

It will print the following information:
Hello! All the compilation process has been embedded in the makefile so please type the following in this order on command line to execute the program:
1. make clean
2. make
3. ./extfat     -- here, you can type various flags in the command line when running the program

Available flags and their uses:
-i inputfile    -- i flag must be followed by a file name
-o outputfile   -- o flag must be followed by a file name
-h              -- help flag displays help information for the user
-c              -- copy flag requires that the input file and output file are specified somewhere in the command line and it will copy input file to output file without change
-v              -- verify flag requires that the input file must be specified somewhere in the command line and it will verify the input file
-d              -- print out the current directories and subdirectories that are specified in create_image.bash prior to making the image
-x              -- Extracts from the extfat image a file to a specified output file
-D              -- Deletes the specified file from the extfat image

Have fun!


make clean
make
./extfat -- here, you can type various flags in the command line when running the program
Available flags and their uses: -i inputfile -- i flag must be followed by a file name -o outputfile -- o flag must be followed by a file name -h -- help flag displays help information for the user -c -- copy flag requires that the input file and output file are specified somewhere in the command line and it will copy input file to output file without change -v -- verify flag requires that the input file must be specified somewhere in the command line and it will verify the input file -d print directory -- print out the current directories and subdirectories that are specified in create_image.bash prior to making the image -x extraction -- Extracts from the extfat image a file to a specified output file

Have fun!