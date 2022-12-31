//-----------------------------------------
// NAME: Saif Mahmud
// REMARKS: Reading ELF binary file with system calls of operating system. 
// Version: February, 2021
//-----------------------------------------
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#pragma pack(push)
#pragma pack(1)
typedef struct ELF_HEADER
{
    uint8_t machineClass;
    uint8_t endianness;
    uint8_t abi;
    uint16_t objectFileType;
    uint16_t instructionSetArchitechture;
    uint32_t entryPointAddress32;
    uint64_t entryPointAddress64;
    uint32_t programHeaderAddress32;
    uint64_t programHeaderAddress64;
    uint32_t sectionHeaderOffset32;
    uint64_t sectionHeaderOffset64;
    uint16_t sizeOfPHentry;
    uint16_t numOfPHentries;
    uint16_t sizeOfSHentry;
    uint16_t numOfSHentries;
    uint16_t secHDRtableIndex;

} elf_header ;
typedef struct P_HEADER
{
    uint32_t segmentType;
    uint32_t offset32;
    uint64_t offset64;
    uint32_t virtualAdd32;
    uint64_t virtualAdd64;
    uint32_t imageFileSize32;
    uint64_t imageFileSize64;
} p_header ;

typedef struct S_HEADER
{
    uint32_t shName;
    uint32_t shType;
    uint32_t virtualAdd32;
    uint64_t virtualAdd64;
    uint32_t secOffset32;
    uint64_t secOffset64;
    uint32_t secSize32;
    uint64_t secSize64;
} s_header ;
#pragma pack(pop)


// prototype
void read_elf_header(elf_header *, int);
void printHeader(elf_header *, char *);
void printPheader(p_header *, elf_header *, int, int);
void printSheader(s_header *, elf_header *, int, int);
void read_print_p_header (p_header *, elf_header *, int);
void read_print_s_header (s_header *, elf_header *, int);
char* endianType(uint8_t);
char* machineType(uint8_t);
void getSname(s_header *, elf_header *, int, int);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("No filename found.\nCheck readme.md for CLI instructions.");
    } else if(argc > 2){
        printf("Too many arguments. Only 1 expected.");
    } else {
        
        char *filename = argv[1];
        
        elf_header header;
        p_header pHeader;
        s_header sHeader;
        
        int elf_fd = open(filename, O_RDONLY);
        
        if(elf_fd == -1)
        {
            perror("Program exited with following code");
        } else {
            read_elf_header(&header, elf_fd);
            printHeader(&header, filename);
            read_print_p_header(&pHeader, &header, elf_fd);
            read_print_s_header(&sHeader, &header, elf_fd);
            close(elf_fd);
        }
        
        perror("Program exited with following code");
        printf("Program written by Saif Mahmud.\n\n");
    }
    return EXIT_SUCCESS;
}
void read_print_s_header (s_header *sHeader, elf_header *header, int handle){
    assert (sHeader != NULL);
    assert (header != NULL);
    assert (handle >= 0);

    //read all section fields
    if(header->machineClass == 1){
        //32 bit
        lseek(handle, header->sectionHeaderOffset32, SEEK_SET);
    } else if(header->machineClass == 2){
        //64 bit
       lseek(handle, header->sectionHeaderOffset64, SEEK_SET);
    }

    for(int i = 0; i < header->numOfSHentries; i++){
        
        read(handle, &sHeader->shName, 4);
        read(handle, &sHeader->shType, 4);
        
        if(header->machineClass == 1){
            //32 bit
            lseek(handle, 4, SEEK_CUR);
            read(handle, &sHeader->virtualAdd32, 4);
            read(handle, &sHeader->secOffset32, 4);
            read(handle, &sHeader->secSize32, 4);
            // lseek(handle, 16, SEEK_CUR);
        } else if(header->machineClass == 2){
            //64 bit
            lseek(handle, 8, SEEK_CUR);
            read(handle, &sHeader->virtualAdd64, 8);
            read(handle, &sHeader->secOffset64, 8);
            read(handle, &sHeader->secSize64, 8);
            // lseek(handle, 24, SEEK_CUR);
        }
        printSheader(sHeader, header, handle, i);
    }
}
void getSname(s_header *sHeader, elf_header *header, int handle, int loopNum){
    uint8_t out = -1;
    uint32_t temp = 0;
    uint64_t temp64 = 0;
    if(header->machineClass == 1){
        
        lseek(handle, header->sectionHeaderOffset32+(header->secHDRtableIndex*header->sizeOfSHentry), SEEK_SET);
        lseek(handle, 16, SEEK_CUR);
        read(handle, &temp, 4);
        lseek(handle, temp+sHeader->shName, SEEK_SET);
        
        printf("\t* section name ");
        while(out != 0){
            read(handle, &out, 1); 
            printf("%c", out);
        }
        printf("\n");
        //return back the seeker
        lseek(handle, header->sectionHeaderOffset32+((loopNum+1)*40), SEEK_SET);
    } else if(header->machineClass == 2) {
        lseek(handle, header->sectionHeaderOffset64+(header->secHDRtableIndex*header->sizeOfSHentry), SEEK_SET);
        lseek(handle, 24, SEEK_CUR);
        read(handle, &temp64, 8);
        lseek(handle, temp64+sHeader->shName, SEEK_SET);
        
        printf("\t* section name ");
        while(out != 0){
            read(handle, &out, 1); 
            printf("%c", out);
        }
        printf("\n");
        //return back the seeker
        lseek(handle, header->sectionHeaderOffset64+((loopNum+1)*64), SEEK_SET);
    }
}
void printSheader(s_header *sHeader, elf_header *header, int handle, int loopNum){
    printf("Section header: #%d\n", loopNum);
    getSname(sHeader, header, handle, loopNum);
    printf("\t* type 0x%02x\n", sHeader->shType);
    if(header->machineClass == 1){
        //32-bit
        printf("\t* virtual address of segment 0x%016x\n", sHeader->virtualAdd32);
        printf("\t* size in file %u bytes\n", sHeader->secSize32);
        printf("\t* first up to 32 bytes starting at 0x%016x\n", sHeader->secOffset32);
        lseek(handle, sHeader->secOffset32, SEEK_SET);
        
        int readUntil = 32;
        if(sHeader->secSize32 < 32){
            readUntil = sHeader->secSize32;
        }
        printf("\t  "); 
        for(int i = 0; i < 32; i++){
            uint8_t x = 0;
            if(i < readUntil){
                read (handle, &x, 1);
            }
            printf("%02x ", x);
            if(i == 15){
                printf("\n\t  ");
            }
            fflush(stdout);
        }
        
        printf("\n\n");
        lseek(handle, header->sectionHeaderOffset32+((loopNum+1)*40), SEEK_SET);
    } else if(header->machineClass == 2){
    //64-bit
        printf("\t* virtual address of segment 0x%" PRIx64 "\n", sHeader->virtualAdd64);
        printf("\t* size in file %lu bytes\n", sHeader->secSize64);
        printf("\t* first up to 32 bytes starting at 0x%" PRIx64 "\n", sHeader->secOffset64);
        lseek(handle, sHeader->secOffset64, SEEK_SET);
        
        int readUntil = 32;
        if(sHeader->secSize32 < 32){
            readUntil = sHeader->secSize64;
        }
        printf("\t  "); 
        for(int i = 0; i < 32; i++){
            uint8_t x = 0;
            if(i < readUntil){
                read (handle, &x, 1);
            }
            printf("%02x ", x);
            if(i == 15){
                printf("\n\t  ");
            }
            fflush(stdout);
        }
        
        printf("\n\n");
        lseek(handle, header->sectionHeaderOffset64+((loopNum+1)*64), SEEK_SET);
    }
}

