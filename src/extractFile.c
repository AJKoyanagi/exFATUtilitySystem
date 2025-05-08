#include "extfat.h"
#include "cmdargs.h"

// given a number N, this macro returns a pointer in the mmapped file
#define cluster(N) ((fp + MB->ClusterHeapOffset * bytesPerSector) + ((N - 2) * bytesPerSector * sectorsPerCluster))

bool found = false;

//use the firstcluster to traverse
bool extract(char* extFile, int offset, Main_Boot* MB, int bytesPerSector, int sectorsPerCluster, void *fp, FILE* fd) //recursively prints and traverses the files
{
	uint32_t *FAT = (uint32_t *)((void *)fp + (MB->FatOffset * bytesPerSector));
	uint8_t prevEntryType;
	GenericDirectoryStruct* GDS = cluster(offset);
	int i = 0;
	char filename[30];
	while (GDS[i].EntryType && !found)
	{
		if (GDS[i].InUse && GDS[i].EntryType == 0x85)
		{
			DirEntry *Dir = (DirEntry *)&GDS[i];
			if(Dir->Directory == 1)
			{
				char filename[30];
				memset(filename, 0, sizeof(filename));
				i += 2; //get to the file name
				struct x *X = (struct x *)&GDS[i];
				if(X->EntryType == 0xc1)
				{
					int j = 0;
					char *ch = (char*) X->FileName;
					while (*ch)
					{
						if(isprint(*ch))
						{
							filename[j] = *ch;
							j++;
						}
						
						// go to the next, and skip one
						ch+=2;
					}
				}
				if(!strcmp(filename, extFile))
				{
					printf("%s cannot be extracted.\n", filename);
					exit(0);
				}
				memset(filename, 0, sizeof(filename));
				i-=1; //get to stream extension
				extract(extFile, GDS[i].FirstCluster, MB, bytesPerSector, sectorsPerCluster, fp, fd);
				i+=1;
			}
		}
		else if(GDS[i].InUse && GDS[i].EntryType == 0xc1) // prints the subdirectories
		{
			struct x *X = (struct x *)&GDS[i];
			char *ch = (char*) X->FileName;
			int j = 0;
			while (*ch)
			{
				if(prevEntryType != 0xc1 && isprint(*ch))
				{
					filename[j] = *ch;
					j++;
				}
				// go to the next, and skip one
				ch+=2;
			}
			if(!strcmp(extFile, filename))
			{
				StreamExtensionEntry *SEE = (StreamExtensionEntry *)&GDS[i-1];
				uint64_t x = SEE->FirstCluster;
				if(x != 0)
				{
					while(x != 0xffffffff)
					{
						char *contents = cluster(x);
						for(int i = 0; i < 4096 && *contents; i++)
						{
							fwrite(contents, 1, 1, fd);
							contents++;
						}
						if(SEE->NoFatChain == 0)
						{
							x = FAT[x];
						}
						else
						{
							break;
						}
					}
					printf("The file was extracted successfully.\n");
				}
				found = true;
			}
			memset(filename, 0, sizeof(filename));
		}
		prevEntryType = GDS[i].EntryType;
		i++;
	}
	return found;
}

bool dirHandler(char* inputFile, char* extractionFile, char* output)
{
	// open the file system image file
	printf("The file to extract is %s\n\n", extractionFile);
	printf("Inside extract directory handler\n");
	int fd = openMapFile(inputFile);
	FILE* fdx = fopen(output, "w");
	bool answer;
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
							PROT_READ | PROT_WRITE,
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
	uint8_t PrevEntry;
	GenericDirectoryStruct *GDS = cluster(MB->FirstClusterOfRootDirectory);

    int i = 0;
    while (GDS[i].EntryType)
    {   
        if(GDS[i].InUse && GDS[i].EntryType == 0xc1) //directories including mmap.c
        {
            struct x *X = (struct x *)&GDS[i];
            char *ch = (char*) X->FileName;
			int j = 0;
			char filename[30];
			while(*ch)
			{
				if(PrevEntry != 0xc1 && isprint(*ch))
				{
					filename[j] = *ch;
				}
				ch+=2;
				j++;
			}
			if(!strcmp(extractionFile, filename))
			{
				printf("%s cannot be extracted.\n", filename);
				exit(0);
			}
			memset(filename, 0, sizeof(filename));
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
					int j = 0;
					char filename[30];
					while(*ch)
					{
						filename[j] = *ch;
						ch+=2;
						j++;
					}
					if(!strcmp(extractionFile, filename))
					{
						printf("%s cannot be extracted.\n", filename);
						exit(0);
					}
					memset(filename, 0, sizeof(filename));
                }
                i -= 1; //get to stream extension
				if(!found)
				{
					answer = extract(extractionFile, GDS[i].FirstCluster, MB, bytesPerSector, sectorsPerCluster, fp, fdx);
				}
                i += 1;
            }
        }
        PrevEntry = GDS[i].EntryType;
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
	return answer;
}