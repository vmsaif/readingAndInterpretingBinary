
# Reading and interpreting binary files (ELF)
One of the first things an operating system does when launching a new process is to read the program into memory. Some operating systems (including Linux) use ELF formatted binaries to represent compiled programs.

The ELF header contains important information about the compiled program. The program extracts and prints the following information from the ELF header:

# What It Prints
ELF Header
- The program's bitness (32-bit or 64-bit).
- The endianness of the file.
- The target operating system's ABI (Application Binary Interface).
- The object file type.
- The instruction set architecture.
- The address of the entry point, which is where the process starts executing.
- The address of the program header table in the file.
- The size and number of entries in the program header table.
- The size and number of entries in the section header table.
- The entry in the section headers that is the string table.

## Program Headers
For each program header, the program prints out:

- The segment type.
- The virtual address of the segment in memory.
- The size in the file image of the program header.
- The first (up to) 32 bytes of actual segment data, formatted as hexadecimal bytes. 

## Section Headers
For each section header, your program should print out:

- The name of the section.
- The ELF header specifies which section header is the string table section header
- The section header type.
- The virtual address of the section in memory.
- The size in the file image of the section header.
- The first (up to) 32 bytes of actual section data, formatted as hexadecimal bytes.

## Compiling and Running

on the terminal, to compile,

```
    clang -o elf_reader elf_reader.c
```
To run write ./{filename} followed by {binary file}, e,g->
```
    ./elf_reader hello.out32
```
Note that, there are 2 sample binary files in the project repository.

## 32-bit binaries
You are able to compile 32-bit binaries using gcc and clang by passing the -m32 option:

    clang -m32 hello.c -o hello.out32 # compile the 32-bit binary
    
Sample Output:

    ./elf_reader hello.out64
    File: hello.out64

    ELF header:
            * 64-bit
            * little endian
            * Compiled for 0x00 (operating system)
            * has type 0x02
            * Compiled for 0x3e (isa)
            * entry point address 0x401040
            * program header table starts at 0x40
            * There are 11 program headers, each is 56 bytes
            * There are 29 section headers, each is 64 bytes
            * the section header string table is 28

## Program Headers
    Program header: #4
        * segment type 0x00000001
        * virtual address of segment 0x402000
        * size in file 312 bytes
        * first up to 32 bytes starting at 0x2000
          01 00 02 00 48 65 6c 6c 6f 2c 20 77 6f 72 6c 64 
          21 0a 00 00 01 1b 03 3b 38 00 00 00 06 00 00 00

## Section Headers
    Section header: #2
        * section name .note.gnu.build-id
        * type 0x07
        * virtual address of segment 0x4002c4
        * size in file 36 bytes
        * first up to 32 bytes starting at 0x2c4
          04 00 00 00 14 00 00 00 03 00 00 00 47 4e 55 00 
          1c b9 d1 a6 25 3b cf 34 18 c3 78 cd d1 58 9b 5c

        .
        .
        .
        .
        .


```
    Program exited with following code: Success
    Program written by Saif Mahmud.
```
