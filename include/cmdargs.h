#ifndef CMDARGS_H
#define CMDARGS_H

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>

// create a struct declaration
struct CmdArgs 
{
    char* inputFile;
    char* outputFile;
    char* extractFile;
    char* deleteFile;
    bool inputFlag;
    bool outputFlag;
    bool helpFlag;
    bool copyFlag;
    bool verifyFlag;
    bool directoryFlag;
    bool extractFlag;
    bool deleteFlag;
};

// create a typedef for this struct
typedef struct CmdArgs CmdArgs;

// function declaration for processing command line arguments
void process_args(CmdArgs* CA_ptr, int argc, char* argv[]);

#endif