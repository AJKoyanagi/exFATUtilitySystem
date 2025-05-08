#include "cmdargs.h"

// function definition that processes command line arguments
void process_args(CmdArgs* CA_ptr, int argc, char* argv[])
{
    // option will be used to process each command line argument
    int option = -1;
    int loop_status = 1;

    // getopt() function returns int value of a flag char specified by the third parameter
    // colons in the string indicate that flag must be followed by a value stored in optarg pointer   
    while(((option = getopt(argc, argv, ":i:o:hcvdD:x:")) != -1) && loop_status)
    {
        switch(option)
        {
            case 'i':
                // if optarg begins with a dash, then that means the next command line argument is a flag which is incorrect
                if (*optarg == '-')
                {
                    printf("%c flag needs an input filename!\n", option);
                    loop_status = 0;
                }
                else if (CA_ptr->inputFlag)
                {
                    // make sure input flag is not used more than once
                    printf("Input flag already used in the command line!\n");
                    loop_status = 0;
                }
                else
                {
                    // store the input filename inside the struct and update input flag status to true
                    CA_ptr->inputFile = optarg;
                    CA_ptr->inputFlag = true;
                }
                break;
            case 'o':
                // if optarg begins with a dash, then that means the next command line argument is a flag which is incorrect
                if (*optarg == '-')
                {
                    printf("%c flag needs an output filename!\n", option);
                    loop_status = 0;
                }
                else if (CA_ptr->outputFlag)
                {
                    // make sure output flag is not used more than once
                    printf("Output flag already used in the command line!\n");
                    loop_status = 0;
                }
                else
                {
                    // store the output filename inside the struct and update the output flag status to true
                    CA_ptr->outputFile = optarg;
                    CA_ptr->outputFlag = true;
                }
                break;
            case 'h':
                // make sure help flag is not used more than once
                if (CA_ptr->helpFlag)
                {
                    printf("Help flag already used in the command line!\n");
                    loop_status = 0;
                }
                else
                {
                    // update help flag status to true
                    CA_ptr->helpFlag = true;
                }
                break;
            case 'c':
                // make sure copy flag is not used more than once
                if (CA_ptr->copyFlag)
                {
                    printf("Copy flag already used in the command line!\n");
                    loop_status = 0;
                }
                else
                {
                    // update copy flag status to true
                    CA_ptr->copyFlag = true;
                }
                break;
            case 'v':
                // make sure verify flag is not used more than once
                if (CA_ptr->verifyFlag)
                {
                    printf("Verify flag already used in the command line!\n");
                    loop_status = 0;
                }
                else
                {
                    // update verify flag status to true
                    CA_ptr->verifyFlag = true;
                }
                break;
            case 'D':
                CA_ptr->deleteFlag = true;
                CA_ptr->deleteFile = optarg;
                break;
            case ':':
                // getopt() will return int value of colon if no value is given due to the presence of colon in the beginning of the string passed in as third parameter to getopt()
                printf("Provided flag needs a filename!\n");
                loop_status = 0;
                break;
            case '?':
                // getopt() will return int value of ? for unrecognized option
                printf("Flag(s) not recognized!\n");
                loop_status = 0;
                break;
            case 'd':
                CA_ptr->directoryFlag = true;
                break;
            case 'x':
                CA_ptr->extractFile = optarg;
                CA_ptr->extractFlag = true;
                break;
            default:
                printf("Unexpected behavior!\n");
                loop_status = 0;
        }
    }
}