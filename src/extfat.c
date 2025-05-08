#include "extfat.h"
#include "cmdargs.h"

int main(int argc, char* argv[])
{  
   fileInfo *fi = (fileInfo *)malloc(sizeof(fileInfo));
    
    if (fi == NULL)
    {
        perror("malloc error");
        return -1;
    }
    
    fi->MB = NULL;
    fi->BB = NULL;
    fi->BB_ = NULL;
    fi->FAT = NULL;
    fi->Data = NULL;
    fi->inputfile = NULL;
    fi->outputfile = NULL;
    fi->mbrChecksum = 0;
    fi->bbrChecksum = 0;
    fi->filename = argv[1];
    fi->dot = 0;
    fi->fd = -1;
    fi->fd2 = -1;
    fi->exfatSize = 0;
    fi->size = 0; 
    fi->SB = 0;
    fi->DB = 0;
    fi->src = 0;
    fi->des = 0;
    fi->bytesPerSector = 0;
    fi->sectorsPerCluster = 0;
    fi->filesize = 0;
    fi->SectorSize = SECTOR_SIZE;
    struct stat st;

	// mapping data into the struct
	fi->MB = (Main_Boot *)mmap(NULL, sizeof(Main_Boot), PROT_READ | PROT_WRITE, MAP_SHARED, fi->fd, 0);
	// create a struct to process command line arguments and initialize to NULL and false values
	CmdArgs CA = {NULL, NULL, NULL, NULL, false, false, false, false, false, false, false, false};

	// call function to process command line arguments
	process_args(&CA, argc, argv);

	// print out CmdArgs struct members based on which ones are activated on command line input
	if (CA.inputFlag)
	{
		printf("This is my input filename: %s     Input flag status: %d\n", CA.inputFile, CA.inputFlag);
		fi->inputfile = CA.inputFile;
		
		// open the file
		fi->fd = openMapFile(fi->inputfile);

		fstat(fi->fd, &st);
		fi->size = st.st_size;

		//maps the input file in memory to the size of struct Main_Boot 
		fi->MB = (Main_Boot *)mmap(NULL, fi->size, PROT_READ | PROT_WRITE, MAP_SHARED, fi->fd, 0);
		if (fi->MB == (void *)-1)
		{
			perror("error from mmap:");
			exit(EXIT_FAILURE);
		}

		fi->bytesPerSector = 2 << (fi->MB->BytesPerSectorShift-1);
		fi->exfatSize = (fi->bytesPerSector * fi->MB->VolumeLength);  
	}

	//takes in the -o argument and populates outputfile variable
	if (CA.outputFlag)
	{
		printf("This is my output filename: %s    Output flag status: %d\n", CA.outputFile, CA.outputFlag);
		fi->outputfile = CA.outputFile;
		printf("File: %s has been opened.\n\n", fi->outputfile);
	}

	//takes in the -c argument for file copying
	if (CA.copyFlag) 
	{
		// check and make sure that copy flag also has an input file and an output file
		if (CA.inputFlag && CA.outputFlag)
		{
			char extension[] = ".image";
			printf("Copy flag set! %d with input filename: %s   and    output filename: %s\n", CA.copyFlag, CA.inputFile, CA.outputFile);
			fi->dot = strrchr(CA.inputFile, '.');

			// copy flag implementation
			if(fi->dot && strcmp(fi->dot, extension) == 0)
			{
			fi->bytesPerSector = 2 << (fi->MB->BytesPerSectorShift-1);
			fi->exfatSize = (fi->bytesPerSector * fi->MB->VolumeLength);  
			//Use the above information to find the size of file for copying

			//assign a size of output file and write the single byte at the end of the file
			createFile(fi->outputfile, fi->exfatSize);

			//open source file again to map again to a size void
			fi->src = openMapFile(fi->inputfile);
			fi->SB = mmap(NULL, fi->exfatSize, PROT_READ | PROT_WRITE, MAP_SHARED, fi->src, 0); //source map

			//reopens outputfile to prepare it for mapping and copying
			fi->des = openMapFile(fi->outputfile);
			fi->DB = mmap(NULL, fi->exfatSize, PROT_READ | PROT_WRITE, MAP_SHARED, fi->des, 0); // destination map

			//copies the source file map into destination map
			memcpy(fi->DB, fi->SB, fi->exfatSize); 
			printf("Memory has been successfully copied.\n\n");
			
			//unmap and closes both source and destination file and close respectively
			closeMap(fi->SB, fi->exfatSize, fi->src);
			closeMap(fi->DB, fi->exfatSize, fi->des);
			}
			else
			{
				printf("\n\nExfat image not detected. Copying different filetype.\n\n");
				fi->src = openMapFile(fi->inputfile);
				fi->filesize = lseek(fi->src, 0, SEEK_END);
				fi->SB = mmap(NULL, fi->filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fi->src, 0); //source map

				//reopens outputfile to prepare it for mapping and copying
				createFile(fi->outputfile, fi->filesize);
				fi->des = openMapFile(fi->outputfile);
				ftruncate(fi->des, fi->filesize);
				fi->DB = mmap(NULL, fi->filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fi->des, 0); // destination map

				//copies the source file map into destination map
				memcpy(fi->DB, fi->SB, fi->filesize); 
				printf("Memory has been successfully copied.\n\n");
				
				//unmap and closes both source and destination file and close respectively
				closeMap(fi->SB, fi->filesize, fi->src);
				closeMap(fi->DB, fi->filesize, fi->des);
			}
		}
		
		else
		{
			//Since -o option is optional, if the output file was not specified, the inputfile will copy back to the inputfile
			printf("No output file was specified.\n");
			printf("%s was copied back to %s\n", CA.inputFile, CA.inputFile);
		}
	}

	if (CA.verifyFlag) // verify flag implementation 
	{
		// check and make sure that verify flag also has an input file
		if (CA.inputFlag)
		{
			char extension[] = ".image";
			fi->dot = strrchr(CA.inputFile, '.');
			fi->fd2 = openMapFile(fi->inputfile);
			
			printf("Verify flag set! %d with input filename: %s\n", CA.verifyFlag, CA.inputFile);
			fstat(fi->fd2, &st);
			fi->size = st.st_size;

			if(fi->dot && strcmp(fi->dot, extension) == 0)
			{
				fi->BB = (Main_Boot *)mmap(NULL,
										fi->size + (12 * SECTOR_SIZE),
										PROT_READ,
										MAP_SHARED,
										fi->fd2,
										0); //sets backup boot offset 12 sectors and sets it 1 sector long 

				fi->BB_ = fi->BB;
				void* t = fi->BB;
				t = t + OFFSET;
				fi->BB = (Main_Boot *) t;

				if (fi->BB == (void *) -1)
				{
				perror("error from mmap:"); // prints error message 
				exit(EXIT_FAILURE);
				}         
				compareBoot(fi->MB, fi->BB, fi->bytesPerSector);
				
				if(munmap(fi->BB_, sizeof(Main_Boot))== -1)
				{
				perror("error from unmap:"); // unmap backup boot
				exit(0);
				}
			}
			else
			{
				printf("\nThis file is not an extfat image. Try another file.\n\n");
			}
		}
		else
		{
			printf("Verify flag needs an input file!\n");
		}
   	}
   
	// print the help info if help flag is set to true here and then exit the program
	if (CA.helpFlag)
	{
		printf("Help flag set! %d\n", CA.helpFlag);
		printf("Hello! All the compilation process has been embedded in the makefile so please type the following in this order on command line to execute the program:\n");
		printf("1. make clean\n2. make\n3. ./extfat     -- here, you can type various flags in the command line when running the program\n\n");
		printf("Available flags and their uses:\n-i              -- i flag must be followed by a file name\n");
		printf("-o              -- o flag must be followed by a file name\n-h              -- help flag displays help information for the user\n");
		printf("-c              -- copy flag requires that the input file and output file are specified somewhere in the command line and it will copy input file to output file without change\n");
		printf("-v              -- verify flag requires that the input file must be specified somewhere in the command line and it will verify the input file\n");
		printf("-d              -- print out the current directories and subdirectories that are specified in create_image.bash prior to making the image\n");
		printf("-x              -- Extracts from the extfat image a file to a specified output file\n");
		printf("-D              -- Deletes the specified file from the extfat image\n\nHave fun!\n");
		exit(0);
	}
	if (CA.directoryFlag) //if the -d command line parameter is found
	{
		if(CA.inputFile == NULL)
		{
			printf("Input file needed to print the directories/sub-directories.\n");
		}
		else
		{
			directoryHandler(CA.inputFile);
		}
	}
	if (CA.extractFlag)
	{
		if(CA.inputFile == NULL)
		{
			printf("Input file needed to extract information.\n");
		}
		else if(CA.outputFile == NULL)
		{
			printf("Output file needed for file to be printed out to.\n");
		}
		else if(CA.extractFile == NULL)
		{
			printf("Filename needed for extraction.\n");
		}
		else
		{
			bool answer = dirHandler(CA.inputFile, CA.extractFile, CA.outputFile);
			if(answer == false)
			{
				printf("We could not find this file.\n");
			}
		}
	}
	if(CA.deleteFlag)
	{
		// delete functionality needs an input file specified
		if(CA.inputFlag)
		{
			directoryHandlerDelete(fi->inputfile, CA.deleteFile);
		}
		else
		{
			printf("Delete flag needs an input file specified!\n");
			exit(0);
		}
   	}

	// check that the offsets match the documentation
	assert(offsetof(Main_Boot, PercentInUse) == 112);

	// closes the source file once size was found
	//if(munmap(fi->MB, sizeof(Main_Boot)) == -1)
	if(fi-> MB && munmap(fi->MB, sizeof(Main_Boot)))
	{
		if(CA.inputFlag == false)
		{
			printf("Input file must be specified.\n");
		}
		else
		{
			perror("Error from unmap: ");
		}
	}

	if (close(fi->fd) && (CA.inputFile && CA.verifyFlag))
	{
		perror("error closing file:");
	}

	fi->fd = 0;

	if (close(fi->fd2) && (CA.inputFile && CA.verifyFlag))
	{
		perror("error closing file:");
	}

	fi->fd2 = 0;
	free(fi);
	return 0;
}
