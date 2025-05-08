#include "extfat.h"
#include "cmdargs.h"

// given a number N, this macro returns a pointer in the mmapped file
#define cluster(N) ((fp + MB->ClusterHeapOffset * bytesPerSector) + ((N - 2) * bytesPerSector * sectorsPerCluster))

void delete(GenericDirectoryStruct *GDS, uint32_t *FAT, int i, int* counter, int* clusterArr)
{
    // change the entry type
    if(GDS[i].EntryType == 0xc1)
    {
        GDS[i].EntryType = 0x41;
    }
    
    // initialize the clusters to be deleted
    uint32_t fCluster = GDS[i-1].FirstCluster;
    uint32_t currCluster = fCluster;

    // flip the inUse bit to 0
    GDS[i].InUse = 0;

    // check if there is a chain of clusters 
    if((GDS[i-1].CustomDefined[0] & 0b00000010) == 0)
    {
        // delete each cluster in the chain
        int j = 0;
        while(currCluster != 0xfffffff8 && currCluster != 0xffffffff)
        {
            uint32_t nCluster = FAT[currCluster];
            clusterArr[j] = currCluster; 
            FAT[currCluster] = 0;
            currCluster = nCluster;
            *counter = *counter + 1;
            j++;
        }
        
        // store the last cluster in the cluster array as well
        clusterArr[j] = currCluster;
    }

    // write the changes to memory
    char* pos = (char*) GDS + (i * sizeof(GenericDirectoryStruct));
    memcpy(pos, &GDS[i], sizeof(GenericDirectoryStruct));
}

void deleteFile(int offset, int level, Main_Boot* MB, int bytesPerSector, int sectorsPerCluster, void *fp, uint32_t *FAT, char* fileName) //recursively deletes the files
{
    uint8_t prevEntryType;
	GenericDirectoryStruct* GDS = cluster(offset);
	int i = 0;

	while (GDS[i].EntryType)
	{
		// This is a temporary structure
		// Needs to be 'well defined' like the generic one, above
		struct x
		{
			uint8_t EntryType : 8;
			uint8_t GeneralSecondaryFlags : 8;
			uint8_t FileName[30];
		};
		
		if (GDS[i].InUse && GDS[i].EntryType == 0x85)
		{
			DirEntry *Dir = (DirEntry *)&GDS[i];
			if(Dir->Directory == 1)
			{
				i += 2; //get to the file name
				struct x *X = (struct x *)&GDS[i];
                char thisfile[100];
				memset(thisfile, 0, 100);
				int t = 0;

                // get filename
				if(X->EntryType == 0xc1)
				{
					char *ch = (char*) X->FileName;
					while (*ch)
					{
						if(isprint(*ch))
						{
                            thisfile[t] = *ch;
							t++;
						}
                                
						// go to the next, and skip one
						ch+=2;
					}
				}

				i-=1; //get to stream extension
				deleteFile(GDS[i].FirstCluster, level+1, MB, bytesPerSector, sectorsPerCluster, fp, FAT, fileName);              
				i+=1;
			}
		}
		else if(GDS[i].InUse && GDS[i].EntryType == 0xc1) // prints the subdirectories
		{
			struct x *X = (struct x *)&GDS[i];
			char *ch = (char*) X->FileName;
            char thisfile[100];
			memset(thisfile, 0, 100);
			int t = 0;

            // get filename
			while (*ch)
			{
				if(prevEntryType != 0xc1 && isprint(*ch))
				{
					thisfile[t] = *ch;
					t++;
				}

				// go to the next, and skip one
				ch+=2;
			}

            // if current filename matches to delete file then delete
            if(!strcmp(thisfile, fileName))
            {
                int counter = 0;
                int clusterArr[10000];

                printf("Found file to be deleted: %s\n", thisfile);
                delete(GDS, FAT, i, &counter, clusterArr);  

                // if there is no chain, clear the memory of the first and only cluster   
                if(counter == 0)
                {
                    memset(cluster(GDS[i-1].FirstCluster), 0, bytesPerSector * sectorsPerCluster);
                }
                else
                {
                    // clear the memory of every cluster in the chain
                    for (int a = 0; a < counter + 1; a++)
                    {
                        memset(cluster(clusterArr[a]), 0, bytesPerSector * sectorsPerCluster);
                    }
                }

                printf("%s has been deleted\n", thisfile);            
                exit(0);
            }
		}
		prevEntryType = GDS[i].EntryType;
		i++;
	}
}

void directoryHandlerDelete(char* inputFile, char* fileName)
{
    // open the file system image file
    int fd = openMapFile(inputFile);
    if (fd == -1)
    {
        perror("file open: ");
        exit(0);
    }

    printf("Inside delete directory handler...\n");

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
                            PROT_READ | PROT_WRITE, //need write permission to modify the file
                            MAP_SHARED,
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

	int i = 0;
    while (GDS[i].EntryType)
    {
        // This is a temporary structure
        // Needs to be 'well defined' like the generic one, above
        struct x
        {
            uint8_t EntryType : 8;
            uint8_t GeneralSecondaryFlags : 8;
            uint8_t FileName[30];
        };
        
        if(GDS[i].InUse && GDS[i].EntryType == 0xc1) //directories including mmap.c
        {
            struct x *X = (struct x *)&GDS[i];
            char *ch = (char*) X->FileName;
			char thisfile[100];
			memset(thisfile, 0, 100);
			int t = 0;

            // get filename
            while (*ch)
            {
                if(prevEntryType != 0xc1 && isprint(*ch))
                {
                    // stores the filename
					thisfile[t] = *ch;
					t++;
                }
                
                // go to the next, and skip one
                ch+=2;
            }

            // if current filename matches then delete the file
            if(!strcmp(thisfile, fileName))
            {
                int counter = 0;
                int clusterArr[10000];

                printf("Found file to be deleted: %s\n", thisfile);
                delete(GDS, FAT, i, &counter, clusterArr);  

                // if there is no chain, clear the memory of the first and only cluster   
                if(counter == 0)
                {
                    memset(cluster(GDS[i-1].FirstCluster), 0, bytesPerSector * sectorsPerCluster);
                }
                else
                {
                    // clear the memory of every cluster in the chain
                    for (int a = 0; a < counter + 1; a++)
                    {
                        memset(cluster(clusterArr[a]), 0, bytesPerSector * sectorsPerCluster);
                    }
                }

                printf("%s has been deleted\n", thisfile);            
                exit(0);
            }
        }
        else if (GDS[i].InUse && GDS[i].EntryType == 0x85)
        {
            DirEntry *Dir = (DirEntry *)&GDS[i];
            if(Dir->Directory == 1)
            {
                i+=2; //get to the file name
                struct x *X = (struct x *)&GDS[i];
                char thisfile[100];
				memset(thisfile, 0, 100);
				int t = 0;

                if(GDS[i].EntryType== 0xc1) //printing it twice
                {
                    char *ch = (char*) X->FileName;

                    // get filename
					while (*ch)
                    {
                        if(isprint(*ch))
                        {
                            // stores the filename
							thisfile[t] = *ch;
							t++;
                        }
                        
                        // go to the next, and skip one
                        ch+=2;
                    }
                }

                i -= 1; //get to stream extension
                deleteFile(GDS[i].FirstCluster, 1, MB, bytesPerSector, sectorsPerCluster, fp, FAT, fileName);
                i += 1; //flextape
            }
        }
        prevEntryType = GDS[i].EntryType;
        i++;
	}

    // if we reach this line, then that means delete function was not called
    // otherwise the program would've exit immediately after delete was called
    printf("Delete file not found!\n");

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
