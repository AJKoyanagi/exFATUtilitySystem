# exFATUtilitySystem
Collecting workspace information# exFATUtilitySystem

## Overview

exFATUtilitySystem is a comprehensive utility for working with exFAT filesystem images, developed as part of the 2023 Spring UTA CSE-3310 course. This tool provides a robust set of operations for managing exFAT images including copying, verification, directory listing, file extraction, and file deletion.

## Features

exFATUtilitySystem provides the following functionality:

- **Copy**: Copy exFAT images or regular files
- **Verify**: Validate exFAT filesystem structure by comparing main and backup boot sectors
- **List Directories**: Display directories and files within an exFAT image
- **Extract Files**: Extract specific files from an exFAT image
- **Delete Files**: Remove files from an exFAT image

## Building the Project

The project uses a Makefile for compilation:

```bash
# Clean previous builds
make clean

# Compile the project
make

# Run the utility
./extfat [options]
```

## Command Line Options

The utility supports the following command line options:

| Option | Description |
|--------|-------------|
| `-h` | Display help information and exit |
| `-i inputfile` | Specify the input file (required for most operations) |
| `-o outputfile` | Specify the output file for copy/extract operations |
| `-c` | Copy flag (requires input and output files) to copy file without changes |
| `-v` | Verify flag (requires input file) to check exFAT structure validity |
| `-d` | Print directories and subdirectories in the exFAT image |
| `-x filename` | Extract specified file from the exFAT image to output file |
| `-D filename` | Delete specified file from the exFAT image |

## Usage Examples

### Display Help
```bash
./extfat -h
```

### Copy an exFAT Image
```bash
./extfat -c -i source.image -o destination.image
```

### Verify an exFAT Image
```bash
./extfat -v -i filesystem.image
```

### List Files and Directories
```bash
./extfat -d -i filesystem.image
```

### Extract a File
```bash
./extfat -i filesystem.image -x document.txt -o extracted_document.txt
```

### Delete a File
```bash
./extfat -i filesystem.image -D unwanted_file.txt
```

## Technical Implementation

The utility uses memory-mapped I/O (mmap) for efficient filesystem access and manipulation. It implements the exFAT specification including:

- Main Boot and Backup Boot record handling and verification
- FAT (File Allocation Table) chain traversal
- Directory structure navigation
- File operations (read, extract, delete)

The implementation works with the exFAT filesystem structure including sectors, clusters, and directory entries as specified in the exFAT specification.

## Testing

The project includes several test suites:

- **Unit Tests**: Individual component testing using the munit framework
- **System Tests**: End-to-end testing of the utility's functionality
- **Shell Scripts**: Automated tests for specific features

Example test scripts are located in the tests directory and can be run individually or as part of a test suite.

## Requirements

- C compiler with C99 support
- POSIX-compliant operating system (Linux recommended)
- System libraries: stdio, stdlib, string, sys/mman, sys/types, sys/stat, etc.

## License

This project is a class assignment for CSE3310 and is provided for educational purposes.

---

*Note: This utility is designed primarily for educational purposes as part of a class project for CSE3310 FUNDAMENTALS OF SOFTWARE ENGR.*
