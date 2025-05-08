#include "extfat.h"

// creates and returns an integer representing a file descriptor
int openMapFile(char *filename)
{
   int fd = open(filename, O_RDWR);
   if (fd == -1)
   {
      perror("input file failed to open");
      exit(0);
   }
   
   return fd;
}

// function that opens a filepointer and sets the size 
void createFile(char *filename, int size)
{
   FILE *fp;
   fp = fopen(filename, "w");
   fseek(fp, size-1, SEEK_END);
   char byte[] = "";
   fwrite(byte , 1 , 1 , fp);
   printf("File: %s has been assigned its size and will be closed.\n\n", filename);
   fclose(fp);   
}

// function that closes a map and file
void closeMap(void *MB, int size, int fd)
{
   if (munmap(MB, size) == -1)
   {
      perror("error from unmap");
      exit(0);
   }

   printf("Map successfully closed.\n\n");
   close(fd);  
}
