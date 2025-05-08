#include "extfat.h"
#include "cmdargs.h"

// given a number N, this macro returns a pointer in the mmapped file
#define cluster(N) ((fp + MB->ClusterHeapOffset * bytesPerSector) + ((N - 2) * bytesPerSector * sectorsPerCluster))

void printDir(int offset, int level, Main_Boot* MB, int bytesPerSector, int sectorsPerCluster, void *fp) //recursively prints and traverses the files
{
	uint8_t prevEntryType;
	GenericDirectoryStruct* GDS = cluster(offset);
	int i = 0;
	while (GDS[i].EntryType)
	{
		if (GDS[i].InUse && GDS[i].EntryType == 0x85)
		{
			DirEntry *Dir = (DirEntry *)&GDS[i];
			if(Dir->Directory == 1)
			{
				i += 2; //get to the file name
				struct x *X = (struct x *)&GDS[i];
				if(X->EntryType == 0xc1)
				{
					for(int i = 0; i < level; i++)
					{
						printf("\t");
					}
					char *ch = (char*) X->FileName;
					while (*ch)
					{
						if(isprint(*ch))
						{
							printf("%c", *ch);
						}
						
						// go to the next, and skip one
						ch+=2;
					}
				}
				printf("\n");
				i-=1; //get to stream extension
				printDir(GDS[i].FirstCluster, level+1, MB, bytesPerSector, sectorsPerCluster, fp);
				i+=1;
			}
		}
		else if(GDS[i].InUse && GDS[i].EntryType == 0xc1) // prints the subdirectories
		{
			struct x *X = (struct x *)&GDS[i];
			char *ch = (char*) X->FileName;
			for(int i = 0; i < level; i++)
			{
				if(prevEntryType != 0xc1)
				{
					printf("\t");
				}
			}
			while (*ch)
			{
				if(prevEntryType != 0xc1 && isprint(*ch))
				{
					printf("%c", *ch);
				}
				// go to the next, and skip one
				ch+=2;
			}
			if(prevEntryType != 0xc1)
			{
				printf("\n");
			}
		}
		prevEntryType = GDS[i].EntryType;
		i++;
	}
}

void directoryHandler(char* inputFile)
{
	// open the file system image file
	int fd = openMapFile(inputFile);
	if (fd == -1)
	{
		perror("file open: ");
		exit(0);
	}

	off_t size = 0;
	struct stat statbuf;
	if (fstat(fd, &statbuf))
	{
		perror("stat of file:");
		exit(0);
	}
	size = statbuf.st_size;

	// mmap the entire file into memory
	// every data item we reference, will be relative to fp...
	void *fp = (void *)mmap(NULL,
							size,
							PROT_READ,
							MAP_PRIVATE,
							fd,
							0); // note the offset

	if (fp == (void *)-1)
	{
		perror("mmap:");
		exit(0);
	}

	// first, is the Main Boot record
	Main_Boot *MB = (Main_Boot *)fp;

	int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
	int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

	uint32_t *FAT = (uint32_t *)((void *)fp + (MB->FatOffset * bytesPerSector));

	assert(FAT[0] == 0xfffffff8);
	assert(FAT[1] == 0xffffffff);

	assert(sizeof(GenericDirectoryStruct) == 32);

	// directory
	uint8_t prevEntryType;
	GenericDirectoryStruct *GDS = cluster(MB->FirstClusterOfRootDirectory);

    printf("The directory listing\n");

    int i = 0;
    while (GDS[i].EntryType)
    {   
        if(GDS[i].InUse && GDS[i].EntryType == 0xc1) //directories including mmap.c
        {
            struct x *X = (struct x *)&GDS[i];
            char *ch = (char*) X->FileName;
            while (*ch)
            {
                if(prevEntryType != 0xc1 && isprint(*ch))
                {
                    printf("%c", *ch);
                }
                
                // go to the next, and skip one
                ch+=2;
            }
            printf("\n");
        }
        else if (GDS[i].InUse && GDS[i].EntryType == 0x85)
        {
            DirEntry *Dir = (DirEntry *)&GDS[i];
            if(Dir->Directory == 1)
            {
                i+=2; //get to the file name
                struct x *X = (struct x *)&GDS[i];
                if(GDS[i].EntryType== 0xc1) //printing it twice
                {
                    char *ch = (char*) X->FileName;
                    while (*ch)
                    {
                        if(isprint(*ch))
                        {
                            printf("%c", *ch);
                        }
                        
                        // go to the next, and skip one
                        ch+=2;
                    }
                }
                printf("\n");
                i -= 1; //get to stream extension
                printDir(GDS[i].FirstCluster, 1, MB, bytesPerSector, sectorsPerCluster, fp);
                i += 1;
            }
        }
        prevEntryType = GDS[i].EntryType;
        i++;
	}


	// unmap the file
	if (munmap(fp, size))
	{
		perror("error from unmap:");
		exit(0);
	}

	// close the file
	if (close(fd))
	{
		perror("close:");
	}
	fd = 0;
}