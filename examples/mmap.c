/*
   This is an example of using mmap to read an extFAT image file.
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
#include <stddef.h>
#include <stdint.h>

#include "extfat.h"
#include "routines.h"

// given a number N, this macro returns a pointer in the mmapped file
#define cluster(N) ((fp + MB->ClusterHeapOffset * bytesPerSector) + ((N - 2) * bytesPerSector * sectorsPerCluster))

int main()
{
   // open the file system image file
   int fd = open("test.image", O_RDWR);
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
   printf("The file size is %ld\n", size);

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

   // print out some things we care about

   printf("FileSystemName %s\n", MB->FileSystemName); // warning, not required to be terminated
   printf("\n");
   printf("BytesPerSectorShift %d\n", MB->BytesPerSectorShift);
   printf("SectorsPerClusterShift %d\n", MB->SectorsPerClusterShift);
   printf("NumberOfFats %d\n", MB->NumberOfFats);

   int bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
   int sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

   printf("-----> the bytesPerSector are %d\n", bytesPerSector);
   printf("-----> the sectorsPerCluster are %d\n", sectorsPerCluster);

   // next, the Backup Boot record
   Main_Boot *BBR = (Main_Boot *)(fp + 12 * bytesPerSector);
   printf("BackUpBootRecord %p\n", BBR);
   printf("FileSystemName %s\n", BBR->FileSystemName); // warning, not required to be terminated

   // Checksum
   uint32_t mbrChecksum = BootChecksum((uint8_t*) MB, (short) bytesPerSector);
   uint32_t bbrChecksum = BootChecksum((uint8_t*) BBR, (short) bytesPerSector);
   printf("Checksum  (MB) %x (BBR) %x\n",mbrChecksum,bbrChecksum);
 
   // The checksum in the disk image
   uint32_t *cksum;
   cksum = (void *)MB + (11 * bytesPerSector);
   for (unsigned int i = 0; i < 4; i++)
      printf("in the image, the checksum %x ", cksum[i]);
   printf("\n");

   // FAT 1 and FAT 2
   // (sometimes there is no FAT2)
   printf("NumberOfFats %d\n", MB->NumberOfFats);
   printf("FatOffset %d\n", MB->FatOffset); // in sectors
   printf("FatLength %d\n", MB->FatLength);
   uint32_t *FAT = (uint32_t *)((void *)fp + (MB->FatOffset * bytesPerSector));
   printf("FAT[0] is at absolute %p relative %8x\n", FAT, MB->FatOffset * bytesPerSector);
   printf("FAT[1] is at absolute %p \n", &FAT[1]);

   // array notation is used to access.
   printf("  %x %x \n", FAT[0], FAT[1]);
   printf("  %x %x \n", FAT[2], FAT[3]);
   printf("  %x %x \n", FAT[4], FAT[5]);

   // defined as always these values
   assert(FAT[0] == 0xfffffff8);
   assert(FAT[1] == 0xffffffff);

   printf("FAT chains\n");
   for (int i = 2; i < MB->ClusterCount - 1; i++)
   {
      switch (FAT[i])
      {
      case 0x00000000:
         break;
      case 0xfffffff7:
         printf("BAD");
         break;
      case 0xffffffff:
         // end of chain
         printf("\n");
         break;
      default:
         printf(" %d", FAT[i]);
         break;
      }
   }
   printf("end of FAT\n");

   // clusters
   //     (cluster(2) is the first one)
   printf("cluster  2 heap starts at %p relative %lx\n", cluster(2), cluster(2) - fp);
   printf("cluster  3 heap starts at %p relative %lx\n", cluster(3), cluster(3) - fp);
   printf("cluster  4 heap starts at %p relative %lx\n", cluster(4), cluster(4) - fp);
   printf("cluster  5 heap starts at %p relative %lx\n", cluster(5), cluster(5) - fp);
   printf("cluster  6 heap starts at %p relative %lx\n", cluster(6), cluster(6) - fp);
   printf("cluster  7 heap starts at %p relative %lx\n", cluster(7), cluster(7) - fp);

   typedef struct
   {
      union
      {
         uint8_t EntryType;
         struct
         {
            uint8_t TypeCode : 5;
            uint8_t TypeImportance : 1;
            uint8_t TypeCategory : 1;
            uint8_t InUse : 1;
         };
      };
      uint8_t CustomDefined[19];
      uint32_t FirstCluster;
      uint64_t DataLength;
   } GenericDirectoryStruct;

   printf("The size of GenericDirectoryStruct %ld %lx\n", sizeof(GenericDirectoryStruct), sizeof(GenericDirectoryStruct));
   assert(sizeof(GenericDirectoryStruct) == 32);

   // directory
   GenericDirectoryStruct *GDS = cluster(MB->FirstClusterOfRootDirectory);
   printf("FirstClusterofRootDirectory %d\n", MB->FirstClusterOfRootDirectory);

   printf("The directory listing\n");
   // The directory listings are complicated.  This code just extracts the file names.
   // it does not handle directories.
   int i = 0;
   while (GDS[i].EntryType)
   {
      // printf("%8lx\tThe Entry Type is %x  \tDataLength %8ld \tFirstCluster %d ", (void *)&GDS[i] - fp, GDS[i].EntryType, GDS[i].DataLength, GDS[i].FirstCluster);
      if (GDS[i].InUse && GDS[i].EntryType == 0xc1)
      {
         // This is a temporary structure
         // Needs to be 'well defined' like the generic one, above
         struct x
         {
            uint8_t EntryType : 8;
            uint8_t GeneralSecondaryFlags : 8;
            uint8_t FileName[30];
         };

         struct x *X = (struct x *)&GDS[i];
         if (X->EntryType == 0xc1)
         {
            char *ch = (char*) X->FileName;
            printf("\t");
            while (*ch)
            {
               printf("%c", *ch);
               // go to the next, and skip one
               ch++;
               ch++;
            }
            printf("\n");
            // eat the next 2 directory entries
            i++;
            i++;
         }
      }
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

   return 0;
}