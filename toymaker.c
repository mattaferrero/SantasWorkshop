/* The Toymaker: a command-line utility designed for parsing and editing linux ELF binaries 
 *
 * Author (Very Important, Must Include in Source): Matt A. Ferrero.
 * 
 * WARNING: This software is designed to be a TEST VIRUS alongisde it's normal function of
 * handling ELF files. IF MODIFIED OR USED INCORRECTLY THIS SOFTWARE _CAN_ AND _WILL_ DESTROY
 * COMPUTER SYSTEMS. I repeat: this software is SOLEY used for testing purposes. No malicious
 * intent is included with this software. Modify and use at your own risk. You have been warned.
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

int main(int argc, char *argv[]) {  

    int                 ctr = 0;
    int                 fd = 0;
    int                 status = 0;

    /* This variable grouping will be used for *mptr's typecastings. */
    unsigned char       *temp = NULL;

    unsigned char       magic_num[EI_NIDENT];
    memset(&magic_num[0], 0, sizeof(magic_num));

    struct  stat        finfo_buf = {0};
    Elf32_Ehdr          Elf32_hdr = {0};
    Elf64_Ehdr          Elf64_hdr = {0};

    void                *mptr = NULL;

    /* To do: implement args/flags later, do more specific error checking */
    if (argc != MAX_ARGS) {
        fprintf(stderr, "Error: exceeded maximum number of arguments. Please man toymaker. Terminating.\n");
        exit(EXIT_SUCCESS);
    }

    /* mmap() requires a file descriptor, so we are using open() instead for convenience instead of fopen(). */
    fd = open(argv[1], O_RDWR); /* O_RDWR read/write. other flags are read only/write only. */
    if (fd == -1) {
        fprintf(stderr, "Error: Could not open file. Terminating.\n");
        return 1;
    }

    status = fstat(fd, &finfo_buf);
    if (status == -1) {
        fprintf(stderr, "Error: Could not grab file status. Terminating.\n");
        
        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after fstat() call. Terminating.\n");
            _exit(1); /* Something has gone horribly, horribly wrong somehow. */
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
        fprintf(stderr, "Error: Mapping failed. Terminating.\n");

        if (close(fd) == -1) {
            fprintf(stderr, "Error: Could not close file descriptor after mmap() call. Terminating.\n");
            _exit(1);
        }
        return 1;
    }

    /* We can close the file descriptor without invaliding the mapping. */
    if (close(fd) == -1) {
        fprintf(stderr, "Error: Could not close file descriptor. Terminating.\n");
        return 1;
    }

    /* Sanity checking for magic number and 32 or 64 bit */
    temp = mptr;

    while (ctr < EI_NIDENT) {
        magic_num[ctr] = temp[ctr];
        ctr++;
    }

    if (magic_num[0] != ELFMAG0 || magic_num[1] != ELFMAG1 || magic_num[2] != ELFMAG2 || magic_num[3] != ELFMAG3) {
        fprintf(stderr, "Error: Target file not ELF object. Terminating.\n");
        return 1;
    }

    if (magic_num[4] == ELFCLASSNONE) {
        fprintf(stderr, "Error: Target ELF file invalid or corrupted class. Terminating.\n");
        return 1;
    }

    else if (magic_num[4] == ELFCLASS32) {
        /* load header into 32 bit struct */
    }

    else if (magic_num[4] == ELFCLASS64) {
        /* Load header into 64 bit struct */
    }

    
    if (munmap(mptr, finfo_buf.st_size) == -1) {
        fprintf(stderr, "Error: Could not unmap file. Terminating.\n");
        return 1;
    }

return 0;
}