void read_print_p_header (p_header *pHeader, elf_header *header, int handle) 
{
    assert (pHeader != NULL);
    assert (handle >= 0);

    //set seeker at program header
    
    if(header->machineClass == 1){
        lseek(handle, header->programHeaderAddress32, SEEK_SET);

    } else if(header->machineClass == 2){
        lseek(handle, header->programHeaderAddress64, SEEK_SET);
    }
    
    for(int i = 0; i < header->numOfPHentries; i++){

        read(handle, &pHeader->segmentType, 4);
        
        if((int)header->machineClass == 1){
            //32-bit
            
            read(handle, &pHeader->offset32, 4);
            read(handle, &pHeader->virtualAdd32, 4);
            lseek(handle, 4, SEEK_CUR);
            read(handle, &pHeader->imageFileSize32, 4);
            
        } else if((int)header->machineClass == 2){
            //64-bit";
            lseek(handle, 4, SEEK_CUR);
            read(handle, &pHeader->offset64, 8);
            read(handle, &pHeader->virtualAdd64, 8);
            lseek(handle, 8, SEEK_CUR);
            read(handle, &pHeader->imageFileSize64, 8);
        }
        printPheader(pHeader, header, handle, i);
    }
}

void printPheader(p_header *pHeader, elf_header *header, int handle, int loopNum){
    printf("Program header: #%d\n", loopNum);
    printf("\t* segment type 0x%08x\n", pHeader->segmentType);
    
    if((int)header->machineClass == 1){
        //32-bit
        printf("\t* virtual address of segment 0x%016x\n", pHeader->virtualAdd32);
        printf("\t* size in file %u bytes\n", pHeader->imageFileSize32);
        printf("\t* first up to 32 bytes starting at 0x%016x\n", pHeader->offset32);
        lseek(handle, pHeader->offset32, SEEK_SET);
        
        int readUntil = 32;
        if(pHeader->imageFileSize32 < 32){
            readUntil = pHeader->imageFileSize32;
        }
        printf("\t  "); 
        for(int i = 0; i < 32; i++){
            uint8_t x = 0;
            if(i < readUntil){
                read (handle, &x, 1);
            }
            printf("%02x ", x);
            if(i == 15){
                printf("\n\t  ");
            }
            fflush(stdout);
        }
        
        printf("\n\n");
        lseek(handle, header->programHeaderAddress32+((loopNum+1)*32), SEEK_SET);
    } else if((int)header->machineClass == 2){
        //64-bit";
        printf("\t* virtual address of segment 0x%" PRIx64 "\n", pHeader->virtualAdd64);
        printf("\t* size in file %lu bytes\n", pHeader->imageFileSize64);
        printf("\t* first up to 32 bytes starting at 0x%" PRIx64 "\n", pHeader->offset64);
        lseek(handle, pHeader->offset64, SEEK_SET);
        
        int readUntil = 32;
        if(pHeader->imageFileSize64 < 32){
            readUntil = pHeader->imageFileSize64;
        }
        printf("\t  "); 
        for(int i = 0; i < 32; i++){
            uint8_t x = 0;
            if(i < readUntil){
                read (handle, &x, 1);
            }
            printf("%02x ", x);
            if(i == 15){
                printf("\n\t  ");
            }
            fflush(stdout);
        }
        
        printf("\n\n");
        lseek(handle, header->programHeaderAddress64+((loopNum+1)*56), SEEK_SET);
    }
}

