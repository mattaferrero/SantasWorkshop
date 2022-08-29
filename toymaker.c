/* 
 * The Toymaker: a command-line utility designed for parsing and editing linux ELF binaries 
 *
 * Author: Matt A. Ferrero.
 * 
 * WARNING: This software is designed to be a TEST VIRUS alongisde it's normal function of
 * handling ELF files. IF MODIFIED OR USED INCORRECTLY THIS SOFTWARE _CAN_ AND _WILL_ DESTROY
 * COMPUTER SYSTEMS. I repeat: this software is SOLEY used for testing purposes. No malicious
 * intent is included with this software. Modify and use at your own risk. You have been warned.
 * 
 * Program Execution Flow:
 * 
 * 1) TO DO: COMMAND LINE ARGS FOR TARGET BINARIES
 * 
 * 2) main() will load the target binary to be modified into memory with mmap(), and call 
 * ehdr_sane_check() to validate each individual header member, and to determine if the
 * binary is 32 or 64 bit. 
 * 
 * 3) ehdr_sane_check() will print out basic header information similar to objdump -h, then 
 * return to main().
 * 
 * 4) either elf32_object() or elf64_object() will be called where modification of the
 * binary is permitted.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <elf.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_ARGS        2

int ehdr_sane_check(void *mptr);

int elf32_object(void *mptr);
int elf64_object(void *mptr);

int main(int argc, char *argv[]) {  

    int                 fd = 0; /* open()'s return val */
    int                 status = 0; /* fstat()'s return val */
    int                 sane = 0; /* ehdr_sane_check()'s return val */

    /* This variable grouping will be used for *mptr's typecastings. 
    Elf32_Ehdr          *Elf32_hdr = NULL;
    Elf64_Ehdr          *Elf64_hdr = NULL;
*/
    /*
    unsigned char       magic_num[EI_NIDENT];
    memset(&magic_num[0], 0, sizeof(magic_num));
*/
    struct  stat        finfo_buf = {0};

    void                *mptr = NULL;

    /* To do: implement args/flags later, do more specific error checking */
    if (argc != MAX_ARGS) {
        fprintf(stderr, "Error: exceeded maximum number of arguments. Please man toymaker.\n");
        return 1;
    }

    /* mmap() requires a file descriptor, so we are using open() instead for convenience instead of fopen(). */
    fd = open(argv[1], O_RDWR); /* O_RDWR read/write. other flags are read only/write only. */
    if (fd == -1) {
        fprintf(stderr, "Error: Could not open file.\n");
        return 1;
    }

    status = fstat(fd, &finfo_buf);
    if (status == -1) {
        fprintf(stderr, "Error: Could not grab file status.\n");
        
        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after fstat() call.\n");
            return 1; /* Something has gone horribly, horribly wrong somehow. (yes this is redundant, im aware). */
        }
        return 1;
    }

    /* 
     * We will load the file into memory with mmap(). Since mmap() does some behind the scenes magic and
     * doesn't actually load the ENTIRE file until we try accessing those parts of it, we don't care about
     * file size and can just do a sanity check for the ELF header contents before mucking around.
     */
    mptr = mmap(NULL, finfo_buf.st_size, PROT_EXEC, MAP_PRIVATE, fd, 0); /* <-- this needs to be fixed stat. grab file data first. */
    if (mptr == MAP_FAILED) {
        fprintf(stderr, "Error: Mapping failed.\n");

        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after mmap() call.\n");
            return 1;
        }
        return 1;
    }

    /* We can close the file descriptor without invaliding the mapping. */
    if (close(fd) == -1) {
        fprintf(stderr, "Error: Could not close file descriptor.\n");
        return 1;
    }

    sane = ehdr_sane_check(mptr); 
    switch(sane) {
        case 0 :
            fprintf(stderr, "Fatal Error: Could not parse EI_CLASS. Closing program.\n");
            break;
        case 1 :
            elf32_object(mptr); 
            break;
        case 2 :
            elf64_object(mptr);
            break; 
    }

    /*
    
    This will be done in ehdr_sane_check() instead.
    tmp = mptr;

    while (ctr < EI_NIDENT) {
        magic_num[ctr] = tmp[ctr];
        ctr++;
    }

    if (magic_num[0] != ELFMAG0 || magic_num[1] != ELFMAG1 || magic_num[2] != ELFMAG2 || magic_num[3] != ELFMAG3) {
        fprintf(stderr, "Error: Target file not ELF object.\n");
        return 1;
    }

    if (magic_num[4] == ELFCLASSNONE) {
        fprintf(stderr, "Error: Target ELF file invalid or corrupted class.\n");
        return 1;
    }

    else if (magic_num[4] == ELFCLASS32) {
        Elf32_hdr = mptr;        
        elf32_object(mptr, Elf32_hdr);        
    }

    else if (magic_num[4] == ELFCLASS64) {
        Elf64_hdr = mptr;        
        elf64_object(mptr, Elf64_hdr);
    }
    */
    
    if (munmap(mptr, finfo_buf.st_size) == -1) {
        fprintf(stderr, "Error: Could not unmap file.\n");
        return 1;
    }

return 0;
}

int ehdr_sane_check(void *mptr) {


}

int elf32_object(void *mptr) {

return 0;
}

int elf64_object(void *mptr) {

return 0;
}
