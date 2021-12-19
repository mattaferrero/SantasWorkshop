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
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAX_ARGS 2

int main(int argc, char *argv[]) {  

    int fd = 0;
    int maplen = 0;
    void *mptr = NULL;

    /* To do: implement args/flags later, do more specific error checking */
    if (argc != MAX_ARGS) {
        fprintf(stderr, "Error: exceeded maximum number of arguments. Please man toymaker. Terminating.\n");
        exit(EXIT_SUCCESS);
    }

    /* 
     * mmap() requires a file descriptor, so we are using open() instead for convenience instead of fopen(). 
     * I _think_ a successful return of fopen() means the file does exist. docs were _not_ helpful.
     */
    fd = open(argv[1], O_RDWR); /* O_RDWR read/write. other flags are read only/write only. */
    if (fd == -1) {
        fprintf(stderr, "Could not open file, terminating.\n");
        return 1;
    }

    /* 
     * We will load the file into memory with mmap(). Since mmap() does some behind the scenes magic and
     * doesn't actually load the ENTIRE file until we try accessing those parts of it, we don't care about
     * file size and can just do a sanity check for the ELF header contents before mucking around.
     */
    mptr = mmap(NULL, maplen, PROT_EXEC, MAP_PRIVATE, fd, 0); /* <-- this needs to be fixed stat. grab file data first. */
    if (mptr == MAP_FAILED) {
        fprintf(stderr, "Mapping failed. Terminating.\n");
        return 1;
    }

    if (close(fd) == -1) {
        fprintf(stderr, "Could not close file descriptor. Terminating.\n");
        return 1;
    }
return 0;
}