void read_elf_header (elf_header *header, int handle) 
{
    assert (header != NULL);
    assert (handle >= 0);

    //set seeker at machineClass
    lseek(handle, 0x04, SEEK_SET);
    read (handle, &header->machineClass, 1); 
    read (handle, &header->endianness, 1);

    //skip 1 byte
    lseek(handle, 1, SEEK_CUR);
    read (handle, &header->abi, 1);

    //skip 7+1 bytes
    lseek(handle, 8, SEEK_CUR);
    read (handle, &header->objectFileType, 2);
    read (handle, &header->instructionSetArchitechture, 2);

    lseek(handle, 0x18, SEEK_SET);
    if(header->machineClass == 1){
        //32-bit
        read (handle, &header->entryPointAddress32, 4);
        read (handle, &header->programHeaderAddress32, 4);
        read (handle, &header->sectionHeaderOffset32, 4);
        lseek(handle, 0x2A, SEEK_SET);

    } else if(header->machineClass == 2){
        //64-bit";
        read (handle, &header->entryPointAddress64, 8);
        read (handle, &header->programHeaderAddress64, 8);
        read (handle, &header->sectionHeaderOffset64, 8);
        lseek(handle, 0x36, SEEK_SET);
    }

    read (handle, &header->sizeOfPHentry, 2);
    read (handle, &header->numOfPHentries, 2);
    read (handle, &header->sizeOfSHentry, 2);
    read (handle, &header->numOfSHentries, 2);
    read (handle, &header->secHDRtableIndex, 2);
}

void printHeader(elf_header *header, char *fileName)
{
    printf("\nFile: %s\n\n", fileName);
    printf("ELF header:\n");

    printf("\t* %s\n", machineType(header->machineClass));
    printf("\t* %s\n", endianType(header->endianness));
    printf("\t* Compiled for 0x%02x (operating system)\n", header->abi);
    printf("\t* has type 0x%02x\n", header->objectFileType);
    printf("\t* Compiled for 0x%02x (isa)\n", header->instructionSetArchitechture);

    if((int)header->machineClass == 1){
        //32 bit
        printf("\t* entry point address 0x%016x\n", header->entryPointAddress32);
        printf("\t* program header table starts at 0x%016x\n", header->programHeaderAddress32);
    } else {
        //64 bit
        printf("\t* entry point address 0x%" PRIx64 "\n", header->entryPointAddress64);
        printf("\t* program header table starts at 0x%" PRIx64 "\n", header->programHeaderAddress64);
    }
    printf("\t* There are %d program headers, each is %d bytes\n", header->numOfPHentries, header->sizeOfPHentry);
    printf("\t* There are %d section headers, each is %d bytes\n", header->numOfSHentries, header->sizeOfSHentry);
    printf("\t* the section header string table is %d\n", header->secHDRtableIndex);
    printf("\n");
}

char* machineType(uint8_t class)
{
    char *output = NULL;
    if(class == 1){
        output = "32-bit";
    } else if(class == 2){
        output = "64-bit";
    } else {
        char* temp = (char*)&class;
        output = strcat("Unrecognized name but the value is: ",temp);
    }
    return output;
}

char* endianType(uint8_t endianVal)
{
    char *output = NULL;
    if(endianVal == 1){
        output = "little endian";
    } else if(endianVal == 2){
        output = "big endian";
    } else {
        char* temp = (char*)&endianVal;
        output = strcat("Unrecognized name but the value is: ",temp);
    }
    return output;
}
