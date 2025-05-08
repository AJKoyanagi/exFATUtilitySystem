#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include "munit.h"
#include "cmdargs.h"
#include <unistd.h>
#include "extfat.h"
#include "routines.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


#define FILENAME "test.image"
#define FILESIZE 1024


#define BUF_SIZE 4096


fileInfo fi = {
    .MB = NULL,
    .BB = NULL,
    .BB_ = NULL,
    .CA = {0},
    .size = 0,
    .FAT = NULL,
    .Data = NULL,
    .filename = NULL,
    .dot = NULL,
    .filesize = 0,
    .mbrChecksum = 0,
    .bbrChecksum = 0,
    .fd = 0,
    .fd2 = 0,
    .exfatSize = 0,
    .SB = NULL,
    .DB = NULL,
    .src = 0,
    .des = 0,
    .bytesPerSector = 0,
    .bytespersector = 0,
    .sectorsPerCluster = 0,
    .SectorSize = 0,
    .FileLength = 0,
    .inputfile = NULL,
    .outputfile = NULL
};


static void* map_file(int fd, size_t size)
{
    void* buf = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    return buf;
}


MunitResult test_closeMap(const MunitParameter params[], void* fixture)
{
    (void)params;
    (void)fixture;
    int fd = open("test.image", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    void* buf = map_file(fd, BUF_SIZE);
    closeMap(buf, BUF_SIZE, fd);

    return MUNIT_OK;
}
MunitResult test_process_args(const MunitParameter* params,void* data)
{
    (void)data;
    (void)params;
    CmdArgs CA = {0};
    int argc = 9;
    char* argv[] = {"extfat", "-i", "input.image", "-o", "output.image", "-v", "-c", "-h", "-d"};

    process_args(&CA, argc, argv);

    munit_assert_true(CA.inputFlag);
    munit_assert_string_equal(CA.inputFile, "input.image");

    munit_assert_true(CA.outputFlag);
    munit_assert_string_equal(CA.outputFile, "output.image");

    munit_assert_true(CA.verifyFlag);
    munit_assert_true(CA.copyFlag);
    munit_assert_true(CA.helpFlag);
    munit_assert_true(CA.directoryFlag);
   

    

    return MUNIT_OK;
}
const uint8_t SAMPLE_MB[SECTOR_SIZE] = {
    0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x08, 0x01, 0x00,
    
};

// Define a sample backup boot record with a valid checksum
const uint8_t SAMPLE_BB[SECTOR_SIZE] = {
   0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x08, 0x01, 0x00,
   
};

static MunitResult test_compare_boot(const MunitParameter params[], void* fixture)
{
    (void)params;
    (void)fixture;

    fi.MB = (Main_Boot*)SAMPLE_MB;
    fi.BB = (Main_Boot*)SAMPLE_BB;

    uint32_t mbrChecksum = BootChecksum((uint8_t*)fi.MB, fi.bytespersector);
    uint32_t bbrChecksum = BootChecksum((uint8_t*)fi.BB, fi.bytespersector);


    munit_assert_uint32(mbrChecksum, ==, bbrChecksum);

    return MUNIT_OK;
}
static void *
test_setup(const MunitParameter params[], void *user_data)
{
    (void)params;
    (void)user_data;
    return NULL;
}

static MunitResult
test_createFile(const MunitParameter params[], void* fixture)
{
    (void)params;
    (void)fixture;
    createFile(FILENAME, FILESIZE);
    FILE *fp = fopen(FILENAME, "r");
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fclose(fp);
    munit_assert_int(file_size, ==, FILESIZE);
    return MUNIT_OK;
}
MunitResult test_openMapFile(const MunitParameter params[], void* fixture) {
    (void) params;
    (void) fixture;
    char* filename = "test.image";
    int fd = openMapFile(filename);

    munit_assert_int(fd, !=, -1);
    close(fd);
    return MUNIT_OK;
}



static MunitTest tests[] = {
    { (char*) "Testing Close Map function \n ", test_closeMap, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    {"Testing Command Line Function \n", test_process_args, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"Testing Checksum function \n",test_compare_boot,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
    {(char*) "Testing Create file Function \n", test_createFile, test_setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*) "Testing OpenMap function \n", test_openMapFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

};

static const MunitSuite suite = {
    (char*) "Running Unit tests for ",
    tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};


int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    return munit_suite_main(&suite, NULL, argc, argv);
}