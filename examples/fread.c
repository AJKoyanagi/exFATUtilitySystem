/*

   This is an example of using lseek / fread to read an extFAT image file.

   To make the sample file:

    % # create the file system image
    % dd if=/dev/zero of=test.image count=1 bs=1G
    % sudo losetup /dev/loop2 test.image
    % sudo /usr/sbin/mkexfatfs /dev/loop2

    % # put something in the file system image
    % mkdir /tmp/d
    % sudo mount /dev/loop2 /tmp/d
    % cp examples/mmap.c /tmp/d

    % # clean up
    % sudo umount /tmp/d
    % sudo losetup -d /dev/loop2
    % rm -rf /tmp/d
    % rm test.image



   Written by Bud Davis, jimmie.davis@uta.edu
   (c) 2023, All Rights Reserved
   Provided to students of CSE3310, UTA. Any use
   other than this course is prohibited.

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "extfat.h"

int main()
{
   // open the file system image file
   FILE *fd = fopen("test.image", "rw");

   // create a place to read into
   Main_Boot *MB = NULL;
   MB = (Main_Boot *)malloc(sizeof(Main_Boot));
   if (!MB)
   {
      perror("error from malloc:");
      exit(0);
   }

   // read it in
   int numStructs = fread(MB, sizeof(Main_Boot), 1, fd);

   if (numStructs != 1)
   {
      printf("not enough bytes read\n");
      exit(0);
   }

   // print out some things we care about

   printf("the pointer to MB is %p  \n", MB);

   printf("JumpBoot  %d %d %d \n", MB->JumpBoot[0], MB->JumpBoot[1], MB->JumpBoot[2]);
   printf("FileSystemName %s\n", MB->FileSystemName); // warning, not required to be terminated
   printf("PartitionOffset %ld\n", MB->PartitionOffset);
   printf("VolumeLength %ld\n", MB->VolumeLength);
   printf("FatOffset %d\n", MB->FatOffset);
   printf("FatLength %d\n", MB->FatLength);
   printf("ClusterHeapOffset %d\n", MB->ClusterHeapOffset);
   printf("ClusterCount %d\n", MB->ClusterCount);
   printf("FirstClusterofRootDirectory %d\n", MB->FirstClusterOfRootDirectory);
   printf("VolumeSerialNumber %x\n", MB->VolumeSerialNumber);

   // some skipped


   printf("BytesPerSectorShift %d\n",MB->BytesPerSectorShift);
   printf("SectorsPerClusterShift %d\n",MB->SectorsPerClusterShift);
   printf("NumberOfFats %d\n",MB->NumberOfFats);

   int bytesPerSector = 2 << (MB->BytesPerSectorShift-1);
   int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift-1);

   printf("-----> the bytesPerSector are %d\n",bytesPerSector);
   printf("-----> the sectorsPerCluster are %d\n",sectorsPerCluster);


      
   // return the malloced memory
   free(MB);
   MB = NULL;  // a good practice, but not essential here

   // close the file
   if (fclose(fd))
   {
      perror("closeStat:");
   }
   fd = NULL;

   return 0;
}