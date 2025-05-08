#ifndef EXTFAT_H
#define EXTFAT_H
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include "cmdargs.h"

#define SECTOR_SIZE 512
#define OFFSET (12 * SECTOR_SIZE)
#define PAGE_ALIGN 4096 // calculate the offset for the backup boot sector

    typedef struct
    {
        char JumpBoot[3];
        char FileSystemName[8];
        char MustBeZero[53];
        long int PartitionOffset;
        long int VolumeLength;
        int FatOffset;
        int FatLength;
        int ClusterHeapOffset;
        int ClusterCount;
        int FirstClusterOfRootDirectory;
        int VolumeSerialNumber;
        short int FileSystemRevision;
        short int VolumeFlags;
        unsigned char BytesPerSectorShift;
        unsigned char SectorsPerClusterShift;
        unsigned char NumberOfFats;
        unsigned char DriveSelect;
        unsigned char PercentInUse;
        unsigned char Reserved[7];
        unsigned char BootCode[390];
        short int BootSignature;
        unsigned char ExcessSpace;
    } Main_Boot;

    typedef struct //GDS struct
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

    typedef struct //struct that stores whether the file is a directory or not
    {
        uint8_t EntryType;
        uint8_t SecondaryCount;
        uint8_t SetChecksum;
        struct
        {
            uint8_t ReadOnly : 1;
            uint8_t Hidden : 1;
            uint8_t System : 1;
            uint8_t Reserved_1 : 1;
            uint8_t Directory : 1;
            uint8_t Archive : 1;
            uint16_t Reserved_2 : 10;
        };
        uint16_t Reserved1;
        uint32_t CreateTimestamp;
        uint32_t LastModifiedTimestamp;
        uint32_t LastAccessedTimestamp;
        uint8_t Create10msIncrement;
        uint8_t LastModified10msIncrement;
        uint8_t CreateUtcOffset;
        uint8_t LastModifiedUtcOffset;
        uint8_t LastAccessedUtcOffset;
        uint64_t Reserved2 : 56;
    }DirEntry;
    
    typedef struct
    {             
        Main_Boot *MB;
        Main_Boot *BB;
        Main_Boot *BB_;
        CmdArgs CA;
        size_t size;
        void *FAT;
        void *Data; 
        char *filename;
        char *dot;
        size_t filesize;
        uint32_t mbrChecksum;
        uint32_t bbrChecksum;
        int fd;
        int fd2; // The file descriptor
        int exfatSize;
        void *SB;
        void *DB;
        int src;
        int des;
        int bytesPerSector;
        int bytespersector;
        int sectorsPerCluster;
        int SectorSize;
        int FileLength;
        char* inputfile;
        char* outputfile;
    }fileInfo;

    struct x
	{
		uint8_t EntryType : 8;
		uint8_t GeneralSecondaryFlags : 8;
		uint8_t FileName[30];
	};

    typedef struct StreamExtensionEntry
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
        union
        {
            uint8_t  GeneralSecondaryFlags;
            struct
            {
                uint8_t AllocationPossible : 1;
                uint8_t NoFatChain : 1;
                uint8_t Reserved : 6;
            };
        };
        uint8_t  Reserved1;
        uint8_t  NameLength;
        uint16_t NameHash;
        uint8_t  Reserved2[2];
        uint8_t  ValidDataLength[8];
        uint8_t  Reserved3[4];
        uint32_t FirstCluster;
        uint64_t DataLength;
    } StreamExtensionEntry;

    void compareBoot(void* MB, void* BB, int bytespersector);
    void compareBoot(void* MB, void* BB, int bytespersector);
    int ftruncate(int fd, off_t length);
    int openMapFile(char *filename);
    void createFile(char *filename, int size);
    void closeMap(void *MB, int size, int fd);
    void directoryHandler(char *inputFile);
    bool extract(char* extFile, int offset, Main_Boot* MB, int bytesPerSector, int sectorsPerCluster, void *fp, FILE* fd);
    bool dirHandler(char* inputFile, char* extractionFile, char* output);
    void directoryHandlerDelete(char* inputFile, char* fileName);
    void delete(GenericDirectoryStruct *GDS, uint32_t *FAT, int i, int* counter, int* clusterArr);
    void deleteFile(int offset, int level, Main_Boot* MB, int bytesPerSector, int sectorsPerCluster, void *fp, uint32_t *FAT, char* fileName);
    
#endif	/* EXTFAT_H */